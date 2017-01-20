#ifndef SIMICS_INTERFACE_H
#define SIMICS_INTERFACE_H

#define SIM_ENHANCE
#define SIM_NET_PORTS
#define SIM_MEMORY
//#define BUFFER_IN_CACHE
#include "../Common/BaseCallbacks.hh"
#include "../scratch-pad/scratch-pad.hh"
#include "../../mem/ruby/system/System.hh"

void RubyExecuteCB(void* arg);
void ScheduleCB(int delay, CallbackBase* cb);
//uint64_t GetSystemTime();
void WriteMemory(uint64_t memAddr, uint8_t* data, size_t size);
void ReadMemory(uint64_t memAddr, uint8_t* data, size_t size);
#endif
