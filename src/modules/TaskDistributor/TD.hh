#ifndef TD_H
#define TD_H

#include <map>
#include <set>
#include <queue>
#include <vector>
#include <string>
#include <cstdio>
#include "../Common/BaseCallbacks.hh"
#include "../Common/PolyhedralAddresser.hh"
#include "../Common/Packetizer.hh"
#include "../Common/TransferDescription.hh"
#include "NetworkInterface.hh"
// #include "SimicsInterface.hh"
#include "CFUIdentifier.hh"

class DMAController;
class BaseTLBMemory;
class SPMInterface;
class PatternSelector;
class TDProgram;
class TD
{
    class JobDescription
    {
      public:
        const TDProgram* hostProgram;
        uint32_t taskStart;
        uint32_t taskEnd;
        void WriteJobSegment(int logicalCFU,
        const std::map<int, int>& logicalToPhysCFUs,
        PacketBuilder& pb) const;
        inline JobDescription()
        {
            hostProgram = NULL;
            taskStart = taskEnd = 0;
        }
    };

    class TaskReadData
    {
      public:
        int process;
        uint64_t logicalAddr;
        uint64_t physicalAddr;
        uint32_t size;
        bool useSharedBuffer;
        uint32_t sharedBufferSize;
        PacketReader* loadedPacket;
        inline TaskReadData()
        {
            loadedPacket = NULL;
            process = 0;
            logicalAddr = physicalAddr = 0;
            size = 0;
            useSharedBuffer = false;
            sharedBufferSize = 0;
        }
    };

    SPMInterface* spm;
    NetworkInterface* netPort;
    DMAController* dma;
    bool currentlyReadingTask;
    bool currentlyAllocating;
    uint32_t myThreadID;
    uint32_t defaultTaskGrain;
    bool addingFpgaAccelerators;
    std::queue<TaskReadData> pendingTaskReads;

    //key process, value size in bytes
    std::map<unsigned int, unsigned int> selectedBiNSize;

    //key process, value queue of pending tasks for asnyc
    std::map<unsigned int, std::queue<TaskReadData> > stalledTaskReads;

    //key process, value program
    std::map<unsigned int, TDProgram*> programSet;

    //key process, value buffer size
    std::map<unsigned int, unsigned int> requiredBufferSize;

    //key cfu id, value process
    std::map<int, unsigned int> cfuUseMap;

    //key cfu id, value buffer id
    std::map<int, int> cfuBufferMap;

    //key buffer id, value reference count
    std::map<int, unsigned int> bufferReferenceCount;

    //key process.  These are programs for which an allocation is pending
    std::set<int> busyPrograms;

    // key process.  These are programs that have failed an allocation, prior to
    // any buffers being freed.  Repeated attempts are all doomed to fail, thus
    // resulting in deadlock
    std::set<int> stalledPrograms;

    PatternSelector* patternSelector;
    std::vector<CFUIdentifier> systemCfuSet;

    //key cfu id, value index into systemCfuSet
    std::map<int, int> cfuIndexMap;

    //key process, value <key logical addr, value vector of cfu ids>
    std::map<int, std::map<uint64_t, std::vector<int> > > cfuTlbMisses;

    //key process, value job queue
    std::map<unsigned int, std::queue<JobDescription> > pendingJobSet;

    //key process id, value core id
    std::map<unsigned int, int> lastKnownCore;

    //key network node, value free buffer set
    std::map<uint32_t, std::vector<int> > freeBuffers;

    //key buffer ID, value network node
    std::map<int, uint32_t> bufferNodes;

    //key process id, value set of legal CFUs to use.
    std::map<unsigned int, std::set<int> > cfuUseFilter;

    std::vector<uint32_t> ExtractMemoryManifest(const TransferDescription& td, uint32_t task);

    int cfusPerIsland;
    //added for FPGA work
    class FpgaRecipe
    {
      public:
        int opCode;
        int ii;
        int pipelineDepth;
        int clockMultiplier;
        int area;
    };

