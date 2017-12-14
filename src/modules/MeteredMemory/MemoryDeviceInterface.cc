#include "MeteredMemory.hh"
#include "modules/MsgLogger/MsgLogger.hh"
#include "MemoryDeviceInterface.hh"

bool IsReadReady(MeteredMemoryHandle* memObject, uint64_t addr, size_t size);

bool IsWriteReady(MeteredMemoryHandle* memObject, uint64_t addr, size_t size);

void IssueRead(MeteredMemoryHandle* memObject, uint64_t addr, size_t size,
  CallbackBase* cb);

void IssueWrite(MeteredMemoryHandle* memObject, uint64_t addr, size_t size,
  const void* dataBlock, CallbackBase* cb);

void IssueReadIntercept(Arg1CallbackBase<const void*>* cb);

bool
IsReadReady(MeteredMemoryHandle* memObject, uint64_t addr, size_t size)
{
    return true;
}

bool
IsWriteReady(MeteredMemoryHandle* memObject, uint64_t addr, size_t size)
{
    return true;
}

void
IssueRead(MeteredMemoryHandle* memObject, uint64_t addr, size_t size,
  CallbackBase* cb)
{
  MeteredMemoryHandle* handle = (MeteredMemoryHandle*)memObject;
  assert(handle);
  assert(cb);
  handle->mem->IssueRead(addr, size, cb);
}

void
IssueWrite(MeteredMemoryHandle* memObject, uint64_t addr, size_t size,
  const void* data, CallbackBase* cb)
{
  MeteredMemoryHandle* handle = (MeteredMemoryHandle*)memObject;
  assert(handle);
  assert(cb);
  handle->mem->IssueWrite(addr, size, cb);
}

// void
// RecieveMemMsg(const gem5MemoryInterfaceMemRequest* mReq, void* args)
// {
//   assert(mReq);
//   assert(args);

//   LinkID* link = (LinkID*)args;
//   assert(link->si->mem);
//   link->si->mem->RecvRequest(*mReq, link->port);

//   if (!link->si->timeAware) {
//     link->si->timeAware = true;
//     ScheduleCB(CallTickCB::Create(link->si), 1);
//   }
// }

void
executeCB(void* arg)
{
  CallbackBase* cb = (CallbackBase*)arg;
  cb->Call();
  cb->Dispose();
}

void
ScheduleCB(CallbackBase* cb, unsigned int delay)
{
  assert(cb);
  assert(delay >= 0);
  scheduleCB(executeCB, cb, delay);
}

MeteredMemoryHandle*
CreateMeteredMemoryHandle(int accID)
{
  MeteredMemoryHandle* memObject = new MeteredMemoryHandle;
  memObject->mem = new MeteredMemory(accID);
  return memObject;
}

MemoryDeviceInterface*
CreateMemoryDeviceInterface()
{
  MemoryDeviceInterface* memInterface = new MemoryDeviceInterface;

  memInterface->IsReadReady = IsReadReady;
  memInterface->IsWriteReady = IsWriteReady;
  memInterface->IssueRead = IssueRead;
  memInterface->IssueWrite = IssueWrite;

  return memInterface;
}
