#ifndef LCACCDEVICE_H
#define LCACCDEVICE_H

#include "../Common/BaseCallbacks.hh"
#include "LCAccOperatingMode.hh"
#include "ObjFactory.hh"
#include "NetworkInterface.hh"
#include <map>
#include <queue>
#include <vector>
#include <string>
#include <cstdio>
#include <cassert>
#include "LCAccCommandListing.hh"
#include "../Common/PolyhedralAddresser.hh"
#include "../Common/Packetizer.hh"
#include "../Common/ComputeDescription.hh"

namespace LCAcc
{
class DMAController;
class SPMInterface;
class LCAccDevice
{
  class ComputeOrder;
  class Action;
  class ComputeElement
  {
  public:
    ComputeOrder* host;
    uint32_t index;
    uint32_t maxCompute;
    uint32_t taskID;
    size_t readsRemaining;
    size_t writesRemaining;
    size_t randomAccessesRemaining;
    bool hasComputed;
  };
  class ComputeOrder
  {
  public:
    LCAccOperatingMode* mode;
    Action* host;
    uint32_t computeCount;
    uint64_t indexOffset;
    uint32_t pendingTransfers;
    uint32_t computesToEmit;
    uint32_t computesToFinish;
    uint32_t currentComputeElement;
    uint32_t pendingComputeElements;
    uint32_t taskID;
    uint32_t maxCompute;
    std::vector<PolyhedralAddresser> argumentAddressGen;
    std::vector<bool> argumentActive;
    std::vector<uint64_t> registers;
    inline unsigned int CalcArgAddress(int argument, int index) const
    {
      return indexOffset + argumentAddressGen[argument].GetAddr(index);
    }
    ComputeElement* NextComputeElement()
    {
      assert(currentComputeElement < computeCount);
      assert(mode);
      std::vector<uint64_t> baseArgAddr;
      ComputeElement* x = new ComputeElement;
      x->host = this;
      x->index = currentComputeElement;
      x->hasComputed = false;
      x->maxCompute = maxCompute;
      x->taskID = taskID;
      x->readsRemaining = x->writesRemaining = 0;
      currentComputeElement++;
      return x;
    }
    ComputeOrder(LCAccOperatingMode* mode, uint32_t computeCount, uint32_t taskID, uint64_t indexOffset, const std::vector<PolyhedralAddresser>& argumentAddressGen, const std::vector<bool>& argumentActive, const std::vector<uint64_t>& registers)
    {
      this->host = NULL;
      this->mode = mode;
      this->taskID = taskID;
      maxCompute = computeCount;
      this->indexOffset = indexOffset;
      this->argumentAddressGen = argumentAddressGen;
      this->registers = registers;
      this->maxCompute = computeCount;
      this->argumentActive = argumentActive;
      this->computesToEmit = this->computesToFinish = this->computeCount = (mode->CallAllAtEnd()) ? 1 : maxCompute;
      currentComputeElement = 0;
      pendingTransfers = pendingComputeElements = 0;
    }
  };
  class EndNodeTransferData
  {
  public:
    std::vector<int32_t> strideSet;
    std::vector<uint32_t> sizeSet;
    uint64_t addr;
    int spm;
    inline bool IsSPM() const
    {
      return spm != -1;
    }
    inline bool IsMemory() const
    {
      return !IsSPM();
    }
    inline PolyhedralAddresser GetPolyhedralAddresser() const
    {
      return PolyhedralAddresser(addr, sizeSet, strideSet);
    }
  };
  class TransferOrder
  {
  public:
    EndNodeTransferData src;
    EndNodeTransferData dst;
    size_t elementSize;
    size_t totalSize;
  };
  class Action
  {
    Action() {}
    Action(const Action&) {}
  public:
    ComputeOrder* compute;
    std::vector<TransferOrder> prefetchSet;
    std::vector<TransferOrder> readSet;
    std::vector<TransferOrder> writeSet;
    int readsInFlight;
    int writesInFlight;
    bool notifyOnComplete;
    int user;
    int taskID;
    int numberOfTasks;
    bool computeReady;
    bool retireReady;
    ~Action()
    {
      if (compute) {
        delete compute;
      }
    }
    Action(int taskID)
    {
      readsInFlight = writesInFlight = 0;
      compute = NULL;
      notifyOnComplete = false;
      this->taskID = taskID;
      computeReady = retireReady = false;
    }
  };
  class TaskBlock
  {
  public:
    uint64_t startingAddress;
    uint32_t bufferSize;
    uint32_t stride;
    uint32_t lowMark;
    uint32_t taskType;
  };
  class Priority
  {
  public:
    static const int Low = -100;
    static const int Medium = 0;
    static const int High = 100;
  };
  static const uint32_t tt_signature = 10;
  static bool initializedModeFactory;
  static ObjFactory<std::string, LCAccOperatingMode> modeFactory;
  static std::map<int, LCAccOperatingMode*> modeSampleSet;
  static std::map<std::string, int> nameToMode;
  static std::map<int, std::string> modeToName;
  int pendingComputeEmittedTransfers;
  std::map<int, LCAccOperatingMode*> modes;
  SPMInterface* spm;
  uint64_t nextComputeAvailable;
  NetworkInterface* netPort;
  DMAController* dma;
  int currentUser;
  int currentUserProc;
  std::queue<Action*> pendingActionSet;
  std::queue<Action*> awaitingComputeActionSet;
  std::queue<Action*> awaitingRetireActionSet;
  std::queue<TaskBlock> pendingTasksToRead;
  int spmTaskLoadAddr;
  uint32_t spmWindowCount;
  uint32_t spmWindowSize;
  int32_t bufferID;
  int32_t bufferSize;
  int prefetchDistance;
  bool lcaActive;

