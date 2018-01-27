#include <set>
#include "DMAEngine.hh"
#include "../Common/mf_api.hh"
#include "../MsgLogger/MsgLogger.hh"
#include "arch/isa_traits.hh"
#include "mem/ruby/system/Sequencer.hh"
#include "mem/ruby/system/RubyPort.hh"
#include "mem/ruby/system/System.hh"
#include "mem/protocol/RequestStatus.hh"
#include "mem/ruby/system/gem5NetworkPort.hh"

//#define SIM_PAGE_FAULT
#ifdef SIM_PAGE_FAULT
#include "sim/system.hh"
#include "arch/vtophys.hh"
#endif

#define PAGE_SIZE (TheISA::PageBytes)
#define BLOCK_SIZE 64
#define NO_SPM_ID -1

TransferEndPointDescription::TransferEndPointDescription(int spm, uint64_t base,
  const std::vector<uint32_t>& size, const std::vector<int32_t>& stride)
  : addrGen(base, size, stride)
{
  this->spm = spm;
}

TransferData::TransferData(int srcSpmID, uint64_t srcLAddr, int dstSpmID,
  uint64_t dstLAddr, size_t elemSize, int prio, uint64_t when, int buffer,
  CallbackBase* onFinish)
{
  this->srcSpmID = srcSpmID;
  this->srcLAddr = srcLAddr;
  this->dstSpmID = dstSpmID;
  this->dstLAddr = dstLAddr;
  this->elementSize = elemSize;
  this->priority = prio;
  this->timeStamp = when;
  this->onFinish = onFinish;
  this->buffer = buffer;
}

void
TransferSetDesc::DecrementPending()
{
  assert(pendingAccesses > 0);
  pendingAccesses--;
  if(pendingAccesses == 0)
  {
    ScheduleCB(0, onFinish);
    finished = true;
  }
}

TransferSetDesc::TransferSetDesc(int spmSrc, uint64_t baseSrc,
  const std::vector<uint32_t>& sizeSrc, const std::vector<int32_t>& strideSrc,
  int spmDst, uint64_t baseDst, const std::vector<uint32_t>& sizeDst,
  const std::vector<int32_t>& strideDst, size_t transferSize, int prio,
  uint64_t when, int buffer, CallbackBase* finish)
  : start(spmSrc, baseSrc, sizeSrc, strideSrc), end(spmDst, baseDst, sizeDst, strideDst)
{
  assert(finish);
  assert(transferSize > 0);
  assert(transferSize <= 8);
  assert(spmSrc != NO_SPM_ID || spmDst != NO_SPM_ID);
  assert(spmSrc == NO_SPM_ID || spmDst == NO_SPM_ID);
  assert(start.addrGen.TotalSize() == end.addrGen.TotalSize());
  this->transferSize = transferSize;
  this->onFinish = finish;
  this->buffer = buffer;
  index = 0;
  priority = prio;
  finished = false;
  timeStamp = when;
  pendingAccesses = start.addrGen.TotalSize();
}

bool
TransferSetDesc::MoreTransfers() const
{
  return start.addrGen.TotalSize() > index;
}

TransferData* TransferSetDesc::NextTransfer()
{
  int i = index;
  index++;
  uint64_t srcAddr = start.addrGen.GetAddr(i);
  uint64_t dstAddr = end.addrGen.GetAddr(i);
  return new TransferData(start.spm, srcAddr, end.spm, dstAddr, transferSize,
    priority, timeStamp, buffer, DecrementPendingCB::Create(this));
}

bool
TransferSetDesc::IsFinished() const
{
  return finished;
}

bool
TransferSetDesc::operator < (const TransferSetDesc& x) const
{
  if(priority != x.priority) {
    return priority < x.priority;
  } else {
    return timeStamp < x.timeStamp;
  }
}

bool
DMAEngine::IsRedirectingToMemory() const
{
  return memObject != NULL;
}

