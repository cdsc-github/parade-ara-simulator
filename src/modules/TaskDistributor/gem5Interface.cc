#include "gem5Interface.hh"
#include <vector>
#include <cstdlib>
#include <cassert>
#include <iostream>

#define SIM_ENHANCE
#define SIM_NET_PORTS
#define SIM_MEMORY
#include "modules/Common/mf_api.hh"
#include "mem/ruby/system/gem5NetworkPortInterface.hh"
#include "mem/ruby/common/Global.hh"
#include "mem/ruby/system/System.hh"
#include "../LCAcc/Interface.hh"
#include "Interface.hh"
#include "modules/LCAcc/memInterface.hh"
#include "TDManager.hh"

#define SP_WORD_SIZE 8
#define BLOCK_SIZE 64

using namespace TaskDistributor;

std::map<int, std::vector<Arg3CallbackBase<int, const void*, unsigned int>*> > TaskDistributor::gem5Interface::netHandlers;

namespace TaskDistributor
{
gem5NetworkPortInterface* snpi;
TDManager manager;
}
struct TDHandle {
  int value;
  int handleID;
};
class TDMessageInterface
{
public:
  void (*ProcessMessage)(void*, int, int, const char*, int);
};

/*Local function definitions: needed for compilation. */
void SetCFUAllocationPerIslandHandler(TDHandle* handle, int val);
void InitializeHandler(TDHandle* handle);
void ProcessMessageHandler(void* handle, int src, int destination, const char* msgBody, int msgLength);
void SetNetworkPortHandler(TDHandle* handle, int port, int device);
void SetTaskGrainHandler(TDHandle* handle, int val);
void AddCFUInterfaceHandler(TDHandle* handle, int id, int port, const char* opName);
void RemoveCFUInterfaceHandler(TDHandle* obj, int id);
void AddCFUFilterInterfaceHandler(TDHandle* obj, uint32_t threadID, int cfuID);
void ClearCFUFilterInterfaceHandler(TDHandle* obj, uint32_t threadID);
void SetSelectorHandler(TDHandle* handle, std::string val);
void SetFpgaAreaHandler(TDHandle* handle, int val);
void AddFpgaPortHandler(TDHandle* handle, int val);
namespace TaskDistributor
{
void SNPIRecvHandler(void*, int src, int dst, const char* buffer, int size);
}
void ExecuteCB(void* cbArg);
void gem5CBWrapper(void* args);
void AllocateCBWrapper(void* args, unsigned int success);

TDHandle* CreateNewTDHandle()
{
  static int id = 0;
  TDHandle *handle = (TDHandle *)malloc(sizeof(TDHandle));
  bzero(handle, sizeof(TDHandle));
  handle->handleID = id++;
  manager.RegisterNewDevice(handle->handleID);
  return handle;
}
void SetCFUAllocationPerIslandHandler(TDHandle* handle, int val)
{
  manager.SetCFUAllocationPerIsland((handle)->handleID, val);
}
void InitializeHandler(TDHandle* handle)
{
  manager.FinalizeDeviceCreation((handle)->handleID);
}
void ProcessMessageHandler(void* handle, int src, int destination, const char* msgBody, int msgLength)
{
  manager.ProcessMessage(src, destination, (unsigned int)msgLength, msgBody);
}
void SetNetworkPortHandler(TDHandle* handle, int port, int device)
{
  assert(snpi);
  assert(handle);
  snpi->BindDeviceToPort(port, device);
  std::cout << "setting TD with port " << port << " and device " << device << std::endl;
  snpi->RegisterRecvHandlerOnDevice(device, ProcessMessageHandler, handle);
  manager.AddNetworkPort((handle)->handleID, port, device);
}
void SetTaskGrainHandler(TDHandle* handle, int val)
{
  manager.SetTaskGrain(((TDHandle*)handle)->handleID, val);
}
void AddCFUInterfaceHandler(TDHandle* handle, int id, int port, const char* opName)
{
  assert(handle);
  assert(opName);
  LCAccInterface* li = g_LCAccInterface;
  assert(li);
  int opCode = li->GetOpCode(opName);
  manager.AddCFU(handle->handleID, id, port, opCode);
}
void ClearCFUFilterInterfaceHandler(TDHandle* obj, uint32_t threadID)
{
  assert(obj);
  manager.ClearCFUFilter(obj->handleID, threadID);
}
void AddCFUFilterInterfaceHandler(TDHandle* obj, uint32_t threadID, int cfuID)
{
  assert(obj);
  manager.AddCFUFilter(((TDHandle*)obj)->handleID, threadID, cfuID);
}
void RemoveCFUInterfaceHandler(TDHandle* obj, int id)
{
  assert(obj);
  manager.RemoveCFU(((TDHandle*)obj)->handleID, id);
}
void SetSelectorHandler(TDHandle* handle, std::string val)
{
  const char* type = val.c_str();
  manager.SetSelector(((TDHandle*)handle)->handleID, type);
}
void SetFpgaAreaHandler(TDHandle* handle, int val)
{
  manager.SetFpgaArea(handle->handleID, val);
}
void AddFpgaPortHandler(TDHandle* handle, int val)
{
  manager.AddFpgaPort((handle)->handleID, val);
}

