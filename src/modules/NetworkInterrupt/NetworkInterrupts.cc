#include <cassert>
#include <stdint.h>
#include <iostream>
#include "NetworkInterrupts.hh"
#include "../Common/mf_api.hh"
#include "../LCAcc/LCAccCommandListing.hh"
#include "../Common/BitConverter.hh"
#include "../TLBHack/TLBHack.hh"
#include "../../sim/system.hh"
#include "../../mem/ruby/system/System.hh"
#include "../MsgLogger/MsgLogger.hh"
#include "modules/Synchronize/Synchronize.hh"
#include "sim/pseudo_inst.hh"
#include "lwi.hh"
#include "modules/LCAcc/SimicsInterface.hh"
#include "arch/vtophys.hh"

#include "arch/x86/tlb.hh"
#include "arch/x86/regs/misc.hh"
#include "arch/x86/pagetable_walker.hh"

#define MAX_ISR_BUFFER_SIZE 128

std::vector<std::queue<CBContainer> > cycleCBRing;

bool
localCBsForCycle(Cycles current_cycle)
{
  uint64_t m_current_cycle = uint64_t(current_cycle);
  size_t oldIndex = (m_current_cycle - 1) % cycleCBRing.size();
  size_t index = m_current_cycle % cycleCBRing.size();

  if (!cycleCBRing[oldIndex].empty()) {
    std::cout << "[assertion fail]oldIndex: " << oldIndex
              << ", currentIndex: " << index << std::endl;
  }

  assert(cycleCBRing[oldIndex].empty());
  return !(cycleCBRing[index].empty());
}

std::queue<CBContainer>&
getCurrentCycleQueue(Cycles current_cycle)
{
  uint64_t m_current_cycle = uint64_t(current_cycle);
  assert(localCBsForCycle(current_cycle));
  size_t index = m_current_cycle % cycleCBRing.size();
  return cycleCBRing[index];
}

void
retireCBsForCycle(Cycles current_cycle)
{
  assert(!localCBsForCycle(current_cycle));
}

void
scheduleCB(void (*cb)(void*), void* args, uint64_t delta)
{
  uint64_t m_global_cycles = uint64_t(g_system_ptr->curCycle());

  assert(cb);

  //  assert(delta < cycleCBRing.size());
  if (delta + 10 >= cycleCBRing.size()) {
    std::cout << "Expanding event ring from " << cycleCBRing.size() << " to " << delta * 2 << std::endl;
    std::vector<std::queue<CBContainer> > oldSchedule = cycleCBRing;
    cycleCBRing.clear();

    for (size_t i = 0; i < delta * 2; i++) {
      cycleCBRing.push_back(std::queue<CBContainer>());
    }

    assert(cycleCBRing.size() > oldSchedule.size());

    for (size_t i = 0; i < oldSchedule.size(); i++) {
      cycleCBRing[(m_global_cycles + i) % cycleCBRing.size()] =
        oldSchedule[(m_global_cycles + i) % oldSchedule.size()];
    }
  }

  size_t index = (m_global_cycles + delta) % cycleCBRing.size();
  cycleCBRing[index].push(CBContainer(cb, args));
}


TLBHackInterface* tlbHackInterface = g_TLBHack_interface;
std::map<int, NetworkInterrupts*> NetworkInterrupts::cpuMap;
std::map<int, std::vector<int> > NetworkInterrupts::pendingReservation;
std::map<int, std::vector<NetworkInterrupts::BiNPerformancePoint> > NetworkInterrupts::BiNCurveInfo; //key thread, second key lcacc
std::map<int, std::vector<NetworkInterrupts::AcceleratorDeclaration> > NetworkInterrupts::accDeclInfo;

