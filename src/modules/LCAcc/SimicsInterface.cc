#include "SimicsInterface.hh"
#include <vector>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <strings.h>

//#define SIMICS30

//#include <simics/api.h>
//#include <simics/alloc.h>
//#include <simics/utils.h>
#include "Interface.hh"
#include "LCAccDevice.hh"
#define SIM_ENHANCE
#define SIM_NET_PORTS
#define SIM_MEMORY

//0. SW version
//#define SIM_SW

//1. ARC version
//#define SIM_ARC

//2. ARC PLUS version
#define SIM_ARC_PLUS

//3. CHARM NON-ISLAND version
//#define SIM_CHARM

//4. CHARM ISLAND version
//#define SIM_CHARM_ISLAND

//for compiling purpose
#if defined(SIM_SW)
#define SIM_ARC
#endif

#if defined(SIM_ARC_PLUS) || defined(SIM_CHARM) || defined(SIM_CHARM_ISLAND)
#define SIM_TD
#endif

//#define BUFFER_IN_CACHE
#include "../Common/mf_api.hh"
// #include "../GlobalAcceleratorManager/GAM.hh"
// #include "../TaskDistributor/Interface.hh"
#include "../../mem/ruby/system/SimicsNetworkPortInterface.hh"
#include "../NetworkInterrupt/NetworkInterrupts.hh"
//#include "../../mem/ruby/common/Global.hh"
#include "../../mem/ruby/system/System.hh"
#include "memInterface.hh"
#define SP_WORD_SIZE 8

using namespace LCAcc;

