#include "gem5Interface.hh"
#include "MeteredMemory.hh"
#include "modules/MsgLogger/MsgLogger.hh"
#include "modules/Common/MemoryDeviceInterface.hh"
#include "modules/NetworkInterrupt/NetworkInterrupts.hh"

using namespace MeteredMemory_Interface;

bool
gem5Interface::IsReadReady(gem5Interface* obj, uint64_t addr, size_t size)
{
    return true;
}

bool
gem5Interface::IsWriteReady(gem5Interface* obj, uint64_t addr, size_t size)
{
    return true;
}

void
gem5Interface::IssueRead(gem5Interface* obj, uint64_t addr, size_t size,
  Arg1CallbackBase<const void*>* cb)
{
  gem5Interface* si = (gem5Interface*)obj;
  assert(si);
  assert(cb);
  si->mem->IssueRead(addr, size, IssueReadInterceptCB::Create(si, cb));
}

void
gem5Interface::IssueWrite(gem5Interface* obj, uint64_t addr, size_t size,
  const void* data, CallbackBase* cb)
{
  gem5Interface* si = (gem5Interface*)obj;
  assert(si);
  assert(cb);
  si->mem->IssueWrite(addr, size, cb);
}

void
gem5Interface::IssueReadIntercept(Arg1CallbackBase<const void*>* cb)
{
  assert(cb);
  cb->Call(NULL);
  cb->Dispose();
}

void
gem5Interface::RecieveMemMsg(const gem5MemoryInterfaceMemRequest* mReq, void* args)
{
  assert(mReq);
  assert(args);

  LinkID* link = (LinkID*)args;
  assert(link->si->mem);
  link->si->mem->RecvRequest(*mReq, link->port);

  if (!link->si->timeAware) {
    link->si->timeAware = true;
    ScheduleCB(CallTickCB::Create(link->si), 1);
  }
}

uint64_t
gem5Interface::CurrentTime()
{
  assert(hookObj.size() > 0);
  assert(hookApi.size() > 0);
  return hookAPi[0]->CurrentTime(hookObj[0]);
}

bool
gem5Interface::SetLatency(void*, gem5Interface* p, int* val)
{
  assert(p);
  assert(val);

  p->mem->SetLatency(*val);
  ML_LOG("meteredmemory", "latency set to " << (*val) << std::endl);

  return true;
}

bool
gem5Interface::SetClock(void*, gem5Interface* p, int* val)
{
  assert(p);
  assert(val);

  p->mem->SetClock(*val);
  ML_LOG("meteredmemory", "clock set to " << (*val) << std::endl);

  return true;
}

bool
gem5Interface::SetBytesPerSec(void*, gem5Interface* p, int* val)
{
  assert(p);
  assert(val);

  p->mem->SetBytesPerSec(*val);
  ML_LOG("meteredmemory", "bytes per second set to " << (*val) << std::endl);

  return true;
}

void
executeCB(void* arg)
{
  CallbackBase* cb = (CallbackBase*)arg;
  cb->Call();
  cb->Dispose();
}

void
gem5Interface::ScheduleCB(CallbackBase* cb, unsigned int delay)
{
  assert(cb);
  assert(delay >= 0);
  scheduleCB(executeCB, cb, delay);
}