void
NetworkInterrupts::TryRaise(int thread)
{
  if (!pendingMsgs[thread].empty()) {
    assert(pendingMsgs[thread].top().thread == thread);
    //lwInt_ifc_t* lwi = (lwInt_ifc_t*)SIM_get_interface(SIM_get_object("opal0"), "lwInt_interface");
    lwInt_ifc_t* lwi = g_lwInt_interface;

    if (lwi->isReady(thread)) {
      Msg m = pendingMsgs[thread].top();
      ML_LOG(GetDeviceName(), "raising interrupt for userthread " << thread);
      assert(m.packet.size() <= MAX_ISR_BUFFER_SIZE);
      lwi->raiseLightWeightInt(m.thread, &(m.packet[0]), m.packet.size(), m.lcacc);
      pendingMsgs[thread].pop();

      uint64_t sum = 0;

      for (int i = 0; i < queueLen.size(); i++) {
        sum += queueLen[i];
      }

      if (queueLen.size()) {
        ML_LOG(GetDeviceName(), "avg queue length: "
               << (float)sum / queueLen.size());
      }
    }

    //EnqueueEvent(nih->attachedCPU, TryRaiseCB::Create(this, thread), 1);
    EnqueueEvent(TryRaiseCB::Create(this, thread), 1);
  }
}

int
NetworkInterrupts::CalcPriority(int source, int threadID,
                                const std::vector<uint8_t>& packet)
{
  if (source >= 1000 && source <= 1031) {
    return 100;
  }

  return 0;
}

NetworkInterrupts*
NetworkInterrupts::LookupNIByCpu(int cpu)
{
  if (cpuMap.find(cpu) != cpuMap.end()) {
    return cpuMap[cpu];
  }

  return NULL;
}

// NetworkInterrupts::NetworkInterrupts(const Params *p)
// {

// }

NetworkInterrupts::NetworkInterrupts(NetworkInterruptHandle* x)
{
  assert(x);
  nih = x;

  if (x->attachedCPU) {
    // contextId vs. threadId: threadId is the hardware thread executing on
    // a single cpu. contextId is the execution context for a given cpu.
    // Here we actually need a cpuId, which should be unique in the system.
    ML_LOG(GetDeviceName(), "attached to threadcontext "
           << x->attachedCPU->contextId());

    //Zhenman: Currently we allow multiple ports/devices binded with one CPU core
    //assert(cpuMap.find(x->attachedCPU) == cpuMap.end());

    // cpuMap[x->procID] = this;
    cpuMap[x->attachedCPU->contextId()] = this;

    x->snpi = g_networkPort_interface;
    x->snpi->BindDeviceToPort(x->portID, x->deviceID);
    x->snpi->RegisterRecvHandlerOnDevice(x->deviceID, HandleNetworkMessage, RecvMessageCB::Create(this));
    ML_LOG(GetDeviceName(), "bind device " << x->deviceID
           << " to port " << x->portID);
  }

  hostPTWLatency = RubySystem::getHostPTWLatency();
  hostPTWalks = 0;
  hostPTWalkTick = 0;
  hostPTWalkTime = 0;
  currState = Ready;

  tlbSize = 32;
  tlb = new X86ISA::TlbEntry[tlbSize];
  std::memset(tlb, 0, sizeof(X86ISA::TlbEntry) * tlbSize);

  for (int x = 0; x < tlbSize; x++) {
    tlb[x].trieHandle = NULL;
    freeList.push_back(&tlb[x]);
  }

  lruSeq = 0;

  interval = 1;
  EnqueueEvent(sampleQueueLenCB::Create(this), interval);
}

NetworkInterrupts::~NetworkInterrupts()
{
  assert(nih);
  assert(cpuMap.find(nih->procID) != cpuMap.end());
  assert(cpuMap[nih->procID] == this);
  cpuMap.erase(nih->procID);
}

int
NetworkInterrupts::GetSignal(int thread)
{
  if (pendingSignals.find(thread) != pendingSignals.end()) {
    assert(!pendingSignals[thread].empty());
    int ret = pendingSignals[thread].front();
    pendingSignals[thread].pop();

    if (pendingSignals[thread].empty()) {
      pendingSignals.erase(thread);
    }

    return ret;
  }

  return 0;
}