bool
DMAEngine::IsReady(uint64_t lAddr, uint64_t pAddr, RubyRequestType direction)
{
  if (!IsRedirectingToMemory()) {
    int L1CacheID;
    if (nodeID < RubySystem::numberOfTDs())
      L1CacheID = RubySystem::tdIDtoL1CacheID(nodeID);
    else
      L1CacheID = RubySystem::accIDtoL1CacheID(RubySystem::deviceIDtoAccID(nodeID));
    AbstractController* L1Controller = g_abs_controls[MachineType_L1Cache][L1CacheID];
    Sequencer* seq = L1Controller->getSequencer();
    return seq->isReady(pAddr);
  } else {
    assert(memInterface);
    assert(memObject);
    assert(direction == RubyRequestType_LD || direction == RubyRequestType_ST);
    if (direction == RubyRequestType_LD) {
      return memInterface->IsReadReady(memObject, pAddr, BLOCK_SIZE);
    } else {
      return memInterface->IsWriteReady(memObject, pAddr, BLOCK_SIZE);
    }
  }
}

bool
DMAEngine::IsBufferReady(int bufferId, uint64_t addr, uint64_t dstAddr,
  RubyRequestType direction)
{
  return true;
}

void
DMAEngine::MakeRequest(uint64_t lAddr, uint64_t pAddr,
  RubyRequestType direction, CallbackBase* cb)
{
  if (!IsRedirectingToMemory()) {
    int L1CacheID;
    if(nodeID < RubySystem::numberOfTDs())
      L1CacheID = RubySystem::tdIDtoL1CacheID(nodeID);
    else
      L1CacheID = RubySystem::accIDtoL1CacheID(RubySystem::deviceIDtoAccID(nodeID));

    AbstractController* L1Controller = g_abs_controls[MachineType_L1Cache][L1CacheID];
    Sequencer* seq = L1Controller->getSequencer();
    Request::Flags flags = 0;
    RequestPtr req = new Request(pAddr, BLOCK_SIZE, flags, L1CacheID);
    MemCmd cmd;
    if(direction == RubyRequestType_ST)
      cmd = MemCmd::WriteReq;
    else
      cmd = MemCmd::ReadReq;

    PacketPtr pkt = new Packet(req, cmd);
    RubyPort::SenderState *senderState = new RubyPort::SenderState(NULL);
    pkt->pushSenderState(senderState);
    RequestStatus status = seq->makeRequest(pkt, RubyExecuteCB, cb);
    assert(status == RequestStatus_Issued);
  } else {
    assert(memInterface);
    assert(memObject);
    assert(direction == RubyRequestType_LD || direction == RubyRequestType_ST);

    if (direction == RubyRequestType_LD) {
      memInterface->IssueRead(memObject, pAddr, BLOCK_SIZE, cb);
    } else {
      memInterface->IssueWrite(memObject, pAddr, BLOCK_SIZE, NULL, cb);
    }
  }
}

void
DMAEngine::MakePrefetch(uint64_t lAddr, uint64_t pAddr)
{
  // do nothing
}

void
DMAEngine::MakeBufferCopy(uint64_t cacheAddr, uint64_t bufferAddr,
  int bufferId, RubyRequestType direction, CallbackBase* cb)
{
  // do nothing
}

void
DMAEngine::MemDevInterfaceIntercept(const void*, CallbackBase* cb)
{
  assert(cb);
  cb->Call();
  cb->Dispose();
}

bool
DMAEngine::TranslateAddress(logical_address_t la, physical_address_t& pa)
{
  assert(la);
  pa = translateAddress->Call(la);
  return pa != 0;
}

void
DMAEngine::ReEnqueueTransfer(TransferData* td)
{
  EnqueueTransfer(td);
}

void
DMAEngine::WriteBlock(uint64_t spmAddr, uint64_t pMemAddr, uint64_t lMemAddr,
  size_t size)
{
  uint64_t d;
  spmInterface->read(spm, spmAddr, &d, size);
  WriteMemory(pMemAddr, d, size);
}

void
DMAEngine::ReadBlock(uint64_t pMemAddr, uint64_t lMemAddr, uint64_t spmAddr,
  size_t size)
{
  uint64_t d = ReadMemory(pMemAddr, size);
  spmInterface->write(spm, spmAddr, &d, size);
}

void
DMAEngine::Splice(CallbackBase* cb1, CallbackBase* cb2)
{
  ScheduleCB(0, cb1);
  ScheduleCB(0, cb2);
}

