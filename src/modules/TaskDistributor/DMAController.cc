#include <string>
#include <cassert>
#include <iostream>
#include "../LCAcc/LCAccCommandListing.hh"
#include "DMAController.hh"
#include "gem5Interface.hh"
#include "../MsgLogger/MsgLogger.hh"
#include "mem/ruby/common/Global.hh"
#include "arch/isa_traits.hh"
#include "../Common/BitConverter.hh"

const int opmode_read = 1;
const int opmode_write = 2;
const int opmode_send = 3;
const int opmode_recv = 4;
const uint64_t tlbPageSize = TheISA::PageBytes;

uint64_t DMAController::GetPageAddr(uint64_t addr) const
{
  return addr - (addr % tlbPageSize);
}
uint64_t DMAController::OnAddressTranslate(uint64_t logicalAddr)
{
  uint64_t pageAddr = GetPageAddr(logicalAddr);
  assert(logicalAddr >= pageAddr);
  uint64_t pageOffset = logicalAddr - pageAddr;

  if (tlbMap.find(pageAddr) != tlbMap.end()) {
    return tlbMap[pageAddr] + pageOffset;
  }

  return 0;
}
void DMAController::OnTLBMiss(uint64_t logicalAddr)
{
  uint64_t pageAddr = GetPageAddr(logicalAddr);

  if (pendingTLBMisses.find(pageAddr) == pendingTLBMisses.end()) {
    pendingTLBMisses.insert(pageAddr);
    onTLBMiss->Call(pageAddr);
  }
}
void DMAController::OnAccessError(uint64_t logicalAddr)
{
  onAccViolation->Call(logicalAddr);
}
void DMAController::OnNetworkMsg(int src, const void* buffer, unsigned int bufSize)
{
  assert(bufSize >= 4);
  assert(buffer);
  uint32_t* b = (uint32_t*)buffer;
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

    for (size_t i = 0; i < localSignals.size(); i++) {
      //                                std::cout << "Comparing : " << localSignals[i].dstAddr << " to " << target.dstAddr << " : " << localSignals[i].requestedAddr << " to " << target.requestedAddr << " : " << localSignals[i].ID << " to " << target.ID << " : " << localSignals[i].size << " to " << target.size << std::endl;
      if (localSignals[i].Matches(target)) {
        std::vector<uint32_t> msg;
        msg.push_back(DMA_MEMORY_RESPONSE);
        bc_request.u64[0] = localSignals[i].requestedAddr;
        msg.push_back(bc_request.u32[0]);
        msg.push_back(bc_request.u32[1]);
        bc_dst.u64[0] = localSignals[i].dstAddr;
        msg.push_back(bc_dst.u32[0]);
        msg.push_back(bc_dst.u32[1]);
        msg.push_back(localSignals[i].size);

        for (unsigned int j = 0; j < localSignals[i].size / sizeof(uint32_t) + ((localSignals[i].size % sizeof(uint32_t)) ? 1 : 0); j++) {
          msg.push_back(0);
        }

        spm->Read(localSignals[i].requestedAddr, localSignals[i].size, &(msg[6]));
        network->SendMessage(localSignals[i].ID, &(msg[0]), msg.size() * sizeof(int32_t));
        TaskDistributor::gem5Interface::RegisterCallback(localSignals[i].onFinish, 0);
        localSignals.erase(localSignals.begin() + i);
        return;
      }
    }

    remoteSignals.push_back(target);
  } else if (opcode == DMA_MEMORY_RESPONSE) {
    assert(bufSize >= (int)sizeof(int32_t) * 4);
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

    //std::cout << "localSignals.size()(" << localSignals.size() << ")" << std::endl;
    for (std::vector<SignalEntry>::iterator i = localSignals.begin(); i != localSignals.end(); i++) {
      if (i->Matches(r)) {
        resolved = true;
        assert(i->onFinish);
        TaskDistributor::gem5Interface::RegisterCallback(i->onFinish, 0);
        spm->Write(r.dstAddr, r.size, &(args[5]));
        localSignals.erase(i);
        return;
      }
    }

    std::cerr << "DMA Memory Response came unsolicited" << std::endl;
    assert(0);
  }
}
DMAController::DMAController(NetworkInterface* ni, SPMInterface* spm, Arg1CallbackBase<uint64_t>* TLBMiss, Arg1CallbackBase<uint64_t>* accessViolation)
{
  assert(ni);
  assert(spm);
  std::cout << "TD:DMAController using g_TdDmaDevice " << spm->GetID() << std::endl;
  dmaDevice = g_TdDmaDevice[spm->GetID()];
  assert(dmaDevice);
  dmaInterface = g_dmaInterface;
  assert(dmaInterface);
  network = ni;
  onAccViolation = accessViolation;
  onTLBMiss = TLBMiss;
  this->spm = spm;
  dmaInterface->Configure(dmaDevice, ni->GetNetworkPort(), spm->GetID(), OnTLBMissCB::Create(this), OnAddressTranslateCB::Create(this), OnAccessErrorCB::Create(this));
  ni->RegisterRecvHandler(OnNetworkMsgCB::Create(this));
}
DMAController::~DMAController()
{
  delete dmaInterface;
}
void DMAController::BeginTransfer(int srcSpm, uint64_t srcAddr, const std::vector<unsigned int>& srcSize, const std::vector<int>& srcStride, int dstSpm, uint64_t dstAddr, const std::vector<unsigned int>& dstSize, const std::vector<int>& dstStride, size_t elementSize, CallbackBase* finishCB)
{
  BeginTransfer(srcSpm, srcAddr, srcSize, srcStride, dstSpm, dstAddr, dstSize, dstStride, elementSize, 0, finishCB);
}
void DMAController::BeginTransfer(int srcSpm, uint64_t srcAddr, const std::vector<unsigned int>& srcSize, const std::vector<int>& srcStride, int dstSpm, uint64_t dstAddr, const std::vector<unsigned int>& dstSize, const std::vector<int>& dstStride, size_t elementSize, int priority, CallbackBase* finishCB)
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
    dmaInterface->StartTransferPrio(dmaDevice, srcSpm, srcAddr, srcSize.size(), &(srcSize[0]), &(srcStride[0]), dstSpm, dstAddr, dstSize.size(), &(dstSize[0]), &(dstStride[0]), elementSize, priority, -1, finishCB);
  } else if (dstSpm == spm->GetID()) {
    //                        std::cout << "Chain Read starting... " << network->GetNetworkPort() << " from [" << srcSpm << "] " << srcAddr << " to " << dstAddr << std::endl;
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
    //                       std::cout << "Chain Write starting... " << network->GetNetworkPort() << std::endl;
    SignalEntry target;
    target.ID = dstSpm;
    target.dstAddr = dstAddr;
    target.requestedAddr = srcAddr;
    target.size = elementSize;

    for (size_t i = 0; i < srcSize.size(); i++) {
      target.size *= srcSize[i];
    }

    target.onFinish = finishCB;

    for (size_t i = 0; i < remoteSignals.size(); i++) {
      //                                std::cout << "Comparing : " << remoteSignals[i].dstAddr << " to " << target.dstAddr << " : " << remoteSignals[i].requestedAddr << " to " << target.requestedAddr << " : " << remoteSignals[i].ID << " to " << target.ID << " : " << remoteSignals[i].size << " to " << target.size << std::endl;
      if (remoteSignals[i].Matches(target)) {
        std::vector<uint32_t> msg;
        msg.push_back(DMA_MEMORY_RESPONSE);
        BitConverter bc_request;
        bc_request.u64[0] = remoteSignals[i].requestedAddr;
        msg.push_back(bc_request.u32[0]);
        msg.push_back(bc_request.u32[1]);
        BitConverter bc_dst;
        bc_dst.u64[0] = remoteSignals[i].dstAddr;
        msg.push_back(bc_dst.u32[0]);
        msg.push_back(bc_dst.u32[1]);
        msg.push_back(remoteSignals[i].size);

        for (unsigned int j = 0; j < remoteSignals[i].size / sizeof(uint32_t) + ((remoteSignals[i].size % sizeof(uint32_t)) ? 1 : 0); j++) {
          msg.push_back(0);
        }

        spm->Read(remoteSignals[i].requestedAddr, remoteSignals[i].size, &(msg[6]));
        network->SendMessage(remoteSignals[i].ID, &(msg[0]), msg.size() * sizeof(int32_t));
        TaskDistributor::gem5Interface::RegisterCallback(target.onFinish, 0);
        remoteSignals.erase(remoteSignals.begin() + i);
        return;
      }
    }

    //std::cout << "localSignals added" << std::endl;
    localSignals.push_back(target);
  }
}
void DMAController::FlushTLB()
{
  tlbMap.clear();
}
void DMAController::AddTLBEntry(uint64_t logical, uint64_t physical)
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
void DMAController::BeginSingleElementTransfer(int srcSpm, uint64_t src, int dstSpm, uint64_t dst, uint32_t size, CallbackBase* finishedCB)
{
  BeginSingleElementTransfer(srcSpm, src, dstSpm, dst, size, 0, finishedCB);
}
void DMAController::BeginSingleElementTransfer(int srcSpm, uint64_t src, int dstSpm, uint64_t dst, uint32_t size, int priority, CallbackBase* finishedCB)
{
  assert(srcSpm == -1 || dstSpm == -1);
  dmaInterface->StartSingleTransferPrio(dmaDevice, srcSpm, src, dstSpm, dst, size, priority, -1, finishedCB);
}

