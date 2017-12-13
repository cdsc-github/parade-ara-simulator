#ifndef SIMICS_INTERFACE_H
#define SIMICS_INTERFACE_H

#include <string>
#include <vector>
#include "Global.hh"
#include "/Common/BaseCallbacks.hh"
#include "modules/scratch-pad/scratch-pad.hh"
#include "mem/ruby/system/System.hh"

class MeteredMemory;

namespace MeteredMemory_Interface
{

class gem5Interface
{
  class LinkID
  {
  public:
    gem5Interface* si;
    int port;
  };

  std::vector<gem5Interface*> hookObj;

  std::vector<mf_gem5_interface_api*> hookApi;

  std::vector<int> controller;

  MeteredMemory* mem;

  bool timeAware;

  void CallTick();

  typedef MemberCallback0<gem5Interface, &gem5Interface::CallTick> CallTickCB;

public:
  static bool IsReadReady(gem5Interface* memObject, uint64_t addr, size_t size);

  static bool IsWriteReady(gem5Interface* memObject, uint64_t addr, size_t size);

  static void IssueRead(gem5Interface* memObject, uint64_t addr, size_t size,
    Arg1CallbackBase<const void*>* cb);

  static void IssueWrite(gem5Interface* memObject, uint64_t addr, size_t size,
    const void* data, CallbackBase* cb);

  static void IssueReadIntercept(Arg1CallbackBase<const void*>* cb);

  static typedef MemberCallback1<gem5Interface, Arg1CallbackBase<const void*>*,
    &gem5Interface::IssueReadIntercept> IssueReadInterceptCB;

  static bool SetLatency(void*, gem5Interface* handle, int* val);

  static bool SetClock(void*, gem5Interface* handle, int* val);

  static bool SetBytesPerSec(void*, gem5Interface* handle, std::string* val)

  static void ScheduleCB(CallbackBase* cb, unsigned int delay);

  uint64_t CurrentTime();

};

}

#endif