    std::map<int, int> fpgaPortLoad;
    std::vector<FpgaRecipe> fpgaRecipeSet;
    int fpgaAreaRemaining;
    int fpgaDeviceIDCounter;
    std::map<int, FpgaRecipe*> fpgaCfuRecipe; //key= cfuID
    std::vector<int> fpgaOpCodeSummary;
    std::map<int, int> fpgaOwningThread;
    std::map<int, std::queue<int> > fpgaCfuPool; //key port, value list of acc device ids
    std::map<int, int> fpgaCfuPortMap; //key id, value port
    std::map<int, std::string> fpgaCfuName; //key id, value name
    bool TryAllocateFpga(std::vector<CFUIdentifier>& id, const std::vector<int>& type, int minLatency, int maxLatency, bool probeAllocate);
    void DeallocateFpga(int id);
    bool IsFpgaCfu(int id);
    //end added for FPGA work
    void MsgHandler(int, const void*, unsigned int);
    void TransferComplete();
    void ExecuteProgram(TaskReadData& trd);
    void ReadProgramDone();
    void StartProgramRead();
    void HandleTLBMiss(uint64_t);
    void HandleAccessViolation(uint64_t);
    void FreeBuffer(int bufferID, bool silent);
    int AcquireBufferID(uint32_t node);
    void BufferFreeComplete(int bufferID);
    void AllocateBufferComplete(bool success, int bufferSize, int bufferID, std::vector<CFUIdentifier> selected, unsigned int process);
    void TryNewAllocation();
    void AllocateMore();
    void translate(int src, uint64_t logicalPage);

    inline std::string GetDeviceName()
    {
        char s[100];
        sprintf(s, "taskdistributor.%02d", netPort->GetNodeID());
        return s;
    }

    typedef Stored4Arg1MemberCallback<TD, bool, int, int, std::vector<CFUIdentifier>, unsigned int, &TD::AllocateBufferComplete> AllocateBufferCompleteCB;
    typedef MemberCallback5<TD, bool, int, int, std::vector<CFUIdentifier>, unsigned int, &TD::AllocateBufferComplete> AllocateBufferCompleteConstCB;
    typedef MemberCallback1<TD, int, &TD::BufferFreeComplete> BufferFreeCompleteCB;
    typedef MemberCallback0<TD, &TD::AllocateMore> AllocateMoreCB;
    typedef Arg3MemberCallback<TD, int, const void*, unsigned int, &TD::MsgHandler> MsgHandlerCB;
    typedef MemberCallback0<TD, &TD::TransferComplete> TransferCompleteCB;
    typedef MemberCallback0<TD, &TD::StartProgramRead> StartProgramReadCB;
    typedef MemberCallback0<TD, &TD::ReadProgramDone> ReadProgramDoneCB;
    typedef Arg1MemberCallback<TD, uint64_t, &TD::HandleTLBMiss> HandleTLBMissCB;
    typedef Arg1MemberCallback<TD, uint64_t, &TD::HandleAccessViolation> HandleAccessViolationCB;
    typedef MemberCallback2<TD, int, uint64_t, &TD::translate> translateCB;

public:
    static std::vector<std::string> GetSelectionAlgs();
    static PatternSelector* GetPatternSelector(const std::string& name);
    TD();
    void AddNetworkInterface(NetworkInterface* port);
    void AddCFU(int id, int cpu, int type);
    void RemoveCFU(int id);
    void SetSelector(const std::string& selectorName);
    void SetCFUAllocationPerIsland(int cfuCount);
    void SetTaskGrain(int grain);
    void SetFpgaArea(int area);
    void AddFpgaPort(int networkPort);
    void AddFpgaRecipe(int opCode, int area, int ii, int pipelineDepth, int cycleMult);
    void ClearCFUFilter(uint32_t thread);
    void AddCFUFilter(uint32_t thread, int cfuID);
    void Initialize();

protected:
    // Access Stats
    uint64_t hits;
    uint64_t misses;
    uint64_t mshrhits;
    uint64_t flushTlb;

public:
    // shared TLB entries
    int numEntries;

    int associativity;

    int hitLatency;

    BaseTLBMemory *tlb;

    void flushAll();

    uint64_t getTlbHits() { return hits + mshrhits; }
    uint64_t getTlbMisses() { return misses; }
    uint64_t getTlbFlush() { return flushTlb; }

};

#endif
