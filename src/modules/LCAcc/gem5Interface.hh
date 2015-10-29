#ifndef SIMICSINTERFACE_H
#define SIMICSINTERFACE_H

#include "../Common/BaseCallbacks.hh"
#include "LCAccManager.hh"
#include <string>
#include <vector>
#include <stdint.h>
#include <map>
#include "mem/protocol/RubyRequestType.hh"
#include "Interface.hh"

namespace LCAcc {

class gem5Interface
{
	static std::map<int, std::vector<Arg3CallbackBase<int, const void*, unsigned int>*> > netHandlers;
public:
	static LCAcc::LCAccManager manager;
	static unsigned long long GetSystemTime();
	static bool SendMessage(int source, int destination, unsigned int msgSize, const void* buffer);
	static void RegisterCallback(CallbackBase* cb, int delay);
        static void ReadPhysical(uint64_t address, void* storage, size_t size);
        static void WritePhysical(uint64_t address, const void* storage, size_t size);
	static void HandleMessage(int src, int dst, const void* buffer, int bufferSize);
        static void TimedBufferRead(int cpu, uint64_t addr, size_t size, int buffer, CallbackBase* cb);
	static void TimedBufferWrite(int cpu, uint64_t addr, size_t size, int buffer, CallbackBase* cb);
	static void RegisterLCAcc(int id, int node, std::vector<int> opmodes, uint32_t threadID);
	static void UnregisterLCAcc(int id);
};

LCAccDeviceHandle* CreateNewLCAccDeviceHandle();
LCAccInterface* CreateLCAccInterface();
int DeleteLCAccDeviceHandle(LCAccDeviceHandle* handle);
void Interface_AddOperatingModeHandler(LCAccDeviceHandle* handle, const char* name);
void Interface_SetNetPortHandler(LCAccDeviceHandle* handle, int port, int device);
void Interface_SetTimingHandler(LCAccDeviceHandle* handle, unsigned int cycleTime, unsigned int pipelineDepth, unsigned int initiationInterval);
void Interface_SetPrefetchDistanceHandler(LCAccDeviceHandle* handle, int distance);
void Interface_SetSPMConfigHandler(LCAccDeviceHandle* handle, int banks, int bankMod, int readPorts, int readLatency, int writePorts, int writeLatency);
void Interface_InitializeHandler(LCAccDeviceHandle* handle, uint32_t thread);
bool Interface_IsIdleHandler(LCAccDeviceHandle* handle);
void Interface_ProcessMessageHandler(void* handle, int src, int destination, const char* msgBody, int msgLength);
int Interface_GetOpCodeHandler(const char* opname);
const char* Interface_GetOpNameHandler(int opCode);
void Interface_GetOpTimingHandler(int opcode, unsigned int* ii, unsigned int* pipeDepth, unsigned int* cycleMult);
void Interface_HookToMemoryDeviceHandler(LCAccDeviceHandle* handle, const char* deviceName);
void SNPIRecvHandler(void*, int src, int dst, const char* buffer, int size);
void CallCBWrapper(void* args);

}

#endif