TaskDistributorInterface* CreateTDInterface()
{
  TaskDistributorInterface* interface = (TaskDistributorInterface *)malloc(sizeof(TaskDistributorInterface));
  interface->SetCFUAllocationPerIsland = SetCFUAllocationPerIslandHandler;
  interface->Initialize = InitializeHandler;
  interface->SetNetworkPort = SetNetworkPortHandler;
  interface->SetTaskGrain = SetTaskGrainHandler;
  interface->AddCFU = AddCFUInterfaceHandler;
  interface->RemoveCFU = RemoveCFUInterfaceHandler;
  interface->AddCFUFilter = AddCFUFilterInterfaceHandler;
  interface->ClearCFUFilter = ClearCFUFilterInterfaceHandler;
  interface->SetSelector = SetSelectorHandler;
  interface->SetFpgaArea = SetFpgaAreaHandler;
  interface->AddFpgaPort = AddFpgaPortHandler;
  snpi = g_networkPort_interface;
  assert(snpi);
  return interface;
}
void TaskDistributor::SNPIRecvHandler(void*, int src, int dst, const char* buffer, int size)
{
  gem5Interface::HandleMessage(src, dst, (const void*) buffer, size);
}
void ExecuteCB(void* cbArg)
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
void gem5Interface::ReadPhysical(uint64_t addr, void* buffer, size_t size)
{
  //TODO: Assumption: this is a functional read, not a timing read,
  // to physical DRAM memory.  It does not matter where it was sent from.
  MemoryInterface::Instance()->functionalRead(addr, (uint8_t*)buffer, size);
}
void gem5Interface::WritePhysical(uint64_t addr, const void* buffer, size_t size)
{
  //TODO: Assumption: this is a functional write, not a timing write.
  // The location is physical DRAM memory, and packet path is unimportant.
  MemoryInterface::Instance()->functionalWrite(addr, (uint8_t*)buffer, size);
}
void gem5Interface::HandleMessage(int src, int dst, const void* buffer, int bufferSize)
{
  if (netHandlers.find(dst) != netHandlers.end()) {
    for (size_t i = 0; i < netHandlers[dst].size(); i++) {
      netHandlers[dst][i]->Call(src, buffer, bufferSize);
    }
  }
}
void gem5CBWrapper(void* args)
{
  CallbackBase* cb = (CallbackBase*)args;
  cb->Call();
  cb->Dispose();
}
void AllocateCBWrapper(void* args, unsigned int success)
{
  Arg1CallbackBase<bool>* cb = (Arg1CallbackBase<bool>*)args;
  cb->Call((success > 0) ? true : false);
  cb->Dispose();
}
int gem5Interface::GetBufferID(int core)
{
  fatal ("Buffer in cache not supported!\n");
  return 0;
}
void gem5Interface::FreeBuffer(int nodeID, int buffer, CallbackBase* cb)
{
  fatal ("Buffer in cache not supported!\n");
}
void gem5Interface::TryBufferAllocate(int nodeID, int buffer, unsigned int size, Arg1CallbackBase<bool>* cb)
{
  fatal ("Buffer in cache not supported!\n");
}
void gem5Interface::GetOpModeTiming(int opCode, unsigned int* ii, unsigned int* pipelineDepth, unsigned int* clockMult)
{
  LCAccInterface* interface = g_LCAccInterface;
  assert(interface);
  interface->GetOpTiming(opCode, ii, pipelineDepth, clockMult);
}
