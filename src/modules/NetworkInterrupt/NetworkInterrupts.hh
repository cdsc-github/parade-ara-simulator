
#ifndef NETWORK_INTERRUPTS_H
#define NETWORK_INTERRUPTS_H

#include <stdint.h>
#include <map>
#include <set>
#include <queue>
#include <vector>
#define SIM_ENHANCE
#define SIM_NET_PORTS
#include "../../mem/ruby/system/gem5NetworkPortInterface.hh"
#include "../Common/BaseCallbacks.hh"
#include "../../cpu/thread_context.hh"

typedef struct NetworkInterruptHandle_t
{
	gem5NetworkPortInterface* snpi;
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
public:
	static std::map<int, std::vector<BiNPerformancePoint> > BiNCurveInfo; //key thread, value bin point
	static std::map<int, std::vector<AcceleratorDeclaration> > accDeclInfo; //key thread, value declaration
	static std::map<int, std::vector<int> > pendingReservation; //key threadID, vector of lcaccID's
	static NetworkInterrupts* LookupNIByCpu(int cpu);
	NetworkInterrupts(NetworkInterruptHandle* x);
	~NetworkInterrupts();

	NetworkInterruptHandle* GetHandle() {return nih;}
	int GetSignal(int thread);
	void RaiseInterrupt(int source, int threadID, const void* buffer, int bufferSize);
	void PutOffInterrupt(int source, int threadID, const void* buffer, int bufferSize, int delay);
	void RecvMessage(int source, const char* buffer, int size);
	typedef MemberCallback4<NetworkInterrupts, int, int, const void*, int, &NetworkInterrupts::RaiseInterrupt> RaiseInterruptCB;
	typedef Arg3MemberCallback<NetworkInterrupts, int, const char*, int, &NetworkInterrupts::RecvMessage> RecvMessageCB;
	typedef MemberCallback1<NetworkInterrupts, int, &NetworkInterrupts::TryRaise> TryRaiseCB;
};

void HandleNetworkMessage(void*, int, int, const char*, int);
void HandleEvent(ThreadContext* obj, void* arg);
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

uint64_t LCAccMagicIntercept(void*, ThreadContext* cpu, int32_t op, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6, uint64_t arg7);
NetworkInterruptHandle* createNetworkInterruptHandle(int portID, int deviceID, int procID);
void HandleEvent(void* arg);

extern std::vector<std::queue<CBContainer> > cycleCBRing;
bool localCBsForCycle(Cycles current_cycle);
std::queue<CBContainer>& getCurrentCycleQueue(Cycles current_cycle);
void retireCBsForCycle(Cycles current_cycle);

#endif