void
NetworkInterrupts::RaiseInterrupt(int source, int threadID,
                                  const void* buffer, int bufferSize)
{
  assert(buffer);
  assert(bufferSize <= MAX_ISR_BUFFER_SIZE);
  bool mustTryRaise = pendingMsgs[threadID].empty();
  Msg m;
  const uint8_t* srcBuffer = (const uint8_t*) buffer;

  for (int i = 0; i < bufferSize; i++) {
    m.packet.push_back(srcBuffer[i]);
  }

  m.lcacc = source;
  m.thread = threadID;
  m.priority = CalcPriority(source, threadID, m.packet);
  pendingMsgs[threadID].push(m);

  if (mustTryRaise) {
    TryRaise(threadID);
  }
}

void
NetworkInterrupts::PutOffInterrupt(int source, int threadID,
                                   const void* buffer, int bufferSize, int delay)
{
  CallbackBase* cb = RaiseInterruptCB::Create(this, source, threadID, buffer, bufferSize);
  //EnqueueEvent(nih->attachedCPU, cb, delay);
  EnqueueEvent(cb, delay);
}

void
NetworkInterrupts::RecvMessage(int source, const char* buffer, int size)
{
  tlbHackInterface = g_TLBHack_interface;
  assert((size_t)size >= sizeof(int32_t));
  const int32_t* args = (const int*) buffer;

  if (args[0] == LCA_RAISE_SIGNAL) {
    assert(size == 3 * sizeof(int32_t));

    int thread = args[1];
    int signal = args[2];
    assert(signal != 0);
    pendingSignals[thread].push(signal);

  } else if (args[0] == LCACC_CMD_TLB_MISS) {
    assert(size >= 9);

    int thread = args[1];
    BitConverter bc_vAddr;
    bc_vAddr.u32[0] = args[2];
    bc_vAddr.u32[1] = args[3];
    uint64_t vAddr = bc_vAddr.u64[0];
    assert(vAddr);

    if (RubySystem::iommu()) {
      // lookup the iotlb
      X86ISA::TlbEntry *entry = lookup(vAddr);

      if (entry) {
        // ML_LOG(GetDeviceName(), "IOTLB hit");
        // iotlb has the entry, send response
        uint64_t pAddr = entry->paddr;
        uint32_t buffer[9];
        buffer[0] = LCACC_CMD_TLB_SERVICE;
        buffer[1] = thread;
        BitConverter bcv;
        bcv.u64[0] = vAddr;
        buffer[2] = bcv.u32[0];
        buffer[3] = bcv.u32[1];
        BitConverter bcp;
        bcp.u64[0] = pAddr;
        buffer[4] = bcp.u32[0];
        buffer[5] = bcp.u32[1];
        buffer[6] = 0;
        buffer[7] = 0;
        buffer[8] = 0;
        // TODO: replace "0" with TD port id
        nih->snpi->SendMessageOnDevice(
          nih->deviceID, 0, buffer, sizeof(buffer));
        // we are done here
      } else {
        // iotlb miss, walk the page table
        setupWalk(thread, vAddr);
      }
    } else {
      // let cpu walk the page table
      setupWalk(thread, vAddr);
    }
  } else {
    RaiseInterrupt(source, args[1], args, size);
  }
}

void
NetworkInterrupts::setupWalk(int thread, uint64_t vAddr)
{
  RequestPtr req = new Request();
  Request::Flags flags;

  if (RubySystem::iommu()) {
    // bypass l1 and l2 cache to simulate the behavior of an IOMMU
    flags.set(Request::BYPASS_CACHE);
  }

  Addr pc = 0;
  const int asid = 0;
  int size = 64;
  req->setVirt(asid, vAddr, size, flags, Request::funcMasterId, pc);
  req->taskId(thread);

  // ML_LOG(GetDeviceName(), "Receive page table walking req from userthread"
  //     << thread << " on 0x" << std::hex << req->getVaddr());
  pendingTranslations.push_back(req);

  ML_LOG(GetDeviceName(), "pendingTranslations enqueue size: "
         << pendingTranslations.size());

  if (currState == Ready) {
    startWalk();
  }
}

void
NetworkInterrupts::startWalk()
{
  assert(pendingTranslations.size());
  currState = Waiting;
  hostPTWalkTick = g_system_ptr->curCycle();
  RequestPtr req = pendingTranslations.front();
  pendingTranslations.pop_front();
  EnqueueEvent(HostPTWalkCB::Create(this, req), hostPTWLatency);
}

