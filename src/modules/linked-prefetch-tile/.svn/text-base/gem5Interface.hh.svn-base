#ifndef SIMICS_INTERFACE_H
#define SIMICS_INTERFACE_H

#define SIM_ENHANCE
#define SIM_NET_PORTS
#define SIM_MEMORY
#include "../Common/BaseCallbacks.hh"
#include "../scratch-pad/scratch-pad.hh"
#include "../../mem/ruby/system/System.hh"

void RubyExecuteCB(void* arg);
void ScheduleCB(int delay, CallbackBase* cb);
void WriteMemory(uint64_t memAddr, uint64_t val, size_t size);
uint64_t ReadMemory(uint64_t memAddr, size_t size);
#endif
