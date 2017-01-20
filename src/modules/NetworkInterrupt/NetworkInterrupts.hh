
#ifndef NETWORK_INTERRUPTS_H
#define NETWORK_INTERRUPTS_H

#define SIMICS30
#include <stdint.h>
#include <map>
#include <set>
#include <queue>
#include <vector>
//#include <simics/api.h>
//#include <simics/alloc.h>
//#include <simics/utils.h>
#define SIM_ENHANCE
#define SIM_NET_PORTS
#include "../../mem/ruby/system/SimicsNetworkPortInterface.hh"
#include "../Common/BaseCallbacks.hh"
#include "../../cpu/thread_context.hh"
#include "cpu/translation.hh"

#include "arch/x86/pagetable.hh"
#include "base/trie.hh"

typedef struct NetworkInterruptHandle_t
{
    SimicsNetworkPortInterface* snpi;
    int portID;
    int deviceID;
    ThreadContext *attachedCPU;
    int procID;
}NetworkInterruptHandle;

class NetworkInterrupts
{
  public:
    class Msg
    {
      public:
        std::vector<uint8_t> packet;
        int thread;
        int lcacc;
        int priority;
        inline bool operator <(const Msg& m) const
        {
          return priority < m.priority;
        }
    };

    class BiNPerformancePoint
    {
      public:
        uint32_t bufferSize;
        uint32_t performance;
        uint32_t cacheImpact;
    };

    class AcceleratorDeclaration
    {
      public:
        int type;
        int count;
    };
  private:
    static std::map<int, NetworkInterrupts*> cpuMap; //key cpu simics object
    NetworkInterruptHandle* nih;
    std::map<int, std::priority_queue<Msg> > pendingMsgs; //key thread
    std::map<int, std::queue<int> > pendingSignals;
    void TryRaise(int thread);
    int CalcPriority(int source, int threadID, const std::vector<uint8_t>& packet);

    uint32_t hostPTWLatency;

    uint64_t hostPTWalks;
    uint64_t hostPTWalkTick;
    uint64_t hostPTWalkTime;

    std::list<RequestPtr> pendingTranslations;

    enum State {
        Ready,
        Waiting
    };

    State currState;

    int interval;

  protected:
    uint32_t tlbSize;

    typedef std::list<X86ISA::TlbEntry *> EntryList;

    X86ISA::TlbEntry * tlb;

    EntryList freeList;

    TlbEntryTrie trie;

    uint64_t lruSeq;

  public:
    static std::map<int, std::vector<BiNPerformancePoint> > BiNCurveInfo; //key thread, value bin point
    static std::map<int, std::vector<AcceleratorDeclaration> > accDeclInfo; //key thread, value declaration
    static std::map<int, std::vector<int> > pendingReservation; //key threadID, vector of lcaccID's
    static NetworkInterrupts* LookupNIByCpu(int cpu);

    std::vector<int> queueLen;

    // typedef NetworkInterruptsParams Params;
    // NetworkInterrupts(const Params *p);

    NetworkInterrupts(NetworkInterruptHandle* x);
    ~NetworkInterrupts();

    NetworkInterruptHandle* GetHandle() {return nih;}
    int GetSignal(int thread);
    void RaiseInterrupt(int source, int threadID, const void* buffer, int bufferSize);
    void PutOffInterrupt(int source, int threadID, const void* buffer, int bufferSize, int delay);
    void RecvMessage(int source, const char* buffer, int size);
    void HostPTWalk(RequestPtr req);
    void sampleQueueLen();
    typedef MemberCallback4<NetworkInterrupts, int, int, const void*, int, &NetworkInterrupts::RaiseInterrupt> RaiseInterruptCB;
    typedef Arg3MemberCallback<NetworkInterrupts, int, const char*, int, &NetworkInterrupts::RecvMessage> RecvMessageCB;
    typedef MemberCallback1<NetworkInterrupts, int, &NetworkInterrupts::TryRaise> TryRaiseCB;
    typedef MemberCallback1<NetworkInterrupts, RequestPtr, &NetworkInterrupts::HostPTWalk> HostPTWalkCB;
    typedef MemberCallback0<NetworkInterrupts, &NetworkInterrupts::sampleQueueLen> sampleQueueLenCB;

    inline std::string GetDeviceName()
    {
      char s[20];
      sprintf(s, "netinterrupts.%02d", nih->portID);
      return s;
    }

    void finishTranslation(WholeTranslationState *state);

    void startWalk();

    void setupWalk(int thread, uint64_t vAddr);

    /** This function is used by the page table walker to determin if it could
    * translate a pending request or if the underlying request has been
    * squashed. This always returns false for the accelerater as it never
    * executes any instructions speculatively.
    */
    bool isSquashed() const { return false; }

    uint64_t getHostPTWalks() { return hostPTWalks; }

    uint64_t getHostPTWalkTime() { return hostPTWalkTime; }

    X86ISA::TlbEntry *lookup(uint64_t va, bool update_lru = true);

    void flushAll();

    void evictLRU();

    uint64_t nextSeq()
    {
        return ++lruSeq;
    }

    X86ISA::TlbEntry * insert(uint64_t vpn, X86ISA::TlbEntry &entry);
};

void HandleNetworkMessage(void*, int, int, const char*, int);
//void HandleEvent(conf_object_t* obj, void* arg);
void HandleEvent(ThreadContext* obj, void* arg);
//void EnqueueEvent(conf_object_t* obj, void* arg, int dt);
void EnqueueEvent(void* arg, int dt);

class CBContainer
{
  void (*cb)(void*);
  void* args;
public:
  CBContainer(void (*cb)(void*), void* args)
  {
    this->cb = cb;
    this->args = args;
  }
  void call()
  {
    cb(args);
  }
};

//X86ISA::IntReg LCAccMagicIntercept(void*, ThreadContext* cpu, int32_t op);
uint64_t LCAccMagicIntercept(void*, ThreadContext* cpu, int32_t op, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6, uint64_t arg7);
NetworkInterruptHandle* createNetworkInterruptHandle(int portID, int deviceID, int procID);
void HandleEvent(void* arg);

extern std::vector<std::queue<CBContainer> > cycleCBRing;
bool localCBsForCycle(Cycles current_cycle);
std::queue<CBContainer>& getCurrentCycleQueue(Cycles current_cycle);
void retireCBsForCycle(Cycles current_cycle);

#endif
