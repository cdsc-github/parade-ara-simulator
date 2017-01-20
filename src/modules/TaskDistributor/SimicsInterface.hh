#ifndef SIMICSINTERFACE_H
#define SIMICSINTERFACE_H

#include "../Common/BaseCallbacks.hh"
#include <string>
#include <vector>
#include <stdint.h>
#include <map>

#include "TDManager.hh"

namespace TaskDistributor
{

class SimicsInterface
{
	static std::map<int, std::vector<Arg3CallbackBase<int, const void*, unsigned int>*> > netHandlers;
public:
        //static unsigned long long GetSystemTime();
	static TDManager manager;
	static bool SendMessage(int source, int destination, unsigned int msgSize, const void* buffer);
	static void RegisterCallback(CallbackBase* cb, int delay);
	static void ReadPhysical(uint64_t address, void* storage, size_t size);
	static void WritePhysical(uint64_t address, const void* storage, size_t size);
	static void HandleMessage(int src, int dst, const void* buffer, int bufferSize);
	static int GetBufferID(int core);
	static void FreeBuffer(int nodeID, int buffer, CallbackBase* cb);
	static void TryBufferAllocate(int nodeID, int buffer, unsigned int size, Arg1CallbackBase<bool>* cb);
	/*static bool CreateSPM(int id);
	static bool CreateDMA(int id);
	static void CreateAccelerator(const std::string& name, const std::vector<int>& opCodeSet, int networkNode, int networkDeviceID);
        static void SetAcceleratorTiming(const std::string& name, int ii, int pipelineDepth, int clockMult);//*/
	static void GetOpModeTiming(int opCode, unsigned int* ii, unsigned int* pipelineDepth, unsigned int* clockMult);
};

// static TDManager manager;

}

#endif