void
DMAEngine::AddTransferSet(int srcDevice, uint64_t srcAddr,
  unsigned int srcDimensions, const unsigned int* srcElementSize,
  const int* srcElementStride, int dstDevice, uint64_t dstAddr,
  unsigned int dstDimensions, const unsigned int* dstElementSize,
  const int* dstElementStride, size_t transferSize, int priority, int buffer,
  CallbackBase* onFinish)
{
  assert(onFinish);
  assert(srcElementStride);
  assert(srcElementSize);
  assert(dstElementStride);
  assert(dstElementSize);

  std::vector<unsigned int> srcSize, dstSize;
  std::vector<int> srcStride, dstStride;

  for (size_t i = 0; i < srcDimensions; i++) {
    srcSize.push_back(srcElementSize[i]);
    srcStride.push_back(srcElementStride[i]);
  }
  for (size_t i = 0; i < dstDimensions; i++) {
    dstSize.push_back(dstElementSize[i]);
    dstStride.push_back(dstElementStride[i]);
  }
  TransferSetDesc* tsd = new TransferSetDesc(srcDevice, srcAddr, srcSize,
    srcStride, dstDevice, dstAddr, dstSize, dstStride, transferSize, priority,
    GetSystemTime(), buffer, onFinish);

#ifdef SIM_PAGE_FAULT
  System *m5_system = *(System::systemList.begin());
  ThreadContext* cpu = m5_system->getThreadContext(0);
  uint64_t pAddr, vAddr;
  int thread = 1;
  for (int i = 0; i < srcSize[0]; i=i+srcStride[0]) {
    vAddr = srcAddr + i;
    if (g_TLBHack_interface && g_TLBHack_interface->PageKnown(thread, vAddr)) {
        pAddr = g_TLBHack_interface->Lookup(thread, vAddr);
    } else {
      pAddr = TheISA::vtophys(cpu, vAddr);
    }

    if (srcStride[0] == 4) {
      uint32_t data;
      gem5Interface::ReadPhysical(pAddr, (void*)&data, 4);
    } else if(srcStride[0] == 8) {
      uint64_t data;
      gem5Interface::ReadPhysical(pAddr, (void*)&data, 8);
    }
  }
#endif

  assert(tsd->MoreTransfers());
  waitingTransferSets.push(tsd);
  TryTransfers();
}

void
DMAEngine::PrefetchSet(uint64_t addr, unsigned int dimensions,
  const unsigned int* elementSize, const int* elementStride, size_t transferSize)
{
  assert(elementStride);
  assert(elementSize);

  std::set<uint64_t> issuedAddrSet;
  std::set<uint64_t> tlbNotifySet;
  PolyhedralAddresser addrGen(addr, dimensions, elementSize, elementStride);

  for (uint32_t i = 0; i < addrGen.TotalSize(); i++) {
    uint64_t lBlockAddr = AddrRound(addrGen.GetAddr(i), BLOCK_SIZE);
    if (issuedAddrSet.find(lBlockAddr) == issuedAddrSet.end()) {
      uint64_t pBlockAddr = translateAddress->Call(lBlockAddr);
      if (pBlockAddr != 0) {
        MakePrefetch(lBlockAddr, pBlockAddr);
      } else if (tlbNotifySet.find(AddrRound(lBlockAddr, PAGE_SIZE)) == tlbNotifySet.end()) {
        onTLBMiss->Call(AddrRound(lBlockAddr, PAGE_SIZE));
        tlbNotifySet.insert(AddrRound(lBlockAddr, PAGE_SIZE));
      }
    }
  }
}

void
DMAEngine::AddSingleTransfer(int srcDevice, uint64_t srcAddr, int dstDevice,
  uint64_t dstAddr, size_t transferSize, int priority, int buffer,
  CallbackBase* onFinish)
{
  assert(onFinish);
  assert(transferSize);

  TransferData* td = new TransferData(srcDevice, srcAddr, dstDevice, dstAddr,
    transferSize, priority, GetSystemTime(), buffer, onFinish);

  EnqueueTransfer(td);
}

DMAEngine::DMAEngine()
{
  spm = NULL;
  spmInterface = NULL;
  spmID = -1;
  nodeID = -1;
  memObject = NULL;
  memInterface = NULL;
  onTLBMiss = NULL;
  translateAddress = NULL;
  onError = NULL;
  lastEmit = 0;
}