void
NetworkInterrupts::HostPTWalk(RequestPtr req)
{
  BaseTLB::Mode mode = BaseTLB::Read;

  WholeTranslationState *state =
    new WholeTranslationState(req, NULL, NULL, mode);
  DataTranslation<NetworkInterrupts*> *translation =
    new DataTranslation<NetworkInterrupts*>(this, state);

  hostPTWalks++;
  // ML_LOG(GetDeviceName(), "Start host page table walking 0x"
  //     << std::hex << req->getVaddr());

  System *m5_system = *(System::systemList.begin());
  ThreadContext* tc = m5_system->getThreadContext(nih->procID);

  X86ISA::Walker *walker = tc->getDTBPtr()->getWalker();
  translation->markDelayed();
  Fault fault = walker->start(tc, translation, req, mode);
}

void
NetworkInterrupts::finishTranslation(WholeTranslationState *state)
{
  if (state->getFault() != NoFault) {
    panic("Translation encountered fault (%s) for address 0x%x",
          state->getFault()->name(), state->mainReq->getVaddr());
  }

  RequestPtr req = state->mainReq;

  assert(req->hasPaddr());

  uint64_t hostPTWalkLatency = g_system_ptr->curCycle() - hostPTWalkTick;
  hostPTWalkTime += hostPTWalkLatency;
  currState = Ready;

  uint64_t logicalPage  = req->getVaddr();
  uint64_t physicalPage = req->getPaddr();

  if (RubySystem::iommu()) {
    // refill iotlb
    X86ISA::TlbEntry newEntry;
    // 4 KB page
    newEntry.logBytes = 12;
    newEntry.paddr = physicalPage;
    newEntry.vaddr = logicalPage;
    insert(logicalPage, newEntry);
  }

  // ML_LOG(GetDeviceName(), "hostPTWalkLatency = " << hostPTWalkLatency);
  // ML_LOG(GetDeviceName(), "Finish host page table walking 0x" << std::hex
  //     << req->getVaddr() << " -> 0x" << req->getPaddr());

  uint32_t buffer[9];
  buffer[0] = LCACC_CMD_TLB_SERVICE;
  buffer[1] = req->taskId();
  BitConverter bcv;
  bcv.u64[0] = logicalPage;
  buffer[2] = bcv.u32[0];
  buffer[3] = bcv.u32[1];
  BitConverter bcp;
  bcp.u64[0] = physicalPage;
  buffer[4] = bcp.u32[0];
  buffer[5] = bcp.u32[1];
  buffer[6] = 0;
  buffer[7] = 0;
  buffer[8] = 0;
  // TODO: replace "0" with TD port id
  nih->snpi->SendMessageOnDevice(nih->deviceID, 0, buffer, sizeof(buffer));

  delete state->mainReq;
  delete state;

  if (pendingTranslations.size()) {
    startWalk();
  }
}

void
NetworkInterrupts::sampleQueueLen()
{
  if (pendingTranslations.size())
    queueLen.push_back(pendingTranslations.size());

  EnqueueEvent(sampleQueueLenCB::Create(this), interval);
}

void
NetworkInterrupts::evictLRU()
{
  // Find the entry with the lowest (and hence least recently updated)
  // sequence number.
  unsigned lru = 0;

  for (unsigned i = 1; i < tlbSize; i++) {
    if (tlb[i].lruSeq < tlb[lru].lruSeq)
      lru = i;
  }

  assert(tlb[lru].trieHandle);
  trie.remove(tlb[lru].trieHandle);
  tlb[lru].trieHandle = NULL;
  freeList.push_back(&tlb[lru]);
}

