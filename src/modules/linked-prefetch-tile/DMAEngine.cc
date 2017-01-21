#include "DMAEngine.hh"
#include "../MsgLogger/MsgLogger.hh"
#include "../Common/mf_api.hh"
#include "arch/isa_traits.hh"
#include "mem/ruby/system/Sequencer.hh"
#include "mem/ruby/system/RubyPort.hh"
#include "mem/ruby/system/System.hh"
#include "mem/protocol/RequestStatus.hh"
#include "mem/ruby/system/SimicsNetworkPort.hh"

//#define SIM_PAGE_FAULT
#ifdef SIM_PAGE_FAULT
#include "sim/system.hh"
#include "arch/vtophys.hh"
#endif

#define PAGE_SIZE (TheISA::PageBytes)

//0. SW version
//#define SIM_SW

//1. ARC version
//#define SIM_ARC

//2. ARC PLUS version
#define SIM_ARC_PLUS

//3. CHARM NON-ISLAND version
//#define SIM_CHARM

//4. CHARM ISLAND version
//#define SIM_CHARM_ISLAND

//for compiling purpose
#if defined(SIM_SW)
#define SIM_ARC
#endif

#if defined(SIM_ARC_PLUS) || defined(SIM_CHARM) || defined(SIM_CHARM_ISLAND)
#define SIM_TD
#endif

void
TransferSetDesc::DecrementPending(size_t finishSize)
{
  pendingSize -= finishSize;

  if (!MoreTransfers() && pendingSize == 0) {
    ScheduleCB(0, onFinish);
    // suicide
    delete this;
  }
}

TransferData*
TransferSetDesc::NextTransfer()
{
  int i = index;
  size_t aggSize = transferSize;

  while (start.addrGen.TotalSize() > i + 1) {
    uint64_t currAddr;
    uint64_t nextAddr;

    if (start.spm == NO_SPM_ID) {
      // is read
      currAddr = start.addrGen.GetAddr(i);
      nextAddr = start.addrGen.GetAddr(i + 1);
    } else {
      // is write
      currAddr = end.addrGen.GetAddr(i);
      nextAddr = end.addrGen.GetAddr(i + 1);
    }

    if (nextAddr == currAddr + transferSize && nextAddr % BLOCK_SIZE != 0) {
      // contiguous address within one cacheline
      aggSize += transferSize;
      i++;
    } else {
      break;
    }
  }

  uint64_t srcAddr = start.addrGen.GetAddr(index);
  uint64_t dstAddr = end.addrGen.GetAddr(index);
  index = i + 1;
  pendingSize += aggSize;
  return new TransferData(start.spm, srcAddr, end.spm, dstAddr, aggSize,
                          priority, timeStamp, buffer, DecrementPendingCB::Create(this, aggSize));
}

DMAEngine::DMAEngine()
{
  spm = NULL;
  spmInterface = NULL;
  spmID = -1;
  nodeID = -1;
  memObject = NULL;
  memInterface = NULL;
  beginTranslateTiming = NULL;
  onError = NULL;
  lastEmit = 0;
  inflight = 0;
  issueWidth = RubySystem::getDMAIssueWidth();
  scheduled = false;
}

DMAEngine::~DMAEngine()
{
  assert(pendingReads.empty());
  assert(pendingWrites.empty());
  assert(waitingTransferSets.empty());
}