DMAEngine::~DMAEngine()
{
  assert(pendingReads.empty());
  assert(pendingWrites.empty());
  assert(waitingTransferSets.empty());
}

void
DMAEngine::Configure(int node, int spm, Arg1CallbackBase<uint64_t>* onTLBMiss,
  Arg1CallbackFunctionBase<uint64_t, uint64_t>* translateAddress,
  Arg1CallbackBase<uint64_t>* onError)
{
  assert(spmID == -1 && nodeID == -1 && this->spm == NULL);

  spmID = spm;
  nodeID = node;

  int AccID = RubySystem::deviceIDtoAccID(spm);

  if (nodeID < RubySystem::numberOfTDs()) {
    this->spm = g_TdSpmObject[spm];
  } else {
    this->spm = g_spmObject[AccID];

    // accelerator-interposed memory
    if (RubySystem::aim()) {
      int portID = RubySystem::deviceIDtoAccID(nodeID);
      memObject = g_memObject[portID];
      memInterface = g_memInterface;

      ML_LOG(GetDeviceName(), "hook to meteredmemory." << std::setfill('0')
        << std::setw(2) << portID);
    }
  }

  spmInterface = g_spmInterface;
  assert(onTLBMiss);
  assert(translateAddress);
  assert(onError);

  this->onTLBMiss = onTLBMiss;
  this->translateAddress = translateAddress;
  this->onError = onError;
}

void
DMAEngine::Restart()
{
  std::vector<TransferData*> retrySet;
  std::vector<uint64_t> clobberSet;
  for (std::map<uint64_t, std::vector<TransferData*> >::iterator it = waitingOnTLB.begin(); it != waitingOnTLB.end(); it++) {
    uint64_t testAddr = translateAddress->Call(it->first);
    if (testAddr) {
      // this tlb miss has been serviced, and thats why i'm restarting
      for (size_t i = 0; i < it->second.size(); i++) {
        retrySet.push_back(it->second[i]);
      }
      clobberSet.push_back(it->first);
    }
  }

  for (size_t i = 0; i < clobberSet.size(); i++) {
    // I am clobbering things later to avoid iterator invalidation. This is
    // technically outside the C++ spec, but I've found plenty of
    // implementations that are iffy about this.
    waitingOnTLB.erase(clobberSet[i]);
  }

  size_t i;
  for (i = 0; i < retrySet.size(); i++) {
    // I enqueue transfers late because I lack assurances that EnqueueTransfer
    // will not edit the waitingOnTLB set, thus invalidating the iterators.
    // This is technically outside the C++ spec, but I've found plenty of
    // implementations that are iffy about this.
    EnqueueTransfer(retrySet[i]);
  }
}