X86ISA::TlbEntry *
NetworkInterrupts::insert(uint64_t vpn, X86ISA::TlbEntry &entry)
{
  // If somebody beat us to it, just use that existing entry.
  X86ISA::TlbEntry *newEntry = trie.lookup(vpn);

  if (newEntry) {
    assert(newEntry->vaddr == vpn);
    return newEntry;
  }

  if (freeList.empty())
    evictLRU();

  newEntry = freeList.front();
  freeList.pop_front();

  *newEntry = entry;
  newEntry->lruSeq = nextSeq();
  newEntry->vaddr = vpn;
  newEntry->trieHandle =
    trie.insert(vpn, TlbEntryTrie::MaxBits - entry.logBytes, newEntry);
  return newEntry;
}

X86ISA::TlbEntry *
NetworkInterrupts::lookup(uint64_t va, bool update_lru)
{
  X86ISA::TlbEntry *entry = trie.lookup(va);

  if (entry && update_lru)
    entry->lruSeq = nextSeq();

  return entry;
}

void
NetworkInterrupts::flushAll()
{
  for (unsigned i = 0; i < tlbSize; i++) {
    if (tlb[i].trieHandle) {
      trie.remove(tlb[i].trieHandle);
      tlb[i].trieHandle = NULL;
      freeList.push_back(&tlb[i]);
    }
  }
}

void
HandleNetworkMessage(void* voidCB, int source, int destination,
                     const char* buffer, int length)
{
  Arg3CallbackBase<int, const char*, int>* cb =
    (Arg3CallbackBase<int, const char*, int>*)voidCB;
  cb->Call(source, buffer, length);
}

void
HandleEvent(void* arg)
{
  CallbackBase* cb = (CallbackBase*)arg;
  cb->Call();
  cb->Dispose();
}

void
EnqueueEvent(void* arg, int dt)
{
  scheduleCB(HandleEvent, arg, dt);
}

NetworkInterruptHandle*
createNetworkInterruptHandle(int portID, int deviceID, int procID)
{
  NetworkInterruptHandle* handle =
    (NetworkInterruptHandle*)malloc(sizeof(NetworkInterruptHandle));
  //Zhenman: suppose we just have one M5 system running
  //System::printSystems();
  std::vector<System *>::iterator system_iterator = System::systemList.begin();
  System *m5_system = *system_iterator;

  handle->snpi = g_networkPort_interface;
  handle->portID = portID;
  handle->deviceID = deviceID;
  handle->attachedCPU = m5_system->getThreadContext(procID);
  handle->procID = procID;

  return handle;
}

