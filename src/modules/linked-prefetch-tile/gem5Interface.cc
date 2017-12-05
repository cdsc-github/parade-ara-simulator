#include "gem5Interface.hh"
#include "../Common/mf_api.hh"
#include "../LCAcc/memInterface.hh"

static void gem5ExecuteCB(void* arg);

static void
gem5ExecuteCB(void* arg)
{
  CallbackBase* cb = (CallbackBase*)arg;
  cb->Call();
  cb->Dispose();
}

void
RubyExecuteCB(void* arg)
{
  CallbackBase* cb = (CallbackBase*)arg;
  cb->Call();
  cb->Dispose();
}

void
ScheduleCB(int delay, CallbackBase* cb)
{
  assert(cb);
  assert(delay >= 0);
  scheduleCB(gem5ExecuteCB, cb, delay);
}

void
WriteMemory(uint64_t memAddr, uint64_t val, size_t size)
{
  assert(size <= sizeof(uint64_t));
  MemoryInterface::Instance()->functionalWrite(memAddr, (uint8_t*) &val, size);
}

uint64_t
ReadMemory(uint64_t memAddr, size_t size)
{
  assert(size <= sizeof(uint64_t));
  uint64_t d;
  MemoryInterface::Instance()->functionalRead(memAddr, (uint8_t*) &d, size);
  return d;
}