namespace LCAcc
{

LCAcc::LCAccManager SimicsInterface::manager;
std::map<int, std::vector<Arg3CallbackBase<int, const void*, unsigned int>*> > SimicsInterface::netHandlers;
static void ExecuteCB(void* cbArg);

/*typedef struct
{
	void (*start_compute)(conf_object_t *cpu, int opal_data_address, int lcaccID);
} lcacc_compute_interface_t; //*/

SimicsNetworkPortInterface* snpi;

/*mf_opal_api_t*  QueryOpalInterface()
{
	static mf_opal_api_t* opal_api = NULL;
	if(opal_api == NULL)
	{
		conf_object_t *opal;
		opal = SIM_get_object("opal0");
		assert(opal);
		opal_api = (mf_opal_api_t *) SIM_get_interface( opal, "mf-opal-api" );
		assert(opal_api);
	}
	return opal_api;
}
mf_ruby_api_t* QueryRubyInterface()
{
	static mf_ruby_api_t* rubyApi = NULL;
	if(rubyApi == NULL)
	{
		conf_object_t* ruby;
		ruby = SIM_get_object("ruby0");
		assert(ruby);
		rubyApi = (mf_ruby_api_t*)SIM_get_interface(ruby, "mf-ruby-api");
		assert(rubyApi);
	}
	return rubyApi;
}//*/

//static conf_object_t* CreateNewDeviceHandle(parse_object_t* po)
LCAccDeviceHandle* CreateNewLCAccDeviceHandle()
{
  static int id = 0;
  LCAccDeviceHandle* handle = (LCAccDeviceHandle *)malloc(sizeof(LCAccDeviceHandle)); //MM_ZALLOC(1, LCAccDeviceHandle);
  bzero(handle, sizeof(LCAccDeviceHandle));
  assert(handle);
  handle->handleID = id++;
  //SIM_object_constructor(&handle->base, po);
  SimicsInterface::manager.RegisterDevice(handle->handleID);
  //return &(handle->base);
  return handle;
}
//static int DeleteDeviceHandle(conf_object_t* obj)
int DeleteLCAccDeviceHandle(LCAccDeviceHandle* handle)
{
  assert(handle);
  SimicsInterface::manager.UnregisterDevice(handle->handleID);
  snpi->UnregisterRecvHandler(handle->netHandle);
  snpi->UnbindDevice(handle->deviceID);
  return 0;
}
//void Interface_AddOperatingModeHandler(conf_object_t* handle, const char* name)
void Interface_AddOperatingModeHandler(LCAccDeviceHandle* handle, const char* name)
{
  assert(handle);
  SimicsInterface::manager.AddOperatingMode(handle->handleID, name);
}
//void Interface_SetNePortHandler(conf_object_t* handle, int port, int device)
void Interface_SetNetPortHandler(LCAccDeviceHandle* handle, int port, int device)
{
  assert(snpi);
  assert(handle);
  handle->deviceID = device;
  snpi->BindDeviceToPort(port, device);
  // warn("bind accelerator device %d to port %d\n", device, port);
  handle->netHandle = snpi->RegisterRecvHandlerOnDevice(device, Interface_ProcessMessageHandler, handle);
  SimicsInterface::manager.AddNetworkPort(handle->handleID, port, device);
}
//void Interface_SetTimingHandler(conf_object_t* handle, unsigned int cycleTime, unsigned int pipelineDepth, unsigned int initiationInterval)
void Interface_SetTimingHandler(LCAccDeviceHandle* handle, unsigned int cycleTime, unsigned int pipelineDepth, unsigned int initiationInterval)
{
  assert(handle);
  SimicsInterface::manager.SetTiming(handle->handleID, initiationInterval, pipelineDepth, cycleTime);
}
//void Interface_SetPrefetchDistanceHandler(conf_object_t* handle, int distance)
void Interface_SetPrefetchDistanceHandler(LCAccDeviceHandle* handle, int distance)
{
  assert(handle);
  SimicsInterface::manager.SetPrefetchDistance(handle->handleID, distance);
}
//void Interface_SetSMConfigHandler(conf_object_t* handle, int banks, int bankMod, int readPorts, int readLatency, int writePorts, int writeLatency)
void Interface_SetSPMConfigHandler(LCAccDeviceHandle* handle, int banks, int bankMod, int readPorts, int readLatency, int writePorts, int writeLatency)
{
  assert(handle);
  assert(readPorts > 0 && writePorts > 0 && readLatency >= 0 && writeLatency >= 0);
  assert(bankMod > 0);
  assert(banks > 0);
  SimicsInterface::manager.SetSPMConfig(handle->handleID, banks, bankMod, readPorts, readLatency, writePorts, writeLatency);
}

//void Interface_InitializeHandler(conf_object_t* handle, uint32_t thread)
void Interface_InitializeHandler(LCAccDeviceHandle* handle, uint32_t thread)
{
  assert(handle);
  SimicsInterface::manager.FinalizeDeviceCreation(handle->handleID, thread);
}
//bool Interface_IsIdleHandler(conf_object_t* handle)
bool Interface_IsIdleHandler(LCAccDeviceHandle* handle)
{
  assert(handle);
  return SimicsInterface::manager.IsIdle(handle->handleID);
}
/*attr_value_t AddOperatingModeHandler(void*, conf_object_t* handle, attr_value_t* val)
{
	std::string name;
	assert(val->kind == Sim_Val_List);
	assert(val->u.list.size == 1);
	val = val->u.list.vector;
	assert(val->kind == Sim_Val_String);
	name = val->u.string;
	Interface_AddOperatingModeHandler(handle, name.c_str());
	return SIM_make_attr_string("Success\n");
}
attr_value_t InitializeHandler(void*, conf_object_t* handle, attr_value_t* val)
{
	uint32_t threadID = 0;
	if(val->kind == Sim_Val_List && val->u.list.size == 1 && val->u.list.vector[0].kind == Sim_Val_Integer)
	{
		threadID = val->u.list.vector[0].u.integer;
	}
	Interface_InitializeHandler(handle, threadID);
	return SIM_make_attr_string("Success\n");
}//*/
void Interface_ProcessMessageHandler(void* handle, int src, int destination, const char* msgBody, int msgLength)
{
  SimicsInterface::manager.ProcessMessage(src, destination, (unsigned int)msgLength, msgBody);
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
//void Interface_HookToMemoryDeviceHandler(conf_object_t* handle, const char* deviceName)
void Interface_HookToMemoryDeviceHandler(LCAccDeviceHandle* handle, const char* deviceName)
{
  assert(handle);
  assert(deviceName);
  SimicsInterface::manager.HookToMemoryDevice(handle->handleID, deviceName);
}
/*attr_value_t SetNetworkPortHandler(void*, conf_object_t* handle, attr_value_t* val)
{
	assert(snpi);
	assert(val->kind == Sim_Val_List);
	assert(val->u.list.size == 2);
	val = val->u.list.vector;
	assert(val[0].kind == Sim_Val_Integer);
	assert(val[1].kind == Sim_Val_Integer);
	int port = val[0].u.integer;
	int device = val[1].u.integer;
	Interface_SetNetPortHandler(handle, port, device);
        return SIM_make_attr_string("Success\n");
}
attr_value_t SetTiming(void*, conf_object_t* handle, attr_value_t* val)
{
	assert(val->kind == Sim_Val_List);
	int CycleTime          ;
        int PipelineDepth      ;
        int InitiationInterval ;

	if (val->u.list.size == 3)
	{
		val = val->u.list.vector;
		assert(val[0].kind == Sim_Val_Integer);
		assert(val[1].kind == Sim_Val_Integer);
		assert(val[2].kind == Sim_Val_Integer);
		CycleTime          = val[0].u.integer;
		PipelineDepth      = val[1].u.integer;
		InitiationInterval = val[2].u.integer;
	}
	else
	{
	        CycleTime          = 1;
	        PipelineDepth      = 1;
        	InitiationInterval = 1;
	}
	Interface_SetTimingHandler(handle, InitiationInterval, PipelineDepth, CycleTime);
	return SIM_make_attr_string("Success\n");
}
attr_value_t SetPrefetchDistanceHandler(void*, conf_object_t* handle, attr_value_t* val)
{
	assert(val->kind == Sim_Val_List);
	assert(val->u.list.size == 1);
	val = val->u.list.vector;
	assert(val->kind == Sim_Val_Integer);
	int dist = val[0].u.integer;
	Interface_SetPrefetchDistanceHandler(handle, dist);
	return SIM_make_attr_string("Success\n");
}
attr_value_t SetSPMConfigHandler(void*, conf_object_t* handle, attr_value_t* val)
{
	assert(handle);
	assert(val->kind == Sim_Val_List);
	assert(val->u.list.size == 6);
	val = val->u.list.vector;
	assert(val[0].kind == Sim_Val_Integer);
	assert(val[1].kind == Sim_Val_Integer);
	assert(val[2].kind == Sim_Val_Integer);
	assert(val[3].kind == Sim_Val_Integer);
	assert(val[4].kind == Sim_Val_Integer);
	assert(val[5].kind == Sim_Val_Integer);
	Interface_SetSPMConfigHandler(handle, val[0].u.integer, val[1].u.integer, val[2].u.integer, val[3].u.integer, val[4].u.integer, val[5].u.integer);
	return SIM_make_attr_string("Success\n");
}
attr_value_t HookToMemoryDeviceHandler(void*, conf_object_t* handle, attr_value_t* val)
{
	assert(handle);
	assert(val->kind == Sim_Val_List);
	assert(val->u.list.size == 1);
	val = val->u.list.vector;
	assert(val[0].kind == Sim_Val_String);
	Interface_HookToMemoryDeviceHandler(handle, val[0].u.string);
	return SIM_make_attr_string("Success\n");
}

#if defined(__cplusplus)
extern "C" {
#endif
void init_local()
{
	class_data_t funcs;
	conf_class_t *lcaccClass;
	LCAccInterface* interface1;

	memset(&funcs, 0, sizeof(class_data_t));
	funcs.new_instance = CreateNewDeviceHandle;
	funcs.delete_instance = DeleteDeviceHandle;

	funcs.description = "A basic LCAcc device node";

	lcaccClass = SIM_register_class("LCAcc", &funcs);
	interface1 = MM_ZALLOC(1, LCAccInterface);
	interface1->ProcessMessage = Interface_ProcessMessageHandler;
	interface1->GetOpCode = Interface_GetOpCodeHandler;
	interface1->GetOpName = Interface_GetOpNameHandler;
	interface1->GetOpTiming = Interface_GetOpTimingHandler;
	interface1->AddOperatingMode = Interface_AddOperatingModeHandler;
	interface1->SetNetPort = Interface_SetNetPortHandler;
	interface1->SetTiming = Interface_SetTimingHandler;
	interface1->SetPrefetchDistance = Interface_SetPrefetchDistanceHandler;
	interface1->SetSPMConfig = Interface_SetSPMConfigHandler;
	interface1->Initialize = Interface_InitializeHandler;
	interface1->IsIdle = Interface_IsIdleHandler;
	interface1->HookToMemoryDevice = Interface_HookToMemoryDeviceHandler;

	SIM_register_interface(lcaccClass, "LCAccInterface", interface1);
	SIM_register_typed_attribute(lcaccClass, "AddOperatingMode", AddOperatingModeHandler, NULL, NULL, NULL, Sim_Attr_Session, "n", NULL, "Add a LCAcc operating mode.");
	SIM_register_typed_attribute(lcaccClass, "Initialize", InitializeHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Initialize the object after all elements have been attached.");
	SIM_register_typed_attribute(lcaccClass, "SetNetPort", SetNetworkPortHandler, NULL, NULL, NULL, Sim_Attr_Session, "ii", NULL, "Add a network port destination to the LCAcc");
	SIM_register_typed_attribute(lcaccClass, "SetTiming", SetTiming, NULL, NULL, NULL, Sim_Attr_Session, "iii", NULL, "SetTiming");
	SIM_register_typed_attribute(lcaccClass, "SetPrefetchDistance", SetPrefetchDistanceHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Sets the distance ahead of the current access to prefetch.  Zero means no prefetching");
	SIM_register_typed_attribute(lcaccClass, "SetSPMConfig", SetSPMConfigHandler, NULL, NULL, NULL, Sim_Attr_Session, "iiiiii", NULL, "Sets the SPM configuration");
	SIM_register_typed_attribute(lcaccClass, "HookToMemoryDevice", HookToMemoryDeviceHandler, NULL, NULL, NULL, Sim_Attr_Session, "s", NULL, "Hook to a memory device");
	snpi = (SimicsNetworkPortInterface*)SIM_get_interface(SIM_get_object("ruby0"), "network-ports");
	assert(snpi);
}
#if defined(__cplusplus)
}
#endif
//*/
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
  SimicsInterface::HandleMessage(src, dst, (const void*) buffer, size);
}
static void ExecuteCB(void* cbArg)
{
  CallbackBase* cb = (CallbackBase*)cbArg;
  cb->Call();
  cb->Dispose();
}
bool SimicsInterface::SendMessage(int source, int destination, unsigned int msgSize, const void* buffer)
{
  assert(snpi);
  snpi->SendMessageOnDevice(source, destination, (const char*)buffer, msgSize);
  return true;
}
void SimicsInterface::RegisterCallback(CallbackBase* cb, int delay)
{
  assert(cb);
  assert(delay >= 0);
  //QueryOpalInterface()->scheduleCB(ExecuteCB, cb, delay);
  scheduleCB(ExecuteCB, cb, delay);
}
unsigned long long SimicsInterface::GetSystemTime()
{
  //return QueryOpalInterface()->getOpalTime(0);
  return (unsigned long long)uint64_t(g_system_ptr->curCycle());
}
void SimicsInterface::ReadPhysical(uint64_t addr, void* buffer, size_t size)
{
  //TODO: Assumption: this is a functional (non-timing) read: therefore,
  //location the read is sent from does not matter.
  MemoryInterface::Instance()->functionalRead(addr, (uint8_t*) buffer, size);
  /*uinteger_t x;
  for(size_t i = 0; i < size; i++)
  {
  	//std::cout << "SIM_read_phys_memory " << addr + i << std::endl;
  	x = SIM_read_phys_memory(SIM_get_processor(0), addr + i, 1);
  	((uint8_t*)buffer)[i] = (uint8_t)x;
  }*/
}
void SimicsInterface::WritePhysical(uint64_t addr, const void* buffer, size_t size)
{
  //TODO: Assumption: this is a functional (non-timing) write: therefore,
  //location the write is sent from does not matter.
  MemoryInterface::Instance()->functionalWrite(addr, (uint8_t*) buffer, size);
  /*uinteger_t x;
  for(size_t i = 0; i < size; i++)
  {
  	x = ((const uint8_t*)buffer)[i];
  	//std::cout << "SIM_write_phys_memory " << addr + i << std::endl;
  	SIM_write_phys_memory(SIM_get_processor(0), addr + i, x, 1);
  }*/
}
void SimicsInterface::HandleMessage(int src, int dst, const void* buffer, int bufferSize)
{
  if (netHandlers.find(dst) != netHandlers.end()) {
    for (size_t i = 0; i < netHandlers[dst].size(); i++) {
      netHandlers[dst][i]->Call(src, buffer, bufferSize);
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
void SimicsInterface::TimedBufferRead(int cpu, uint64_t addr, size_t size, int buffer, CallbackBase* cb)
{
  //TODO: Not accurate for multiple accelerators:
  // Currently, all requests are sent from a single accelerator.
  // One way to fix is to make a mapping from cpu to memory interface,
  // if there is at most one accelerator per cpu.
  MemoryInterface::Instance()->sendReadRequest(addr, (uint8_t*) &buffer, size, CallCBWrapper, (void *)cb);
  //SimicsInterface::makeBufferRequestCB(cpu, addr, buffer, RubyRequestType_LD, CallCBWrapper, cb);
}
void SimicsInterface::TimedBufferWrite(int cpu, uint64_t addr, size_t size, int buffer, CallbackBase* cb)
{
  //TODO: Not accurate for multiple accelerators:
  // Currently, all requests are sent from a single accelerator.
  // One way to fix is to make a mapping from cpu to memory interface,
  // if there is at most one accelerator per cpu.
  MemoryInterface::Instance()->sendWriteRequest(addr, (uint8_t*) &buffer, size, CallCBWrapper, (void *)cb);
  //SimicsInterface::makeBufferRequestCB(cpu, addr, buffer, RubyRequestType_ST, CallCBWrapper, cb);
}

/*void SimicsInterface::makeBufferRequestCB(int cpuNumber, uint64_t bufferBlockAddr, int bufferID, RubyRequestType typeOfRequest, void(*userCB)(void*), void* userArgs)// spm$ <-> arg buffer
{
  Sequencer* targetSequencer_ptr = ;
  targetSequencer_ptr->makeBufferRequest(bufferBlockAddr, bufferID, typeOfRequest, userCB, userArgs);
}*/

/*bool SimicsInterface::CreateSPM(int id)
{
	char s[100];
	sprintf(s, "scratch-pad%d", id);
	conf_class_t* c = SIM_get_class("scratch-pad");
	assert(c);
	assert(SIM_get_object(s) == NULL);
	conf_object_t* x = SIM_create_object(c, s, SIM_make_attr_list(0));
	assert(x);
	return true;
}
bool SimicsInterface::CreateDMA(int id)
{
        char s[100];
        sprintf(s, "prefetcher%d", id);
	conf_class_t* c = SIM_get_class("prefetcher-tile");
        assert(c);
	assert(SIM_get_object(s) == NULL);
        conf_object_t* x = SIM_create_object(c, s, SIM_make_attr_list(0));
	assert(x);
	return true;
}
void SimicsInterface::DeleteSPM(int id)
{
	char s[100];
	sprintf(s, "scratch-pad%d", id);
	conf_object_t* x = SIM_get_object(s);
	assert(x);
	SIM_delete_object(x);
}
void SimicsInterface::DeleteDMA(int id)
{
	char s[100];
	sprintf(s, "prefetcher%d", id);
	conf_object_t* x = SIM_get_object(s);
	assert(x);
	SIM_delete_object(x);
} //*/
void SimicsInterface::RegisterLCAcc(int id, int node, std::vector<int> opmodes, uint32_t threadID)
{
  bool foundGam = false;
  bool foundTD = false;

#ifdef SIM_TD
  /*SIM_clear_exception();
    conf_object_t* tdObject = SIM_get_object("td0");//*/
  //TODO: Currently only support 1 TD
  TDHandle* tdObject = g_TDHandle[0];

  if (tdObject) {
    foundTD = true;
    // std::cout << "Registering lcacc " << id << " with TD" << std::endl;
    /*TaskDistributorInterface* td = (TaskDistributorInterface*)SIM_get_interface(tdObject, "TaskDistributorInterface");
      assert(td);//*/
    TaskDistributorInterface* td = g_TDInterface;

    if (threadID != 0) {
      td->AddCFUFilter(tdObject, threadID, id);
    }

    for (size_t i = 0; i < opmodes.size(); i++) {
      td->AddCFU(tdObject, id, node, LCAcc::LCAccDevice::GetOpName(opmodes[i]));
    }
  }

#endif
  //SIM_clear_exception();
  assert(foundGam || foundTD);
  assert(!foundGam || !foundTD);
}
void SimicsInterface::UnregisterLCAcc(int id)
{
  //TODO: Currently only support 1 TD
  TDHandle* tdObject = g_TDHandle[0];

  if (tdObject) {
    TaskDistributorInterface* td = g_TDInterface;
    assert(td);
    td->RemoveCFU(tdObject, id);
  }
}

}