uint64_t
LCAccMagicIntercept(void*, ThreadContext* cpu, int32_t op,
                    uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,
                    uint64_t arg5, uint64_t arg6, uint64_t arg7)
{

  uint64_t retVal = 0;
  tlbHackInterface = g_TLBHack_interface;

  //Note Barrier and TLB Touch are needed by initilization stage, where ruby is disabled
  //We only need the TLB inerface setup for these two operations
  if (op == 0xBA00 || op == 0xBA01) {
    // Barrier Tick or Barrier Blocked
    retVal = MagicIntercept(NULL, cpu, op, (int)arg2);
    return retVal;
  } else if (op == 0xC101) {
    // Touch (TLBHack)
    ML_LOG("MagicIntercept", "writing 0x" << std::hex << arg2);

    if (RubySystem::TLBHack()) {
      MagicHandler(NULL, cpu, op, (int)arg1, arg2);
    }

    return retVal;
  }

  // contextId vs. threadId: threadId is the hardware thread executing on a
  // single cpu. contextId is the execution context for a given cpu.
  // Here we actually need a cpuId, which should be unique in the system.
  NetworkInterrupts* ni = NetworkInterrupts::LookupNIByCpu(cpu->contextId());

  if (ni == NULL) {
    std::cout << "WARNING!  Magic instruction called from processor that lacks a network interface." << std::endl;
    std::cout << "CPU: " << cpu->threadId() << " called with magic instr : " << op << std::endl;
    std::cout << "If you are seeing this message, something is likely quite wrong." << std::endl;
    assert(0);
  }

  NetworkInterruptHandle* nih = ni->GetHandle();
  assert(nih);

  switch (op) {
  case (0xC010): { //MAGIC_LCACC_RESERVE
    int thread = (int)arg1; //cpu->readIntReg(X86ISA::INTREG_RSI);  //SIM_read_register(cpu, SIM_get_register_number(cpu, "l3"));
    int lcacc = (int)arg2; //cpu->readIntReg(X86ISA::INTREG_RDX); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l4"));
    int delay = (int)arg3; //cpu->readIntReg(X86ISA::INTREG_RCX); // SIM_read_register(cpu, SIM_get_register_number(cpu, "l5"));
    std::cout << "LCAcc_Reserve: thread " << thread << " lcacc " << lcacc << " delay " << delay << std::endl;

    if (lcacc == 0) {
      assert(NetworkInterrupts::pendingReservation.find(thread)
             != NetworkInterrupts::pendingReservation.end());
      assert(NetworkInterrupts::pendingReservation[thread].size()
             >= 1);
      std::cout << "Reserving ";

      for (size_t i = 0; i < NetworkInterrupts::pendingReservation[thread].size(); i++) {
        std::cout << "[" << NetworkInterrupts::pendingReservation[thread][i] << "]";
      }

      std::cout << " from " << nih->deviceID << std::endl;
      std::vector<uint32_t> packet;
      int32_t target = 0;//gam target
      packet.push_back(GAM_CMD_RESERVE);
      packet.push_back(thread);
      packet.push_back(delay);
      packet.push_back(NetworkInterrupts::pendingReservation[thread].size());

      for (size_t i = 0; i < NetworkInterrupts::pendingReservation[thread].size(); i++) {
        packet.push_back(NetworkInterrupts::pendingReservation[thread][i]);
      }

      if (NetworkInterrupts::BiNCurveInfo.find(thread)
          != NetworkInterrupts::BiNCurveInfo.end()) {
        packet.push_back(NetworkInterrupts::BiNCurveInfo[thread].size());

        for (size_t i = 0; i < NetworkInterrupts::BiNCurveInfo[thread].size(); i++) {
          packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].bufferSize);
          packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].performance);
          packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].cacheImpact);
        }

        NetworkInterrupts::BiNCurveInfo.erase(thread);
      }

      nih->snpi->SendMessageOnDevice(nih->deviceID, target,
                                     &(packet[0]), sizeof(uint32_t) * packet.size());
      NetworkInterrupts::pendingReservation.erase(thread);
    } else {
      std::cout << "Adding reservation for " << lcacc << " to pending queue of thread " << thread << std::endl;
      NetworkInterrupts::pendingReservation[thread].push_back(lcacc);
    }
  }
  break;

  case (0xC019): { //MAGIC_LCACC_DECLARE_ACC
    //Declare accelerator use.  Used for TD arbitration with BiN
    int thread = (int)arg1; //cpu->readIntReg(X86ISA::INTREG_RSI); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l3"));
    NetworkInterrupts::AcceleratorDeclaration aDecl;
    aDecl.type = (int)arg2; //cpu->readIntReg(X86ISA::INTREG_RDX); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l4"));
    aDecl.count = (int)arg3; //cpu->readIntReg(X86ISA::INTREG_RCX); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l5"));
    NetworkInterrupts::accDeclInfo[thread].push_back(aDecl);
  }
  break;

  case (0xC020): { //MAGIC_BiN_CURVE
    std::cout << "***** MAGIC_SEND_BiN_CURVE\n";
    int thread = (int)arg1; //cpu->readIntReg(X86ISA::INTREG_RSI); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l3"));
    std::cout << "Adding BiN curve info for thread " << thread << std::endl;
    uint32_t size = (uint32_t)arg2; //cpu->readIntReg(X86ISA::INTREG_RDX); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l4"));

    if (size != 0) {
      //non zero size = valid element
      NetworkInterrupts::BiNPerformancePoint p;
      p.bufferSize = size;
      p.performance = (uint32_t)arg3; //cpu->readIntReg(X86ISA::INTREG_RCX); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l5"));
      p.cacheImpact = (uint32_t)arg4; //cpu->readIntReg(X86ISA::INTREG_R8); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l6"));
      NetworkInterrupts::BiNCurveInfo[thread].push_back(p);
    } else {
      //size zero is a special marker that indicates that we should fire off the existing curve points and request arbitration.
      assert(NetworkInterrupts::BiNCurveInfo.find(thread) != NetworkInterrupts::BiNCurveInfo.end());
      std::vector<uint32_t> packet;
      packet.push_back(BIN_CMD_ARBITRATE_REQUEST);
      packet.push_back(thread);
      packet.push_back(NetworkInterrupts::accDeclInfo[thread].size());
      packet.push_back(NetworkInterrupts::BiNCurveInfo[thread].size());

      for (size_t i = 0; i < NetworkInterrupts::accDeclInfo[thread].size(); i++) {
        packet.push_back(NetworkInterrupts::accDeclInfo[thread][i].type);
        packet.push_back(NetworkInterrupts::accDeclInfo[thread][i].count);
      }

      for (size_t i = 0; i < NetworkInterrupts::BiNCurveInfo[thread].size(); i++) {
        packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].bufferSize);
        packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].performance);
        packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].cacheImpact);
      }

      nih->snpi->SendMessageOnDevice(nih->deviceID, 0, &(packet[0]), sizeof(uint32_t) * packet.size());
      NetworkInterrupts::BiNCurveInfo.erase(thread);
      NetworkInterrupts::accDeclInfo.erase(thread);
    }
  }
  break;

  case (0xC011): { //MAGIC_LCACC_REQUEST
    int32_t target = 0;
    uint32_t buffer[3];
    buffer[0] = GAM_CMD_REQUEST;
    buffer[1] = (uint32_t)arg1;
    buffer[2] = (uint32_t)arg2;
    std::cout << "Requesting " << std::dec << (int32_t)buffer[2]
              << " from " << nih->deviceID << std::endl;
    nih->snpi->SendMessageOnDevice(nih->deviceID, target, buffer,
                                   sizeof(buffer));
  }
  break;

  case (0xC012): { //MAGIC_LCACC_COMMAND
    //std::cout << "Sending LCAcc Command to " << SIM_read_register(cpu, SIM_get_register_number(cpu, "l2")) << " from " << nih->deviceID << std::endl;
    uint32_t buffer[9];
    int target = (int)arg2; //cpu->readIntReg(X86ISA::INTREG_RDX); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l2"));
    buffer[0] = (uint32_t)arg3; //cpu->readIntReg(X86ISA::INTREG_RCX); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l3"));
    buffer[1] = (uint32_t)arg1; //cpu->readIntReg(X86ISA::INTREG_RSI); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l1"));
    BitConverter bc_vAddr;
    bc_vAddr.u64[0] = (uint64_t)arg4; //cpu->readIntReg(X86ISA::INTREG_R8); //SIM_read_register(cpu, SIM_get_register_number(cpu, "l4"));
    buffer[2] = bc_vAddr.u32[0];
    buffer[3] = bc_vAddr.u32[1];

    // ML_LOG(GetDeviceName(), "Sending LCAcc command addr 0x" << std::hex
    //     << bc_vAddr.u64[0] << " for thread " << buffer[1]);

    BitConverter bc;

    if (bc_vAddr.u64[0]) {
      if (tlbHackInterface &&
          tlbHackInterface->PageKnown(buffer[1],
                                      bc_vAddr.u64[0])) {
        bc.u64[0] = tlbHackInterface->Lookup(buffer[1],
                                             bc_vAddr.u64[0]);
        // std::cout << bc.u64[0] << ", " << TheISA::vtophys(cpu, bc_vAddr.u64[0]) << std::endl;
      } else {
        bc.u64[0] = TheISA::vtophys(cpu, bc_vAddr.u64[0]);
      }

      buffer[4] = bc.u32[0];
      buffer[5] = bc.u32[1];

      if (buffer[4] == 0 && buffer[5] == 0) {
        // ML_LOG(GetDeviceName(),
        //     "Command issued with un-translated non-zero TLB page");
        assert(buffer[4] == 0 && buffer[5] == 0);
      }
    } else {
      bc.u64[0] = 0;
      buffer[4] = 0;
      buffer[5] = 0;
    }

    buffer[6] = (uint32_t)arg5;
    buffer[7] = (uint32_t)arg6;
    buffer[8] = (uint32_t)arg7;

    retVal = 1;
    nih->snpi->SendMessageOnDevice(nih->deviceID, target, buffer, sizeof(buffer));
  }
  break;

  case (0xC013): { //MAGIC_LCACC_FREE
    int target = 0; //fixed GAM addr
    int32_t buffer[3];
    buffer[0] = GAM_CMD_RELEASE;
    buffer[1] = (int32_t)arg1;
    buffer[2] = (int32_t)arg2;

    nih->snpi->SendMessageOnDevice(nih->deviceID, target,
                                   buffer, sizeof(buffer));
  }
  break;

  case (0x911C): { //Get signal
    int thread = (int)arg3;
    //SIM_write_register(cpu, SIM_get_register_number(cpu, "l3"), ni->GetSignal(thread));
    //cpu->setFloatReg(X86ISA::INTREG_RCX, (X86ISA::IntReg)ni->GetSignal(thread));
    retVal = ni->GetSignal(thread);
  }
  break;

  case (0XCCCD): {
    fatal("Should never invoke KillThread in the benchmarks! Use BarrierTick and BarrierWait instead.");
  }
  break;

  case (0xCCCE): {
    System* sys = cpu->getSystemPtr();
    warn("===>>>>> Cycle: %u Tick: %u",
         sys->curCycle(), sys->clockEdge());
    warn("CPU %u", cpu->cpuId());
    warn("Thread %d", (int)arg1);
    warn("Debug Value %d", (int)arg2);
  }
  break;

  case (0xBA00): //Barrier Tick
  case (0xBA01): { //Barrier Blocked
    retVal = MagicIntercept(NULL, cpu, op, (int)arg2);
  }
  break;

  case (0xC101): { //Touch (TLBHack)
    MagicHandler(NULL, cpu, op, (int)arg1, arg2);
  }
  break;

  case (0xC000): { //MAGIC_LWI_REGISTER
    int thread = (int)arg1;
    int lcacc = (int)arg2;
    logical_address_t addr = (uint64_t)arg3;
    int max = (int32_t)arg5;

    uint64_t i;

    for (i = (int32_t)arg4; i < max; i++) {
      logical_address_t logicalAddress = addr + i;
      physical_address_t physicalAddress = TheISA::vtophys(cpu, logicalAddress);

      if (physicalAddress) {
        LWI_RegisterAccepter(thread, lcacc, physicalAddress, logicalAddress, i);
      } else {
        break;
      }
    } //End for loop

    retVal = i;
  }
  break;

  case (0xC004): { //MAGIC_LWI_CLEAR_INTERRUPT
    LWI_EndInterruptHandling(arg1);
  }
  break;

  case (0xC00C): { //MAGIC_LWI_CHECK
    int thread = (int)arg1;
    logical_address_t la = 0;
    LWI_StoredMessage msg;
    LWI_MessageAccepter accepter;

    if (LWI_GetMessagePair(thread, msg, accepter)) {
      assert(accepter.la_args.size());
      assert(accepter.la_args[0]);
      assert(accepter.pa_args.size());
      assert(accepter.la_args[1]);
      assert(msg.packet.size() < 100);
      assert(msg.packet.size() % 4 == 0);
      std::cout << "[LWI_MAGIC_CHECK] @ userthread " << thread
                << ": msg of size" << msg.packet.size() / 4
                << std::endl;

      for ( int i = 0; i < msg.packet.size() / 4; i++) {
        //TODO: Check if this works with new 64 bit addresses.
        assert(accepter.la_args.size() > i * 4);
        uint32_t* v = (uint32_t* )&msg.packet[i * 4];
        std::cout << i << ", " << accepter.la_args[i * 4] << ", " << accepter.pa_args[i * 4] << "," << *v << std::endl;
        LCAcc::SimicsInterface::WritePhysical(accepter.pa_args[i * 4], (void*)v, 4);
      }

      la = accepter.la_args[0];
      assert(la);
    }

    assert(la != 1 && la != 2);
    //SIM_write_register(cpu, reg, la);
    retVal = la;
  }
  break;
  }

  return retVal;
}
