#ifndef MEMORY_DEVICE_INTERFACE_H
#define MEMORY_DEVICE_INTERFACE_H

#include "BaseCallbacks.hh"
namespace MeteredMemory_Interface
{
class SimicsInterface;
};

typedef struct MemoryDeviceInterface_t {
  bool (*IsReadReady)(MeteredMemory_Interface::SimicsInterface* obj, uint64_t addr, size_t size);
  bool (*IsWriteReady)(MeteredMemory_Interface::SimicsInterface* obj, uint64_t addr, size_t size);
  void (*IssueRead)(MeteredMemory_Interface::SimicsInterface* obj, uint64_t addr, size_t size, Arg1CallbackBase<const void*>* cb);
  void (*IssueWrite)(MeteredMemory_Interface::SimicsInterface* obj, uint64_t addr, size_t size, const void* dataBlock, CallbackBase* cb);
} MemoryDeviceInterface;

#endif
