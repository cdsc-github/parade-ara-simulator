#include "SimicsInterface.hh"
#include <vector>
#include <cstdlib>
#include <cassert>
#include <iostream>

//#define SIMICS30

/*#include <simics/api.h>
#include <simics/alloc.h>
#include <simics/utils.h>*/
#define SIM_ENHANCE
#define SIM_NET_PORTS
#define SIM_MEMORY
//#define BUFFER_IN_CACHE
#include "modules/Common/mf_api.hh"
// #include "../GlobalAcceleratorManager/GAM.hh"
#include "mem/ruby/system/SimicsNetworkPortInterface.hh"
#include "mem/ruby/common/Global.hh"
#include "mem/ruby/system/System.hh"
#include "../LCAcc/Interface.hh"
#include "Interface.hh"
//#include "lcacc/lcacc_interface.hh"
#include "modules/LCAcc/memInterface.hh"
#include "TDManager.hh"

#define SP_WORD_SIZE 8
#define BLOCK_SIZE 64

using namespace TaskDistributor;

std::map<int, std::vector<Arg3CallbackBase<int, const void*, unsigned int>*> > TaskDistributor::SimicsInterface::netHandlers;

namespace TaskDistributor
{
SimicsNetworkPortInterface* snpi;
TDManager SimicsInterface::manager;
}

/*mf_opal_api_t*  QueryOpalInterface()
{
	static mf_opal_api_t* opal_api = NULL;
	if(opal_api == NULL)
	{
		conf_object_t *opal;
		opal = SIM_get_object("opal0");
		opal_api = (mf_opal_api_t *) SIM_get_interface( opal, "mf-opal-api" );
		assert(opal_api);
	}
	return opal_api;
}*/
/*mf_ruby_api_t* QueryRubyInterface()
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
}*/
struct TDHandle {
  //conf_object_t base;
  int value;
  int handleID;
};
class TDMessageInterface
{
public:
  void (*ProcessMessage)(void*, int, int, const char*, int);
};

/*Local function definitions: needed for compilation. */
//void AddOperatingModeHandler(TDHandle*, std::string* val);
void SetCFUAllocationPerIslandHandler(TDHandle* handle, int val);
void InitializeHandler(TDHandle* handle);
void ProcessMessageHandler(void* handle, int src, int destination, const char* msgBody, int msgLength);
void SetNetworkPortHandler(TDHandle* handle, int port, int device);
void SetTaskGrainHandler(TDHandle* handle, int val);
void AddCFUInterfaceHandler(TDHandle* handle, int id, int port, const char* opName);
void RemoveCFUInterfaceHandler(TDHandle* obj, int id);
void AddCFUFilterInterfaceHandler(TDHandle* obj, uint32_t threadID, int cfuID);
void ClearCFUFilterInterfaceHandler(TDHandle* obj, uint32_t threadID);
//void AddCFUHandler(TDHandle* handle, void* val);
void SetSelectorHandler(TDHandle* handle, std::string val);
void SetFpgaAreaHandler(TDHandle* handle, int val);
void AddFpgaPortHandler(TDHandle* handle, int val);
namespace TaskDistributor
{
void SNPIRecvHandler(void*, int src, int dst, const char* buffer, int size);
}
void ExecuteCB(void* cbArg);
void SimicsCBWrapper(void* args);
void AllocateCBWrapper(void* args, unsigned int success);

