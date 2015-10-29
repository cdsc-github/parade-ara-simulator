#include "gem5Interface.hh"
#include <vector>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <strings.h>

#include "Interface.hh"
#include "LCAccDevice.hh"
#define SIM_ENHANCE
#define SIM_NET_PORTS
#define SIM_MEMORY

#include "../Common/mf_api.hh"
#include "../../mem/ruby/system/gem5NetworkPortInterface.hh"
#include "../NetworkInterrupt/NetworkInterrupts.hh"
#include "../../mem/ruby/system/System.hh"
#include "memInterface.hh"
#define SP_WORD_SIZE 8

using namespace LCAcc;

namespace LCAcc {

LCAcc::LCAccManager gem5Interface::manager;
std::map<int, std::vector<Arg3CallbackBase<int, const void*, unsigned int>*> > gem5Interface::netHandlers;
static void ExecuteCB(void* cbArg);

gem5NetworkPortInterface* snpi;

LCAccDeviceHandle* CreateNewLCAccDeviceHandle()
{
	static int id = 0;
	LCAccDeviceHandle* handle = (LCAccDeviceHandle *)malloc(sizeof(LCAccDeviceHandle));
	bzero(handle, sizeof(LCAccDeviceHandle));
	assert(handle);
	handle->handleID = id++;
	gem5Interface::manager.RegisterDevice(handle->handleID);
	return handle;
}
int DeleteLCAccDeviceHandle(LCAccDeviceHandle* handle)
{
	assert(handle);
	gem5Interface::manager.UnregisterDevice(handle->handleID);
	snpi->UnregisterRecvHandler(handle->netHandle);
	snpi->UnbindDevice(handle->deviceID);
	return 0;
}
void Interface_AddOperatingModeHandler(LCAccDeviceHandle* handle, const char* name)
{
	assert(handle);
	gem5Interface::manager.AddOperatingMode(handle->handleID, name);
}
void Interface_SetNetPortHandler(LCAccDeviceHandle* handle, int port, int device)
{
	assert(snpi);
	assert(handle);
	handle->deviceID = device;
	snpi->BindDeviceToPort(port, device);
	warn("bind accelerator device %d to port %d\n", device, port);
	handle->netHandle = snpi->RegisterRecvHandlerOnDevice(device, Interface_ProcessMessageHandler, handle);
	gem5Interface::manager.AddNetworkPort(handle->handleID, port, device);
}
void Interface_SetTimingHandler(LCAccDeviceHandle* handle, unsigned int cycleTime, unsigned int pipelineDepth, unsigned int initiationInterval)
{
	assert(handle);
	gem5Interface::manager.SetTiming(handle->handleID, initiationInterval, pipelineDepth, cycleTime);
}
void Interface_SetPrefetchDistanceHandler(LCAccDeviceHandle* handle, int distance)
{
	assert(handle);
	gem5Interface::manager.SetPrefetchDistance(handle->handleID, distance);
}
void Interface_SetSPMConfigHandler(LCAccDeviceHandle* handle, int banks, int bankMod, int readPorts, int readLatency, int writePorts, int writeLatency)
{
	assert(handle);
	assert(readPorts > 0 && writePorts > 0 && readLatency >= 0 && writeLatency >= 0);
	assert(bankMod > 0);
	assert(banks > 0);
	gem5Interface::manager.SetSPMConfig(handle->handleID, banks, bankMod, readPorts, readLatency, writePorts, writeLatency);
}
void Interface_InitializeHandler(LCAccDeviceHandle* handle, uint32_t thread)
{
	assert(handle);
	gem5Interface::manager.FinalizeDeviceCreation(handle->handleID, thread);
}
bool Interface_IsIdleHandler(LCAccDeviceHandle* handle)
{
	assert(handle);
	return gem5Interface::manager.IsIdle(handle->handleID);
}
void Interface_ProcessMessageHandler(void* handle, int src, int destination, const char* msgBody, int msgLength)
{
        gem5Interface::manager.ProcessMessage(src, destination, (unsigned int)msgLength, msgBody);
}
int Interface_GetOpCodeHandler(const char* opname)
{
	return LCAcc::LCAccDevice::GetOpCode(std::string(opname));
}
const char* Interface_GetOpNameHandler(int opCode)
{
	return LCAcc::LCAccDevice::GetOpName(opCode);
}
void Interface_GetOpTimingHandler(int opcode, unsigned int* ii, unsigned int* pipeDepth, unsigned int* cycleMult)
{
	assert(ii);
	assert(pipeDepth);
	assert(cycleMult);
	LCAcc::LCAccDevice::GetOpTiming(opcode, *ii, *pipeDepth, *cycleMult);
}
void Interface_HookToMemoryDeviceHandler(LCAccDeviceHandle* handle, const char* deviceName)
{
	assert(handle);
	assert(deviceName);
	gem5Interface::manager.HookToMemoryDevice(handle->handleID, deviceName);
}
LCAccInterface* CreateLCAccInterface()
{
        snpi = g_networkPort_interface;

	LCAccInterface* interface = (LCAccInterface*)malloc(sizeof(LCAccInterface));
	interface->ProcessMessage = Interface_ProcessMessageHandler;
	interface->GetOpCode = Interface_GetOpCodeHandler;
	interface->GetOpName = Interface_GetOpNameHandler;
	interface->GetOpTiming = Interface_GetOpTimingHandler;
	interface->AddOperatingMode = Interface_AddOperatingModeHandler;
	interface->SetNetPort = Interface_SetNetPortHandler;
	interface->SetTiming = Interface_SetTimingHandler;
	interface->SetPrefetchDistance = Interface_SetPrefetchDistanceHandler;
	interface->SetSPMConfig = Interface_SetSPMConfigHandler;
	interface->Initialize = Interface_InitializeHandler;
	interface->IsIdle = Interface_IsIdleHandler;
	interface->HookToMemoryDevice = Interface_HookToMemoryDeviceHandler;

	return interface;
}
void SNPIRecvHandler(void*, int src, int dst, const char* buffer, int size)
{
	gem5Interface::HandleMessage(src, dst, (const void*) buffer, size);
}
static void ExecuteCB(void* cbArg)
{
	CallbackBase* cb = (CallbackBase*)cbArg;
	cb->Call();
	cb->Dispose();
}
bool gem5Interface::SendMessage(int source, int destination, unsigned int msgSize, const void* buffer)
{
	assert(snpi);
	snpi->SendMessageOnDevice(source, destination, (const char*)buffer, msgSize);
	return true;
}
void gem5Interface::RegisterCallback(CallbackBase* cb, int delay)
{
	assert(cb);
	assert(delay >= 0);
	scheduleCB(ExecuteCB, cb, delay);
}
unsigned long long gem5Interface::GetSystemTime()
{
        return (unsigned long long)uint64_t(g_system_ptr->curCycle());
}
void gem5Interface::ReadPhysical(uint64_t addr, void* buffer, size_t size)
{
	//TODO: Assumption: this is a functional (non-timing) read: therefore,
	//location the read is sent from does not matter.
	MemoryInterface::Instance()->functionalRead(addr, (uint8_t*) buffer, size);
}
void gem5Interface::WritePhysical(uint64_t addr, const void* buffer, size_t size)
{
	//TODO: Assumption: this is a functional (non-timing) write: therefore,
	//location the write is sent from does not matter.
	MemoryInterface::Instance()->functionalWrite(addr, (uint8_t*) buffer, size);
}
void gem5Interface::HandleMessage(int src, int dst, const void* buffer, int bufferSize)
{
	if(netHandlers.find(dst) != netHandlers.end())
	{
		for(size_t i = 0; i < netHandlers[dst].size(); i++)
		{
			netHandlers[dst][i]->Call(src,buffer,bufferSize);
		}
	}
}
void CallCBWrapper(void* args)
{
	CallbackBase* cb = (CallbackBase*)args;
	assert(cb);
	cb->Call();
	cb->Dispose();
}
void gem5Interface::TimedBufferRead(int cpu, uint64_t addr, size_t size, int buffer, CallbackBase* cb)
{
	MemoryInterface::Instance()->sendReadRequest(addr, (uint8_t*) &buffer, size, CallCBWrapper, (void *)cb);
}
void gem5Interface::TimedBufferWrite(int cpu, uint64_t addr, size_t size, int buffer, CallbackBase* cb)
{
	MemoryInterface::Instance()->sendWriteRequest(addr, (uint8_t*) &buffer, size, CallCBWrapper, (void *)cb);
}
void gem5Interface::RegisterLCAcc(int id, int node, std::vector<int> opmodes, uint32_t threadID)
{
	bool foundTD = false;

	//TODO: Currently only support 1 TD
	TDHandle* tdObject = g_TDHandle[0];
	if(tdObject)
	{
		foundTD = true;
		std::cout << "Registering lcacc " << id << " with TD" << std::endl;
		TaskDistributorInterface* td = g_TDInterface;
		if(threadID != 0)
		{
			td->AddCFUFilter(tdObject, threadID, id);
		}
		for(size_t i = 0; i < opmodes.size(); i++)
		{
			td->AddCFU(tdObject, id, node, LCAcc::LCAccDevice::GetOpName(opmodes[i]));
		}
	}
	assert(foundTD);
}
void gem5Interface::UnregisterLCAcc(int id)
{
	//TODO: Currently only support 1 TD
	TDHandle* tdObject = g_TDHandle[0];
	if(tdObject)
	{
	  TaskDistributorInterface* td = g_TDInterface;
	  assert(td);
	  td->RemoveCFU(tdObject, id);
	}
}

}