bool
DMAEngine::EnqueueTransfer(TransferData* td)
{
  bool isRead = (td->dstSpmID == spmID);
  uint64_t lAddr = isRead ? td->srcLAddr : td->dstLAddr;
  uint64_t lPageAddr = AddrRound(lAddr, PAGE_SIZE);
  uint64_t pPageAddr = translateAddress->Call(lPageAddr);

  if (pPageAddr == 0) {
    // TLB miss
    waitingOnTLB[lPageAddr].push_back(td);
    onTLBMiss->Call(lPageAddr);
    return true;  // successfully put in pending, don't block
  }

  uint64_t pAddr = pPageAddr + (lAddr - lPageAddr);
  uint64_t pBlockAddr = AddrRound(pAddr, BLOCK_SIZE);
  std::map<uint64_t, CallbackBase*>& pendingType =
    (isRead) ? pendingReads : pendingWrites;
  std::map<uint64_t, CallbackBase*>& pendingOpposite =
    (!isRead) ? pendingReads : pendingWrites;

  if (pendingType.find(pBlockAddr) != pendingType.end()) {
    pendingType[pBlockAddr] =
      SpliceCB::Create(this, pendingType[pBlockAddr],
        (isRead ? (CallbackBase*)ReadBlockCB::Create(
                    this, pAddr, lAddr, td->dstLAddr, td->elementSize) :
                  (CallbackBase*)WriteBlockCB::Create(
                    this, td->srcLAddr, pAddr, lAddr, td->elementSize)));

    pendingType[pBlockAddr] =
      SpliceCB::Create(this, pendingType[pBlockAddr], td->onFinish);

    delete td;
    return true;
  }

  if (pendingOpposite.find(pBlockAddr) != pendingOpposite.end()) {
    pendingOpposite[pBlockAddr] =
      SpliceCB::Create(this, pendingOpposite[pBlockAddr],
        ReEnqueueTransferCB::Create(this, td));
    return true;
  }

  RubyRequestType requestType = (isRead) ? RubyRequestType_LD : RubyRequestType_ST;
  bool isReady;

  if (td->buffer == -1) {
    // this is the private buffer case. Just work directly between memory and DMA
    isReady = IsReady(lAddr, pBlockAddr, requestType);
  } else {
    // this is the shared buffer case. Target the shared buffer
    isReady = IsBufferReady(td->buffer, pBlockAddr, (isRead) ?
      td->dstLAddr : td->srcLAddr, requestType);
  }

  if (!isReady) {
    waitingTransfers.push(td);
    if (lastEmit != GetSystemTime()) {
      // nothing was emitted this cycle
      ScheduleCB(1, TryTransfersCB::Create(this));
    }
    return false;  // block the chain
  }
  lastEmit = GetSystemTime();

  pendingType[pBlockAddr] = SpliceCB::Create(this,
    (isRead ? (CallbackBase*)ReadBlockCB::Create(
                this, pAddr, lAddr, td->dstLAddr, td->elementSize) :
              (CallbackBase*)WriteBlockCB::Create(
                this, td->srcLAddr, pAddr, lAddr, td->elementSize)),
    td->onFinish);

  if (td->buffer == -1) {
    // this is the no buffer case
    assert(emitTime.find(pBlockAddr) == emitTime.end());
    emitTime[pBlockAddr] = GetSystemTime();
    MakeRequest(lAddr, pBlockAddr, requestType,
      OnMemoryResponseCB::Create(this, pBlockAddr, lastEmit));
  } else {
    // a shared buffer is being used. access that instead
    assert(emitTime.find(pBlockAddr) == emitTime.end());
    emitTime[pBlockAddr] = GetSystemTime();
    MakeBufferCopy(pBlockAddr,
      (isRead) ? td->dstLAddr : td->srcLAddr, td->buffer, requestType,
      OnMemoryResponseCB::Create(this, pBlockAddr, lastEmit));
  }
  delete td;
  return true;  // keep accesses coming
}

void
DMAEngine::TryTransfers()
{
  while (true) {
    if ((waitingTransferSets.empty() && !waitingTransfers.empty()) ||
        (!waitingTransfers.empty() && !waitingTransferSets.empty() &&
        waitingTransfers.top()->priority >= waitingTransferSets.top()->priority))
    {
      TransferData* td = waitingTransfers.top();
      waitingTransfers.pop();
      if (!EnqueueTransfer(td)) {
        return;
      }
    } else if(!waitingTransferSets.empty()) {
      TransferSetDesc* tsd = waitingTransferSets.top();
      assert(tsd->MoreTransfers());
      TransferData* d = tsd->NextTransfer();
      if (!tsd->MoreTransfers()) {
        waitingTransferSets.pop();
      }
      bool success = EnqueueTransfer(d);
      if (!success) {
        return;
      }
    } else {
      return;
    }
  }
}

void
DMAEngine::OnMemoryResponse(uint64_t addr, uint64_t calledTime)
{
  assert(emitTime.find(addr) != emitTime.end());
  emitTime.erase(addr);
  if (pendingReads.find(addr) != pendingReads.end()) {
    ScheduleCB(0, pendingReads[addr]);
    pendingReads.erase(addr);
  }

  if (pendingWrites.find(addr) != pendingWrites.end()) {
    ScheduleCB(0, pendingWrites[addr]);
    pendingWrites.erase(addr);
  }
  ScheduleCB(0, TryTransfersCB::Create(this));
}

void
DMAEngine::UnhookMemoryPort()
{
  assert(memObject);
  assert(memInterface);
  memObject = NULL;
  memInterface = NULL;
}