void
DMAEngine::Configure(int node, int spm,
                     Arg1CallbackBase<TransferData*>* beginTranslateTiming,
                     Arg1CallbackBase<uint64_t>* onError)
{
  assert(spmID == -1 && nodeID == -1 && this->spm == NULL);
  spmID = spm;
  nodeID = node;
#ifdef SIM_ARC
  this->spm = g_spmObject[RubySystem::deviceIDtoAccID(spm)];
#endif
#ifdef SIM_TD

  if (nodeID < RubySystem::numberOfTDs()) {
    this->spm = g_TdSpmObject[spm];
  } else {
    this->spm = g_spmObject[RubySystem::deviceIDtoAccID(spm)];
  }

#endif
  spmInterface = g_spmInterface;
  this->beginTranslateTiming = beginTranslateTiming;
  this->onError = onError;
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
#ifdef SIM_ARC
    int L1CacheID = RubySystem::accIDtoL1CacheID(
                      RubySystem::deviceIDtoAccID(nodeID));
#endif
#ifdef SIM_TD
    int L1CacheID;

    if (nodeID < RubySystem::numberOfTDs()) {
      L1CacheID = RubySystem::tdIDtoL1CacheID(nodeID);
    } else {
      L1CacheID = RubySystem::accIDtoL1CacheID(
                    RubySystem::deviceIDtoAccID(nodeID));
    }

#endif
    //std::cout << "[DMAEngine::MakeRequest]nodeID " << nodeID << ", L1CacheID " << L1CacheID << std::endl;
    AbstractController* L1Controller =
      g_abs_controls[MachineType_L1Cache][L1CacheID];
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
#ifdef SIM_ARC
    int L1CacheID = RubySystem::accIDtoL1CacheID(RubySystem::deviceIDtoAccID(nodeID));
#endif
#ifdef SIM_TD
    int L1CacheID;

    if (nodeID < RubySystem::numberOfTDs())
      L1CacheID = RubySystem::tdIDtoL1CacheID(nodeID);
    else
      L1CacheID = RubySystem::accIDtoL1CacheID(RubySystem::deviceIDtoAccID(nodeID));

#endif
    AbstractController* L1Controller =
      g_abs_controls[MachineType_L1Cache][L1CacheID];
    Sequencer* seq = L1Controller->getSequencer();
    Request::Flags flags = 0;
    RequestPtr req = new Request(pAddr, BLOCK_SIZE, flags, L1CacheID);
    MemCmd cmd;

    if (direction == RubyRequestType_ST)
      cmd = MemCmd::WriteReq;
    else
      cmd = MemCmd::ReadReq;

    PacketPtr pkt = new Packet(req, cmd);
    // RubyPort::SenderState *senderState = new RubyPort::SenderState(NULL);
    // pkt->pushSenderState(senderState);
    RequestStatus status = seq->makeRequest(pkt, RubyExecuteCB, cb);
    assert(status == RequestStatus_Issued);

    // ML_LOG("DMAEngine", "memory request issued 0x" << std::hex << lAddr);
  } else {
    assert(memInterface);
    assert(memObject);
    assert(direction == RubyRequestType_LD || direction == RubyRequestType_ST);

    if (direction == RubyRequestType_LD) {
      memInterface->IssueRead(memObject, pAddr, BLOCK_SIZE,
                              MemDevInterfaceInterceptCB::Create(this, cb));
    } else {
      memInterface->IssueWrite(memObject, pAddr, BLOCK_SIZE, NULL, cb);
    }
  }
}

void
DMAEngine::MakePrefetch(uint64_t lAddr, uint64_t pAddr)
{
  //do nothing
}

void
DMAEngine::MakeBufferCopy(uint64_t cacheAddr, uint64_t bufferAddr,
                          int bufferId, RubyRequestType direction, CallbackBase* cb)
{
  //do nothing
}

void
DMAEngine::MemDevInterfaceIntercept(const void*, CallbackBase* cb)
{
  assert(cb);
  cb->Call();
  cb->Dispose();
}

void
DMAEngine::reFinishTranslation(TransferData* td)
{
  finishTranslation(td);
}

void
DMAEngine::WriteBlock(
  uint64_t spmAddr, uint64_t pMemAddr, uint64_t lMemAddr, size_t size)
{
  uint8_t* data = new uint8_t [size];
  spmInterface->read(spm, spmAddr, data, size);
  WriteMemory(pMemAddr, data, size);
  delete [] data;
}

void
DMAEngine::ReadBlock(
  uint64_t pMemAddr, uint64_t lMemAddr, uint64_t spmAddr, size_t size)
{
  uint8_t* data = new uint8_t [size];
  ReadMemory(pMemAddr, data, size);
  spmInterface->write(spm, spmAddr, data, size);
  delete [] data;
}

void
DMAEngine::Splice(CallbackBase* cb1, CallbackBase* cb2)
{
  ScheduleCB(0, cb1);
  ScheduleCB(0, cb2);
}

