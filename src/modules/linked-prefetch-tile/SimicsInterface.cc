#include "SimicsInterface.hh"
#include "../Common/mf_api.hh"
#include "../LCAcc/memInterface.hh"

static void SimicsExecuteCB(void* arg);
static void SimicsExecuteCB(void* arg)
{
  CallbackBase* cb = (CallbackBase*)arg;
  cb->Call();
  cb->Dispose();
}
void RubyExecuteCB(void* arg)
{
  CallbackBase* cb = (CallbackBase*)arg;
  cb->Call();
  cb->Dispose();
}
void ScheduleCB(int delay, CallbackBase* cb)
{
  assert(cb);
  assert(delay >= 0);//*/
  scheduleCB(SimicsExecuteCB, cb, delay);
}
uint64_t GetSystemTime()
{
  return (unsigned long long)uint64_t(g_system_ptr->curCycle());
}
void
WriteMemory(uint64_t memAddr, uint8_t* data, size_t size)
{
  // assert(size <= sizeof(uint64_t));
  MemoryInterface::Instance()->functionalWrite(memAddr, data, size);
}

void
ReadMemory(uint64_t memAddr, uint8_t* data, size_t size)
{
  // assert(size <= sizeof(uint64_t));
  MemoryInterface::Instance()->functionalRead(memAddr, data, size);
}
