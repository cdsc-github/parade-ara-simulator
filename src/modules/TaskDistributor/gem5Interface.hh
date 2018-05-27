#ifndef SIMICSINTERFACE_H
#define SIMICSINTERFACE_H

#include "../Common/BaseCallbacks.hh"
#include <string>
#include <vector>
#include <stdint.h>
#include <map>

namespace TaskDistributor
{

class gem5Interface
{
  static std::map<int, std::vector<Arg3CallbackBase<int, const void*, unsigned int>*> > netHandlers;
public:
  static bool SendMessage(int source, int destination, unsigned int msgSize, const void* buffer);
  static void RegisterCallback(CallbackBase* cb, int delay);
  static void ReadPhysical(uint64_t address, void* storage, size_t size);
  static void WritePhysical(uint64_t address, const void* storage, size_t size);
  static void HandleMessage(int src, int dst, const void* buffer, int bufferSize);
  static int GetBufferID(int core);
  static void FreeBuffer(int nodeID, int buffer, CallbackBase* cb);
  static void TryBufferAllocate(int nodeID, int buffer, unsigned int size, Arg1CallbackBase<bool>* cb);
  static void GetOpModeTiming(int opCode, unsigned int* ii, unsigned int* pipelineDepth, unsigned int* clockMult);
};

}

#endif