void
DMAEngine::AddTransferSet(
  int srcDevice, uint64_t srcAddr, unsigned int srcDimensions,
  const unsigned int* srcElementSize, const int* srcElementStride,
  int dstDevice, uint64_t dstAddr, unsigned int dstDimensions,
  const unsigned int* dstElementSize, const int* dstElementStride,
  size_t transferSize, int priority, int buffer, CallbackBase* onFinish)
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

  TransferSetDesc* tsd = new TransferSetDesc(
    srcDevice, srcAddr, srcSize, srcStride,
    dstDevice, dstAddr, dstSize, dstStride,
    transferSize, priority, GetSystemTime(), buffer, onFinish);

  assert(tsd->MoreTransfers());

  waitingTransferSets.push(tsd);

  if (!scheduled) {
    ScheduleCB(1, TryTransfersCB::Create(this));
    scheduled = true;
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

  waitingTransfers.push(td);

  if (!scheduled) {
    ScheduleCB(1, TryTransfersCB::Create(this));
    scheduled = true;
  }
}

void
DMAEngine::PrefetchSet(uint64_t addr, unsigned int dimensions,
                       const unsigned int* elementSize, const int* elementStride,
                       size_t transferSize)
{
  // assert(elementStride);
  // assert(elementSize);
  // std::set<uint64_t> issuedAddrSet;
  // std::set<uint64_t> tlbNotifySet;
  // PolyhedralAddresser addrGen(addr, dimensions, elementSize, elementStride);
  // for (uint32_t i = 0; i < addrGen.TotalSize(); i++) {
  //     uint64_t lBlockAddr = AddrRound(addrGen.GetAddr(i), BLOCK_SIZE);
  //     if (issuedAddrSet.find(lBlockAddr) == issuedAddrSet.end()) {
  //         uint64_t pBlockAddr = translateAddress->Call(lBlockAddr);
  //         if (pBlockAddr != 0) {
  //             MakePrefetch(lBlockAddr, pBlockAddr);
  //         } else if (tlbNotifySet.find(
  //             AddrRound(lBlockAddr, PAGE_SIZE)) == tlbNotifySet.end()) {
  //             onTLBMiss->Call(AddrRound(lBlockAddr, PAGE_SIZE));
  //             tlbNotifySet.insert(AddrRound(lBlockAddr, PAGE_SIZE));
  //         }
  //     }
  // }
}

void
DMAEngine::finishTranslation(TransferData* td)
{
  uint64_t lAddr = td->getVaddr();
  uint64_t pAddr = td->getPaddr();
  uint64_t pBlockAddr = AddrRound(pAddr, BLOCK_SIZE);

  std::map<uint64_t, CallbackBase*>& pendingType =
    (td->isRead()) ? pendingReads : pendingWrites;

  std::map<uint64_t, CallbackBase*>& pendingOpposite =
    (!td->isRead()) ? pendingReads : pendingWrites;

  if (pendingType.find(pBlockAddr) != pendingType.end()) {
    pendingType[pBlockAddr] = SpliceCB::Create(this,
                              pendingType[pBlockAddr], (td->isRead() ?
                                  (CallbackBase*)ReadBlockCB::Create(this, pAddr, lAddr, td->dstLAddr, td->elementSize) :
                                  (CallbackBase*)WriteBlockCB::Create(this, td->srcLAddr, pAddr, lAddr, td->elementSize)));

    pendingType[pBlockAddr] = SpliceCB::Create(this,
                              pendingType[pBlockAddr], td->onFinish);
    delete td;
    return;
  }

  if (pendingOpposite.find(pBlockAddr) != pendingOpposite.end()) {
    pendingOpposite[pBlockAddr] = SpliceCB::Create(this,
                                  pendingOpposite[pBlockAddr], reFinishTranslationCB::Create(this, td));
    return;
  }

  RubyRequestType requestType =
    (td->isRead()) ? RubyRequestType_LD : RubyRequestType_ST;

  bool isReady;

  if (td->buffer == -1) {
    // This is the no buffer case. Just work between memory and DMA
    isReady = IsReady(lAddr, pBlockAddr, requestType);
  } else {
    // This is the shared buffer case. Target the shared buffer
    isReady = IsBufferReady(td->buffer, pBlockAddr,
                            (td->isRead()) ? td->dstLAddr : td->srcLAddr, requestType);
  }

  if (!isReady) {
    waitingTransfers.push(td);
    return;
  }

  lastEmit = GetSystemTime();
  pendingType[pBlockAddr] = SpliceCB::Create(this,
                            (td->isRead() ?
                             (CallbackBase*)ReadBlockCB::Create(this, pAddr, lAddr, td->dstLAddr, td->elementSize) :
                             (CallbackBase*)WriteBlockCB::Create(this, td->srcLAddr, pAddr, lAddr, td->elementSize)),
                            td->onFinish);

  if (td->buffer == -1) {
    // this is the no buffer case
    assert(emitTime.find(pBlockAddr) == emitTime.end());
    emitTime[pBlockAddr] = GetSystemTime();
    MakeRequest(lAddr, pBlockAddr, requestType,
                OnMemoryResponseCB::Create(this, pBlockAddr, lastEmit));
  } else {
    // a shared buffer is being used.  access that instead
    assert(emitTime.find(pBlockAddr) == emitTime.end());
    emitTime[pBlockAddr] = GetSystemTime();
    MakeBufferCopy(pBlockAddr, (td->isRead()) ? td->dstLAddr : td->srcLAddr,
                   td->buffer, requestType,
                   OnMemoryResponseCB::Create(this, pBlockAddr, lastEmit));
  }

  delete td;
}