//static conf_object_t* CreateNewDeviceHandle(parse_object_t* po)
TDHandle* CreateNewTDHandle()
{
  static int id = 0;
  TDHandle *handle = (TDHandle *)malloc(sizeof(TDHandle));
  bzero(handle, sizeof(TDHandle));
  handle->handleID = id++;
  /*SIM_object_constructor(&handle->base, po);
  conf_object_t* dummyLca = SIM_get_object("dummyLCAccObject");
  if(dummyLca == NULL)
  {
  	SIM_clear_exception();
  	conf_class_t* objClass = SIM_get_class("LCAcc");
  	assert(objClass);
  	dummyLca = SIM_create_object(objClass, "dummyLCAccObject", SIM_make_attr_list(0));
  	assert(dummyLca);
  }//*/
  SimicsInterface::manager.RegisterNewDevice(handle->handleID);
  return handle;
  //return &(handle->base);
}
//attr_value_t AddOperatingModeHandler(void*, conf_object_t* handle, attr_value_t* val)
/*void AddOperatingModeHandler(TDHandle*, std::string* val)
{
	//TODO: Does this function do anything?  Seems to assign val
	//to a local variable, but does nothing else.
	std::string name;
	assert(val);
	//assert(val->kind == Sim_Val_List);
	//assert(val->u.list.size == 1);
	//val = val->u.list.vector;
	//assert(val->kind == Sim_Val_String);
	//name = val->u.string;
	name = *val;
	//return SIM_make_attr_string("Success\n");
}//*/
//attr_value_t SetCFUAllocationPerIslandHandler(void*, conf_object_t* handle, attr_value_t* val)
void SetCFUAllocationPerIslandHandler(TDHandle* handle, int val)
{
  //assert(val->kind == Sim_Val_List);
  //assert(val->u.list.size == 1);
  //val = val->u.list.vector;
  //assert(val->kind == Sim_Val_Integer);
  //manager.SetCFUAllocationPerIsland(((TDHandle*)handle)->handleID, val->u.integer);
  SimicsInterface::manager.SetCFUAllocationPerIsland((handle)->handleID, val);
  //return SIM_make_attr_string("Success\n");
}
//attr_value_t InitializeHandler(void*, conf_object_t* handle, attr_value_t* val)
void InitializeHandler(TDHandle* handle)
{
  SimicsInterface::manager.FinalizeDeviceCreation((handle)->handleID);
  //return SIM_make_attr_string("Success\n");
}
void ProcessMessageHandler(void* handle, int src, int destination, const char* msgBody, int msgLength)
{
  SimicsInterface::manager.ProcessMessage(src, destination, (unsigned int)msgLength, msgBody);
}
//attr_value_t SetNetworkPortHandler(void*, conf_object_t* handle, attr_value_t* val)
void SetNetworkPortHandler(TDHandle* handle, int port, int device)
{
  assert(snpi);
  assert(handle);
  snpi->BindDeviceToPort(port, device);
  std::cout << "setting TD with port " << port << " and device " << device << std::endl;
  snpi->RegisterRecvHandlerOnDevice(device, ProcessMessageHandler, handle);
  SimicsInterface::manager.AddNetworkPort((handle)->handleID, port, device);
}
//attr_value_t SetTaskGrainHandler(void*, conf_object_t* handle, attr_value_t* val)
void SetTaskGrainHandler(TDHandle* handle, int val)
{
  //assert(val->kind == Sim_Val_List);
  //assert(val->u.list.size == 1);
  //val = val->u.list.vector;
  //assert(val[0].kind == Sim_Val_Integer);
  //int grain = val[0].u.integer;
  SimicsInterface::manager.SetTaskGrain(((TDHandle*)handle)->handleID, val);
  //return SIM_make_attr_string("Success\n");
}
//void AddCFUInterfaceHandler(conf_object_t* obj, int id, int port, const char* opName)
void AddCFUInterfaceHandler(TDHandle* handle, int id, int port, const char* opName)
{
  assert(handle);
  assert(opName);
  /*conf_object_t* baseAccel = SIM_get_object("dummyLCAccObject");
  assert(baseAccel);
  LCAccInterface* li = (LCAccInterface*)SIM_get_interface(baseAccel, "LCAccInterface");//*/
  LCAccInterface* li = g_LCAccInterface;
  assert(li);
  int opCode = li->GetOpCode(opName);
  SimicsInterface::manager.AddCFU(handle->handleID, id, port, opCode);
}
//void ClearCFUFilterInterfaceHandler(conf_object_t* obj, uint32_t threadID)
void ClearCFUFilterInterfaceHandler(TDHandle* obj, uint32_t threadID)
{
  assert(obj);
  SimicsInterface::manager.ClearCFUFilter(obj->handleID, threadID);
}
//void AddCFUFilterInterfaceHandler(conf_object_t* obj, uint32_t threadID, int cfuID)
void AddCFUFilterInterfaceHandler(TDHandle* obj, uint32_t threadID, int cfuID)
{
  assert(obj);
  SimicsInterface::manager.AddCFUFilter(((TDHandle*)obj)->handleID, threadID, cfuID);
}
//void RemoveCFUInterfaceHandler(conf_object_t* obj, int id)
void RemoveCFUInterfaceHandler(TDHandle* obj, int id)
{
  assert(obj);
  SimicsInterface::manager.RemoveCFU(((TDHandle*)obj)->handleID, id);
}
//attr_value_t AddCFUHandler(void*, conf_object_t* handle, attr_value_t* val)
/*void AddCFUHandler(void*, void* handle, void* val)
{
std::cout << "\"AddCFU\" has been depricated.  This functionality is now performed automatically when an accelerator is created." << std::endl;
	assert(val->kind == Sim_Val_List);
	assert(val->u.list.size == 3);
	val = val->u.list.vector;
	assert(val[0].kind == Sim_Val_Integer);
	assert(val[1].kind == Sim_Val_Integer);
	assert(val[2].kind == Sim_Val_String);
	int deviceID = val[0].u.integer;
	int cpu = val[1].u.integer;
	const char* type = val[2].u.string;
	AddCFUInterfaceHandler(handle, deviceID, cpu, type);
	//return SIM_make_attr_string("Success\n");
}//*/
//attr_value_t SetSelectorHandler(void*, conf_object_t* handle, attr_value_t* val)
void SetSelectorHandler(TDHandle* handle, std::string val)
{
  //assert(val);
  //assert(val->kind == Sim_Val_List);
  //assert(val->u.list.size == 1);
  //val = val->u.list.vector;
  //assert(val[0].kind == Sim_Val_String);
  //const char* type = val[0].u.string;
  const char* type = val.c_str();
  SimicsInterface::manager.SetSelector(((TDHandle*)handle)->handleID, type);
  //return SIM_make_attr_string("Success\n");
}
//attr_value_t SetFpgaAreaHandler(void*, conf_object_t* handle, attr_value_t* val)
void SetFpgaAreaHandler(TDHandle* handle, int val)
{
  //assert(val->kind == Sim_Val_List);
  //assert(val->u.list.size == 1);
  //val = val->u.list.vector;
  //assert(val[0].kind == Sim_Val_Integer);
  //manager.SetFpgaArea(((TDHandle*)handle)->handleID, val[0].u.integer);
  SimicsInterface::manager.SetFpgaArea(handle->handleID, val);
  //return SIM_make_attr_string("Success\n");
}
//attr_value_t AddFpgaPortHandler(void*, conf_object_t* handle, attr_value_t* val)
void AddFpgaPortHandler(TDHandle* handle, int val)
{
  //assert(val->kind == Sim_Val_List);
  //assert(val->u.list.size == 1);
  //val = val->u.list.vector;
  //assert(val[0].kind == Sim_Val_Integer);
  //manager.AddFpgaPort(((TDHandle*)handle)->handleID, val[0].u.integer);
  SimicsInterface::manager.AddFpgaPort((handle)->handleID, val);
  //return SIM_make_attr_string("Success\n");
  //return true;
}
/*attr_value_t AddFpgaRecipeHandler(void*, conf_object_t* handle, attr_value_t* val)
{
        assert(val->kind == Sim_Val_List);
        assert(val->u.list.size == 5);
        val = val->u.list.vector;
        assert(val[0].kind == Sim_Val_String);
	assert(val[1].kind == Sim_Val_Integer);
	assert(val[2].kind == Sim_Val_Integer);
	assert(val[3].kind == Sim_Val_Integer);
	assert(val[4].kind == Sim_Val_Integer);
	conf_object_t* baseAccel = SIM_get_object("dummyLCAccObject");
	assert(baseAccel);
	LCAccInterface* li = (LCAccInterface*)SIM_get_interface(baseAccel, "LCAccInterface");
	assert(li);
	int opcode = li->GetOpCode(val[0].u.string);
	manager.AddFpgaRecipe(((TDHandle*)handle)->handleID, opcode, val[1].u.integer, val[2].u.integer, val[3].u.integer, val[4].u.integer);
        return SIM_make_attr_string("Success\n");
}*/
/*#if defined(__cplusplus)
extern "C" {
#endif
void init_local()
{
	class_data_t funcs;
	conf_class_t *tdClass;
	memset(&funcs, 0, sizeof(class_data_t));
	funcs.new_instance = CreateNewDeviceHandle;
	funcs.description = "A task distributor";

	tdClass = SIM_register_class("TaskDistributor", &funcs);
	TaskDistributorInterface* interface = MM_ZALLOC(1, TaskDistributorInterface);
	interface->AddCFU = AddCFUInterfaceHandler;
	interface->ClearCFUFilter = ClearCFUFilterInterfaceHandler;
	interface->AddCFUFilter = AddCFUFilterInterfaceHandler;
	interface->RemoveCFU = RemoveCFUInterfaceHandler;

	SIM_register_interface(tdClass, "TaskDistributorInterface", interface);
	SIM_register_typed_attribute(tdClass, "Initialize", InitializeHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Initialize the object after all elements have been attached.");
	SIM_register_typed_attribute(tdClass, "SetNetPort", SetNetworkPortHandler, NULL, NULL, NULL, Sim_Attr_Session, "ii", NULL, "Add a network port destination to the TD");
	SIM_register_typed_attribute(tdClass, "AddCFU", AddCFUHandler, NULL, NULL, NULL, Sim_Attr_Session, "iin", NULL, "Add a cfu to be tracked by this TD");
	SIM_register_typed_attribute(tdClass, "SetTaskGrain", SetTaskGrainHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Set the number of tasks per sequence to be issued by this TD");
	SIM_register_typed_attribute(tdClass, "SetSelector", SetSelectorHandler, NULL, NULL, NULL, Sim_Attr_Session, "n", NULL, "Set the pattern selector that will be used.");
	SIM_register_typed_attribute(tdClass, "SetCFUAllocationPerIsland", SetCFUAllocationPerIslandHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Sets the number of CFUs that can be allocated on a single island.");
	SIM_register_typed_attribute(tdClass, "SetFpgaArea", SetFpgaAreaHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Sets the fpga area");
	SIM_register_typed_attribute(tdClass, "AddFpgaPort", AddFpgaPortHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Adds a port connected to the chips FPGA");
	SIM_register_typed_attribute(tdClass, "AddFpgaRecipe", AddFpgaRecipeHandler, NULL, NULL, NULL, Sim_Attr_Session, "niiii", NULL, "Adds an FPGA recipe");
	snpi = (SimicsNetworkPortInterface*)SIM_get_interface(SIM_get_object("ruby0"), "network-ports");
	assert(snpi);
}
#if defined(__cplusplus)
}
#endif*/

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
  /*SIM_register_interface(tdClass, "TaskDistributorInterface", interface);
  SIM_register_typed_attribute(tdClass, "Initialize", InitializeHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Initialize the object after all elements have been attached.");
  SIM_register_typed_attribute(tdClass, "SetNetPort", SetNetworkPortHandler, NULL, NULL, NULL, Sim_Attr_Session, "ii", NULL, "Add a network port destination to the TD");
  SIM_register_typed_attribute(tdClass, "AddCFU", AddCFUHandler, NULL, NULL, NULL, Sim_Attr_Session, "iin", NULL, "Add a cfu to be tracked by this TD");
  SIM_register_typed_attribute(tdClass, "SetTaskGrain", SetTaskGrainHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Set the number of tasks per sequence to be issued by this TD");
  SIM_register_typed_attribute(tdClass, "SetSelector", SetSelectorHandler, NULL, NULL, NULL, Sim_Attr_Session, "n", NULL, "Set the pattern selector that will be used.");
  SIM_register_typed_attribute(tdClass, "SetCFUAllocationPerIsland", SetCFUAllocationPerIslandHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Sets the number of CFUs that can be allocated on a single island.");
  SIM_register_typed_attribute(tdClass, "SetFpgaArea", SetFpgaAreaHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Sets the fpga area");
  SIM_register_typed_attribute(tdClass, "AddFpgaPort", AddFpgaPortHandler, NULL, NULL, NULL, Sim_Attr_Session, "i", NULL, "Adds a port connected to the chips FPGA");
  SIM_register_typed_attribute(tdClass, "AddFpgaRecipe", AddFpgaRecipeHandler, NULL, NULL, NULL, Sim_Attr_Session, "niiii", NULL, "Adds an FPGA recipe");
  snpi = (SimicsNetworkPortInterface*)SIM_get_interface(SIM_get_object("ruby0"), "network-ports");//*/
}
void TaskDistributor::SNPIRecvHandler(void*, int src, int dst, const char* buffer, int size)
{
  SimicsInterface::HandleMessage(src, dst, (const void*) buffer, size);
}
void ExecuteCB(void* cbArg)
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
/*unsigned long long SimicsInterface::GetSystemTime()
{
	//return QueryOpalInterface()->getOpalTime(0);
        return (unsigned long long)uint64_t(g_system_ptr->curCycle());
}//*/
void SimicsInterface::ReadPhysical(uint64_t addr, void* buffer, size_t size)
{
  /*uinteger_t x;
  for(size_t i = 0; i < size; i++)
  {
  	x = SIM_read_phys_memory(SIM_get_processor(0), addr + i, 1);
  	((uint8_t*)buffer)[i] = (uint8_t)x;
  }*/
  //TODO: Assumption: this is a functional read, not a timing read,
  // to physical DRAM memory.  It does not matter where it was sent from.
  MemoryInterface::Instance()->functionalRead(addr, (uint8_t*)buffer, size);
}
void SimicsInterface::WritePhysical(uint64_t addr, const void* buffer, size_t size)
{
  /*uinteger_t x;
  for(size_t i = 0; i < size; i++)
  {
  	x = ((const uint8_t*)buffer)[i];
  	SIM_write_phys_memory(SIM_get_processor(0), addr + i, x, 1);
  }*/
  //TODO: Assumption: this is a functional write, not a timing write.
  // The location is physical DRAM memory, and packet path is unimportant.
  MemoryInterface::Instance()->functionalWrite(addr, (uint8_t*)buffer, size);
}
void SimicsInterface::HandleMessage(int src, int dst, const void* buffer, int bufferSize)
{
  if (netHandlers.find(dst) != netHandlers.end()) {
    for (size_t i = 0; i < netHandlers[dst].size(); i++) {
      netHandlers[dst][i]->Call(src, buffer, bufferSize);
    }
  }
}
void SimicsCBWrapper(void* args)
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
int SimicsInterface::GetBufferID(int core)
{
  fatal ("Buffer in cache not supported!\n");
  //return QueryRubyInterface()->acquireBufferID(core);
  return 0;
}
void SimicsInterface::FreeBuffer(int nodeID, int buffer, CallbackBase* cb)
{
  fatal ("Buffer in cache not supported!\n");
  //QueryRubyInterface()->bufferFreeCB(nodeID, buffer, SimicsCBWrapper, cb);
}
void SimicsInterface::TryBufferAllocate(int nodeID, int buffer, unsigned int size, Arg1CallbackBase<bool>* cb)
{
  fatal ("Buffer in cache not supported!\n");
  //QueryRubyInterface()->bufferAllocateCB(nodeID, buffer, (size + (BLOCK_SIZE - 1)) / BLOCK_SIZE, AllocateCBWrapper, cb);
}
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
}//*/
/*void SimicsInterface::CreateAccelerator(const std::string& name, const std::vector<int>& opCodeSet, int networkNode, int networkDeviceID)
{
        conf_class_t* c = SIM_get_class("LCAcc");
	assert(c);
	conf_object_t* obj = SIM_create_object(c, name.c_str(), SIM_make_attr_list(0));
	assert(obj);
	LCAccInterface* interface = (LCAccInterface*)SIM_get_interface(obj, "LCAccInterface");

        LCAccInterface* interface = g_LCAccInterface;
	assert(interface);
	for(size_t i = 0; i < opCodeSet.size(); i++)
	{
		const char* name = interface->GetOpName(opCodeSet[i]);
		assert(name);
		interface->AddOperatingMode(obj, name);
	}
	interface->SetNetPort(obj, networkNode, networkDeviceID);
	interface->Initialize(obj, 0);
	//assert(SIM_get_pending_exception() == 0);
}
void SimicsInterface::SetAcceleratorTiming(const std::string& name, int ii, int pipelineDepth, int clockMult)
{
	conf_object_t* obj = SIM_get_object(name.c_str());
	assert(obj);
	LCAccInterface* interface = (LCAccInterface*)SIM_get_interface(obj, "LCAccInterface");

        LCAccInterface* interface = g_LCAccInterface;
	assert(interface);
	interface->SetTiming(obj, ii, pipelineDepth, clockMult);
	//assert(SIM_get_pending_exception() == 0);
}//*/
void SimicsInterface::GetOpModeTiming(int opCode, unsigned int* ii, unsigned int* pipelineDepth, unsigned int* clockMult)
{
  /*conf_object_t* obj = SIM_get_object("dummyLCAccObject");
  assert(obj);
  LCAccInterface* interface = (LCAccInterface*)SIM_get_interface(obj, "LCAccInterface");//*/
  LCAccInterface* interface = g_LCAccInterface;
  assert(interface);
  interface->GetOpTiming(opCode, ii, pipelineDepth, clockMult);
  //assert(SIM_get_pending_exception() == 0);
}
