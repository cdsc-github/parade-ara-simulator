#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>

#define SIM_NET_PORTS
#define SIM_MEMORY
#define SIM_ENHANCE

#ifndef SIMICS30
#define SIMICS30
#endif

#include "modules/Common/mf_api.hh"
#include "modules/Common/BaseCallbacks.hh"
#include "mem/ruby/system/gem5NetworkPortInterface.hh"

#endif