void
DMAEngine::TryTransfers()
{
  if (inflight >= issueWidth) {
    scheduled = false;
    return;
  }

  TransferData* td;

  if ((waitingTransferSets.empty() && !waitingTransfers.empty()) ||
      (!waitingTransfers.empty() && !waitingTransferSets.empty() &&
       waitingTransfers.top()->priority >=
       waitingTransferSets.top()->priority)) {

    td = waitingTransfers.top();
    waitingTransfers.pop();

  } else if (!waitingTransferSets.empty()) {
    TransferSetDesc* tsd = waitingTransferSets.top();
    td = tsd->NextTransfer();

    if (!tsd->MoreTransfers()) {
      waitingTransferSets.pop();
    }
  } else {
    // no more transfer
    scheduled = false;
    return;
  }

  beginTranslateTiming->Call(td);

  inflight++;

  if (inflight < issueWidth && transferLeft()) {
    ScheduleCB(1, TryTransfersCB::Create(this));
    scheduled = true;
  } else {
    scheduled = false;
  }

  // ML_LOG("DMAEngine", "issued 0x" << std::hex << td->getVaddr() << std::dec
  //     << " size: " << td->elementSize << " inflight: " << inflight);
}

void
DMAEngine::OnMemoryResponse(uint64_t addr, uint64_t calledTime)
{
  assert(emitTime.find(addr) != emitTime.end());
  emitTime.erase(addr);

  assert(inflight);
  inflight--;
  // ML_LOG("DMAEngine", "received 0x" << std::hex << addr << std::dec
  //     << " inflight: " << inflight);

  if (pendingReads.find(addr) != pendingReads.end()) {
    ScheduleCB(0, pendingReads[addr]);
    pendingReads.erase(addr);
  }

  if (pendingWrites.find(addr) != pendingWrites.end()) {
    ScheduleCB(0, pendingWrites[addr]);
    pendingWrites.erase(addr);
  }

  if (!scheduled && transferLeft()) {
    ScheduleCB(1, TryTransfersCB::Create(this));
    scheduled = true;
  }
}

void
DMAEngine::HookToMemoryPort(const char* deviceName)
{
  assert(memObject == NULL);
  assert(memInterface == NULL);
  assert(deviceName);
  memObject = g_memObject;
  assert(memObject);
  memInterface = g_memInterface;
  assert(memInterface);
}

void
DMAEngine::UnhookMemoryPort()
{
  assert(memObject);
  assert(memInterface);
  memObject = NULL;
  memInterface = NULL;
}

bool
DMAEngine::transferLeft()
{
  return (!waitingTransfers.empty() || !waitingTransferSets.empty());
}
