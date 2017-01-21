#include "MsgLogger.h"
#include <map>
#include <cassert>
#include <fstream>
#include <simics/api.h>
#include <simics/alloc.h>
#include <simics/utils.h>
#define SIMICS30
#define SIM_ENHANCE
#include "../Common/mf_api.hh"

std::ofstream outputStream;
unsigned long long GetSystemTime()
{
  static mf_opal_api_t* interface = NULL;

  if (interface == NULL) {
    interface = (mf_opal_api_t *) SIM_get_interface( SIM_get_object("opal0"), "mf-opal-api" );
  }

  return interface->getOpalTime(0);
}
std::ostream& Log(const std::string& x)
{
  unsigned long long tick = GetSystemTime();
  outputStream << "[obj:\"" << x << "\"] [tick:" << tick << "] ";
  return outputStream;
}
static conf_object_t* CreateNewDeviceHandle(parse_object_t* po)
{
  static conf_object_t* handle = NULL;
  assert(handle == NULL);
  handle = MM_ZALLOC(1, conf_object_t);
  SIM_object_constructor(handle, po);
  return handle;
}
attr_value_t SetFileName(void*, conf_object_t*, attr_value_t* var)
{
  std::string n;
  assert(var->kind == Sim_Val_List && var->u.list.size == 1);
  var = var->u.list.vector;
  assert(var->kind == Sim_Val_String);
  outputStream.open(var->u.string);
  return SIM_make_attr_string("Success\n");
}

attr_value_t SetFileState(void*, conf_object_t*, attr_value_t* var)
{
  outputStream.open(var->u.string);
  return SIM_make_attr_string("Success\n");
}

#ifdef __cplusplus
extern "C" {
#endif
void init_local()
{
  class_data_t funcs;
  conf_class_t *objClass;
  MsgLogger_Interface* interface;

  memset(&funcs, 0, sizeof(class_data_t));
  funcs.new_instance = CreateNewDeviceHandle;
  funcs.description =
    "THE logger.";
  objClass = SIM_register_class("MsgLogger", &funcs);
  interface = MM_ZALLOC(1, MsgLogger_Interface);
  interface->Log = Log;
  SIM_register_interface(objClass, "MsgLogger_Interface", interface);
  SIM_register_typed_attribute(objClass, "SetFileName", SetFileName, NULL, NULL, NULL, Sim_Attr_Session, "s", NULL, "");
  SIM_register_typed_attribute(objClass, "SetFileState", SetFileState, NULL, NULL, NULL, Sim_Attr_Session, "s", NULL, "");
  SIM_new_object(objClass, "MsgLogger");
}
#ifdef __cplusplus
}
#endif