  std::string lcaccDeviceName;

  void PrefetchOrder(const TransferOrder& to);
  void EmitActionReads(Action* a);
  void FinishedActionRead(Action* a, size_t index);
  void EmitActionCompute(Action* a);
  void SPMReadComplete(ComputeElement* ce);
  void IssueComputeElement(ComputeElement* ce);
  void AnnounceComputeBegin(ComputeElement* ce);
  void PerformComputeElement(ComputeElement* ce);
  void ComputeRandomAccessComplete(ComputeElement* ce, int iteration, int maxIteration, int taskID, uint64_t spmAddr, uint64_t memAddr, int accessType);
  void SPMWriteComplete(ComputeElement* ce);
  void TryRetireComputeElement(ComputeElement* ce);
  void EmitActionWrites(Action* a);
  void FinishedActionWrite(Action* a, size_t index);
  void RetireAction(Action* a);

  unsigned int CalculateAddressOffset(int index, const std::vector<unsigned int>& size, const std::vector<int>& stride, int maxDepth);
  void StartTask();
  void MoveToNextTask();
  void Compute();
  void ComputeEmittedTransferComplete();
  void OutputPendingComputeTransfers();
  void SetOperatingState(int state);
  void SetSystemArgState(int index, int arg);
  void MsgHandler(int, const void*, unsigned int);
  void TransferComplete();
  void ReadTaskSignatureDone();
  void ParseTaskSignature(PacketReader& pr);
  void StartTaskRead();
  void HandleTLBMiss(uint64_t);
  void HandleAccessViolation(uint64_t);
  inline std::string GetDeviceName()
  {
    char suffix[20];
    sprintf(suffix, ".%02d", netPort->GetNodeID());
    return lcaccDeviceName + suffix;
  }
  typedef MemberCallback1<LCAccDevice, ComputeElement*, &LCAccDevice::SPMReadComplete> SPMReadCompleteCB;
  typedef MemberCallback1<LCAccDevice, ComputeElement*, &LCAccDevice::IssueComputeElement> IssueComputeElementCB;
  typedef MemberCallback1<LCAccDevice, ComputeElement*, &LCAccDevice::AnnounceComputeBegin> AnnounceComputeBeginCB;
  typedef MemberCallback1<LCAccDevice, ComputeElement*, &LCAccDevice::PerformComputeElement> PerformComputeElementCB;
  typedef MemberCallback7<LCAccDevice, ComputeElement*, int, int, int, uint64_t, uint64_t, int, &LCAccDevice::ComputeRandomAccessComplete> ComputeRandomAccessCompleteCB;
  typedef MemberCallback1<LCAccDevice, ComputeElement*, &LCAccDevice::SPMWriteComplete> SPMWriteCompleteCB;
  typedef MemberCallback2<LCAccDevice, LCAccDevice::Action*, size_t, &LCAccDevice::FinishedActionRead> FinishedActionReadCB;
  typedef MemberCallback1<LCAccDevice, LCAccDevice::Action*, &LCAccDevice::EmitActionWrites> EmitActionWritesCB;
  typedef MemberCallback2<LCAccDevice, LCAccDevice::Action*, size_t, &LCAccDevice::FinishedActionWrite> FinishedActionWriteCB;

