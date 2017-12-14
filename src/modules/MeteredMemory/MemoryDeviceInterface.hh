#ifndef MEMORY_DEVICE_INTERFACE_H
#define MEMORY_DEVICE_INTERFACE_H

#include "modules/Common/BaseCallbacks.hh"

class MeteredMemory;

typedef struct
{
    MeteredMemory* mem;
} MeteredMemoryHandle;

typedef struct MemoryDeviceInterface_t
{
  bool (*IsReadReady)(MeteredMemoryHandle* memObject, uint64_t addr,
    size_t size);

  bool (*IsWriteReady)(MeteredMemoryHandle* memObject, uint64_t addr,
    size_t size);

  void (*IssueRead)(MeteredMemoryHandle* memObject, uint64_t addr, size_t size,
    CallbackBase* cb);

  void (*IssueWrite)(MeteredMemoryHandle* memObject, uint64_t addr, size_t size,
    const void* dataBlock, CallbackBase* cb);

  // void (*IssueReadIntercept)(Arg1CallbackBase<const void*>* cb);

} MemoryDeviceInterface;

void executeCB(void* arg);

void ScheduleCB(CallbackBase* cb, unsigned int delay);

MemoryDeviceInterface* CreateMemoryDeviceInterface();

MeteredMemoryHandle* CreateMeteredMemoryHandle(int accID);

#endif
