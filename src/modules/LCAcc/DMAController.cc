#include <string>
#include <cassert>
#include <iostream>

#include "DMAController.hh"
#include "LCAccCommandListing.hh"
#include "gem5Interface.hh"
#include "../MsgLogger/MsgLogger.hh"
#include "../../mem/ruby/common/Global.hh"
#include "arch/isa_traits.hh"
#include "../Common/BitConverter.hh"

using namespace LCAcc;

const int opmode_read = 1;
const int opmode_write = 2;
const int opmode_send = 3;
const int opmode_recv = 4;
const uint64_t tlbPageSize = TheISA::PageBytes;

namespace LCAcc
{
const int DMAController::AccessType::Read = 0;
const int DMAController::AccessType::Write = 1;
const int DMAController::AccessType::ReadLock = 3;
const int DMAController::AccessType::WriteLock = 4;
const int DMAController::AccessType::WriteUnlock = 5;
const int DMAController::AccessType::Unlock = 6;

uint64_t
DMAController::GetPageAddr(uint64_t addr) const
{
  return addr - (addr % tlbPageSize);
}

uint64_t
DMAController::OnAddressTranslate(uint64_t logicalAddr)
{
  uint64_t pageAddr = GetPageAddr(logicalAddr);
  assert(logicalAddr >= pageAddr);
  uint64_t pageOffset = logicalAddr - pageAddr;

  if (tlbMap.find(pageAddr) != tlbMap.end()) {
    return tlbMap[pageAddr] + pageOffset;
  }

  return 0;
}

void
DMAController::OnTLBMiss(uint64_t logicalAddr)
{
  uint64_t pageAddr = GetPageAddr(logicalAddr);

  if (pendingTLBMisses.find(pageAddr) == pendingTLBMisses.end()) {
    pendingTLBMisses.insert(pageAddr);
    onTLBMiss->Call(pageAddr);
  }
}

void
DMAController::OnAccessError(uint64_t logicalAddr)
{
  onAccViolation->Call(logicalAddr);
}

void
DMAController::OnNetworkMsg(int src, const void* buf, unsigned int bufSize)
{
  assert(bufSize >= 4);
  assert(buf);
  uint32_t* b = (uint32_t*)buf;
  uint32_t opcode = b[0];

  if (opcode == DMA_MEMORY_REQUEST) {
    assert(bufSize == sizeof(uint32_t) * 6);
    SignalEntry target;
    target.ID = src;
    BitConverter bc_dst;
    bc_dst.u32[0] = b[1];
    bc_dst.u32[1] = b[2];
    target.dstAddr = bc_dst.u64[0];
    BitConverter bc_request;
    bc_request.u32[0] = b[3];
    bc_request.u32[1] = b[4];
    target.requestedAddr = bc_request.u64[0];
    target.size = b[5];
    target.onFinish = NULL;

    for (std::vector<SignalEntry>::iterator i = localSignals.begin(); i != localSignals.end(); i++) {
      if (i->Matches(target)) {
        std::vector<uint32_t> msg;
        msg.push_back(DMA_MEMORY_RESPONSE);
        bc_request.u64[0] = i->requestedAddr;
        msg.push_back(bc_request.u32[0]);
        msg.push_back(bc_request.u32[1]);
        bc_dst.u64[0] = i->dstAddr;
        msg.push_back(bc_dst.u32[0]);
        msg.push_back(bc_dst.u32[1]);
        msg.push_back(i->size);

        for (unsigned int j = 0; j < i->size / sizeof(uint32_t) + ((i->size % sizeof(uint32_t)) ? 1 : 0); j++) {
          msg.push_back(0);
        }

        spm->Read(i->requestedAddr, i->size, &(msg[6]));
        network->SendMessage(i->ID, &(msg[0]), msg.size() * sizeof(int32_t));
        assert(i->onFinish);
        gem5Interface::RegisterCallback(i->onFinish, 0);
        localSignals.erase(i);
        return;
      }
    }

    remoteSignals.push_back(target);
  } else if (opcode == DMA_MEMORY_RESPONSE) {
    assert(bufSize >= (int)sizeof(int32_t) * 6);
    uint32_t* args = (uint32_t*) & (b[1]);
    SignalEntry r;
    r.ID = src;
    BitConverter bc_dst;
    bc_dst.u32[0] = args[2];
    bc_dst.u32[1] = args[3];
    r.dstAddr = bc_dst.u64[0];
    BitConverter bc_request;
    bc_request.u32[0] = args[0];
    bc_request.u32[1] = args[1];
    r.requestedAddr = bc_request.u64[0];
    r.size = args[4];
    assert(bufSize == sizeof(uint32_t) * 6 + r.size);
    bool resolved = false;

    for (std::vector<SignalEntry>::iterator i = localSignals.begin(); i != localSignals.end(); i++) {
      if (i->Matches(r)) {
        resolved = true;
        assert(i->onFinish);
        gem5Interface::RegisterCallback(i->onFinish, 0);
        spm->Write(r.dstAddr, r.size, &(args[5]));
        localSignals.erase(i);
        return;
      }
    }

    std::cerr << "DMA Memory Response came unsolicited" << std::endl;
    assert(0);
  }
}

DMAController::DMAController(NetworkInterface* ni, SPMInterface* spm,
  Arg1CallbackBase<uint64_t>* TLBMiss,
  Arg1CallbackBase<uint64_t>* accessViolation)
{
  dmaDevice = g_dmaDevice[RubySystem::deviceIDtoAccID(spm->GetID())];
  dmaInterface = g_dmaInterface;
  network = ni;
  onAccViolation = accessViolation;
  onTLBMiss = TLBMiss;
  buffer = -1;
  this->spm = spm;

  dmaInterface->Configure(dmaDevice, ni->GetNodeID(), spm->GetID(),
    OnTLBMissCB::Create(this), OnAddressTranslateCB::Create(this),
    OnAccessErrorCB::Create(this));

  ni->RegisterRecvHandler(OnNetworkMsgCB::Create(this));

  isHookedToMemory = false;
}

DMAController::~DMAController()
{
  if (isHookedToMemory) {
    assert(dmaDevice);
    assert(dmaInterface);
    dmaInterface->UnhookMemoryPort(dmaDevice);
  }
}

void
DMAController::BeginTransfer(int srcSpm, uint64_t srcAddr,
  const std::vector<unsigned int>& srcSize, const std::vector<int>& srcStride,
  int dstSpm, uint64_t dstAddr, const std::vector<unsigned int>& dstSize,
  const std::vector<int>& dstStride, size_t elementSize, CallbackBase* finishCB)
{
  BeginTransfer(srcSpm, srcAddr, srcSize, srcStride,
    dstSpm, dstAddr, dstSize, dstStride, elementSize, 0, finishCB);
}

void
DMAController::BeginTransfer(int srcSpm, uint64_t srcAddr,
  const std::vector<unsigned int>& srcSize, const std::vector<int>& srcStride,
  int dstSpm, uint64_t dstAddr, const std::vector<unsigned int>& dstSize,
  const std::vector<int>& dstStride, size_t elementSize, int priority,
  CallbackBase* finishCB)
{
  assert(finishCB);
  assert(srcSize.size() == srcStride.size());
  assert(dstSize.size() == dstStride.size());
  assert(srcSize.size() > 0);
  assert(dstSize.size() > 0);
  assert(elementSize <= 8);
  assert(elementSize > 0);
  assert(srcSpm != -1 || dstSpm != -1);
  assert(srcSpm == spm->GetID() || dstSpm == spm->GetID());

  if (srcSpm == -1 || dstSpm == -1) {
    dmaInterface->StartTransferPrio(dmaDevice, srcSpm, srcAddr, srcSize.size(),
      &(srcSize[0]), &(srcStride[0]), dstSpm, dstAddr, dstSize.size(),
      &(dstSize[0]), &(dstStride[0]), elementSize, priority, buffer, finishCB);
  } else if (dstSpm == spm->GetID()) {
    uint32_t msg[6];
    msg[0] = DMA_MEMORY_REQUEST;
    BitConverter bc_dst;
    bc_dst.u64[0] = dstAddr;
    msg[1] = bc_dst.u32[0];
    msg[2] = bc_dst.u32[1];
    BitConverter bc_src;
    bc_src.u64[0] = srcAddr;
    msg[3] = bc_src.u32[0];
    msg[4] = bc_src.u32[1];
    uint32_t size = elementSize;

    for (size_t i = 0; i < dstSize.size(); i++) {
      size *= dstSize[i];
    }

    msg[5] = size;
    network->SendMessage(srcSpm, msg, 6 * sizeof(uint32_t));
    SignalEntry target;
    target.ID = srcSpm;
    target.dstAddr = dstAddr;
    target.requestedAddr = srcAddr;
    target.size = elementSize;

    for (size_t i = 0; i < srcSize.size(); i++) {
      target.size *= srcSize[i];
    }

    target.onFinish = finishCB;
    localSignals.push_back(target);
  } else { //srcSpm == id
    assert(srcSpm == spm->GetID());
    SignalEntry target;
    target.ID = dstSpm;
    target.dstAddr = dstAddr;
    target.requestedAddr = srcAddr;
    target.size = elementSize;

    for (size_t i = 0; i < srcSize.size(); i++) {
      target.size *= srcSize[i];
    }

    target.onFinish = finishCB;

    for (std::vector<SignalEntry>::iterator i = remoteSignals.begin(); i != remoteSignals.end(); i++) {
      if (i->Matches(target)) {
        std::vector<uint32_t> msg;
        msg.push_back(DMA_MEMORY_RESPONSE);
        BitConverter bc_request;
        bc_request.u64[0] = i->requestedAddr;
        msg.push_back(bc_request.u32[0]);
        msg.push_back(bc_request.u32[1]);
        BitConverter bc_dst;
        bc_dst.u64[0] = i->dstAddr;
        msg.push_back(bc_dst.u32[0]);
        msg.push_back(bc_dst.u32[1]);
        msg.push_back(i->size);

        for (unsigned int j = 0; j < i->size / sizeof(uint32_t) + ((i->size % sizeof(uint32_t)) ? 1 : 0); j++) {
          msg.push_back(0);
        }

        spm->Read(i->requestedAddr, i->size, &(msg[6]));
        network->SendMessage(i->ID, &(msg[0]), msg.size() * sizeof(int32_t));
        gem5Interface::RegisterCallback(target.onFinish, 0);
        assert(i->onFinish == NULL);
        remoteSignals.erase(i);
        return;
      }
    }

    localSignals.push_back(target);
  }
}

void
DMAController::PrefetchMemory(uint64_t baseAddr,
  const std::vector<unsigned int>& size, const std::vector<int>& stride,
  size_t elementSize)
{
  dmaInterface->Prefetch(dmaDevice, baseAddr, size.size(),
    &(size[0]), &(stride[0]), elementSize);
}

void
DMAController::SetBuffer(int buf)
{
  buffer = buf;
}

void
DMAController::FlushTLB()
{
  tlbMap.clear();
}

void
DMAController::AddTLBEntry(uint64_t logical, uint64_t physical)
{
  uint64_t lPageAddr = GetPageAddr(logical);
  uint64_t pPageAddr = GetPageAddr(physical);
  uint64_t pageOffset = logical - lPageAddr;
  assert(logical >= lPageAddr);
  assert(physical >= pPageAddr);
  assert(logical - lPageAddr == physical - pPageAddr);
  assert(physical > pageOffset);
  assert(tlbMap.find(lPageAddr) == tlbMap.end() || tlbMap[lPageAddr] == pPageAddr);
  tlbMap[lPageAddr] = pPageAddr;

  if (pendingTLBMisses.find(lPageAddr) != pendingTLBMisses.end()) {
    pendingTLBMisses.erase(lPageAddr);
    dmaInterface->Restart(dmaDevice);
  }
}

void
DMAController::HookToMemoryController(const std::string& deviceName)
{
  assert(dmaInterface);
  assert(dmaDevice);

  if (isHookedToMemory) {
    dmaInterface->UnhookMemoryPort(dmaDevice);
  }

  dmaInterface->HookToMemoryPort(dmaDevice, deviceName.c_str());
}

void
DMAController::BeginSingleElementTransfer(int mySPM, uint64_t spmAddr,
  uint64_t memAddr, uint32_t size, int type, CallbackBase* finishedCB)
{
  //std::cout << "Accessing single element " << spmAddr << std::endl;
  bool isRead = (type == AccessType::Read || type == AccessType::ReadLock);
  bool isLock = (type == AccessType::ReadLock || type == AccessType::WriteLock);
  bool isUnlock = (type == AccessType::WriteUnlock || type == AccessType::Unlock);
  assert(!isLock || !isUnlock);
  assert(!isLock);//for now, since locking is disabled
  assert(!isUnlock);//for now, since locking is disabled

  dmaInterface->StartSingleTransferPrio(dmaDevice,
    isRead ? -1 : mySPM, isRead ? memAddr : spmAddr,
    isRead ? mySPM : -1, isRead ? spmAddr : memAddr,
    size, 0, buffer, finishedCB);
}
}