  typedef MemberCallback0<LCAccDevice, &LCAccDevice::OutputPendingComputeTransfers> OutputPendingComputeTransfersCB;
  typedef MemberCallback0<LCAccDevice, &LCAccDevice::ComputeEmittedTransferComplete> ComputeEmittedTransferCompleteCB;
  typedef Arg3MemberCallback<LCAccDevice, int, const void*, unsigned int, &LCAccDevice::MsgHandler> MsgHandlerCB;
  typedef MemberCallback0<LCAccDevice, &LCAccDevice::Compute> ComputeCB;
  typedef MemberCallback0<LCAccDevice, &LCAccDevice::TransferComplete> TransferCompleteCB;
  typedef MemberCallback0<LCAccDevice, &LCAccDevice::ReadTaskSignatureDone> ReadTaskSignatureDoneCB;
  typedef Arg1MemberCallback<LCAccDevice, uint64_t, &LCAccDevice::HandleTLBMiss> HandleTLBMissCB;
  typedef Arg1MemberCallback<LCAccDevice, uint64_t, &LCAccDevice::HandleAccessViolation> HandleAccessViolationCB;
  typedef MemberCallback0<LCAccDevice, &LCAccDevice::StartTaskRead> StartTaskReadCB;

  int mCycleTime;
  int mPipelineDepth;
  int mInitiationInterval;
  std::vector<int> prefetcherSet;
  int spmBanks;
  int spmBankMod;
  int spmReadPortCount;
  int spmWritePortCount;
  int spmReadPortLatency;
  int spmWritePortLatency;
  std::string memoryDeviceHookName;
  void CalculateSPMMapping(int bankCount, int bankMod, int readPorts, int writePorts, const ComputeDescription& compute, std::map<unsigned int, unsigned int>& addrMap);
public:
  static inline int GetOpCode(const std::string& name)
  {
    assert(nameToMode.find(name) != nameToMode.end());
    return nameToMode[name];
  }
  static inline const char* GetOpName(int opCode)
  {
    assert(modeToName.find(opCode) != modeToName.end());
    return modeToName[opCode].c_str();
  }
  static inline void GetOpTiming(int opCode, unsigned int& ii, unsigned int& pipe, unsigned int& cycleMult)
  {
    assert(modeSampleSet.find(opCode) != modeSampleSet.end());
    LCAccOperatingMode* mode = modeSampleSet[opCode];
    assert(mode);
    cycleMult = mode->CycleTime();
    pipe = mode->PipelineDepth();
    ii = mode->InitiationInterval();
  }
  LCAccDevice();
  ~LCAccDevice();
  void AddOperatingMode(const std::string& name);
  void AddNetworkInterface(NetworkInterface* port);
  void AddProgrammablePrefetcher(int networkID);
  void Initialize(uint32_t threadID);
  bool IsIdle();
  void SetPrefetchDistance(int dist);
  void SetSPMConfig(int banks, int bankMod, int readPorts, int readLatency, int writePorts, int writeLatency);
  void SetTiming(unsigned int ii, unsigned int pipeline, unsigned int cycle);
  void HookToMemoryDevice(const std::string& name);
  SPMInterface* getSPM()
  {
    return spm;
  }
};
}

#endif
