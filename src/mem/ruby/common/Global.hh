/*
 * Copyright (c) 1999-2008 Mark D. Hill and David A. Wood
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __MEM_RUBY_COMMON_GLOBAL_HH__
#define __MEM_RUBY_COMMON_GLOBAL_HH__

#include <map>
#include <vector>

#include "base/str.hh"
#include "base/types.hh"

class RubySystem;
extern RubySystem* g_system_ptr;

class AbstractController;
extern std::vector<std::map<uint32_t, AbstractController *> > g_abs_controls;

// A globally visible time at which the actual execution started. Meant only
// for components with in Ruby. Initialized in RubySystem::startup().
extern Cycles g_ruby_start;

#define SIM_NET_PORTS
#ifdef SIM_NET_PORTS
#include "../../../modules/NetworkInterrupt/lwint.hh"
#include "../../../modules/NetworkInterrupt/NetworkInterrupts.hh"
#include "../system/SimicsNetworkPortInterface.hh"
#include "../../../modules/TLBHack/TLBHack.hh"
// #include "../../../modules/GlobalAcceleratorManager/GAM.hh"
#include "../../../modules/Common/MemoryDeviceInterface.hh"
#include "../../../modules/linked-prefetch-tile/DMAEngine.hh"
#include "../../../modules/linked-prefetch-tile/prefetcher-tile.hh"
#include "../../../modules/scratch-pad/scratch-pad.hh"
#include "../../../modules/LCAcc/SimicsInterface.hh"
#include "../../../modules/TaskDistributor/Interface.hh"
#include "modules/LCAcc/Interface.hh"
extern lwInt_ifc_t* g_lwInt_interface;
extern SimicsNetworkPortInterface* g_networkPort_interface;
extern std::vector<NetworkInterruptHandle*> g_network_interrupt_handle;
extern std::vector<NetworkInterrupts*> g_network_interrupts;
extern TLBHackInterface* g_TLBHack_interface;
// extern GAMInterface* g_gamObject;
extern std::vector<TDHandle*> g_TDHandle;
extern TaskDistributorInterface* g_TDInterface;
extern MeteredMemory_Interface::SimicsInterface* g_memObject;
extern MemoryDeviceInterface* g_memInterface;
extern std::vector<DMAEngineHandle*> g_dmaDevice;
extern std::vector<DMAEngineHandle*> g_TdDmaDevice;
extern prftch_direct_interface_t* g_dmaInterface;
extern std::vector<ScratchPadHandle*> g_spmObject;
extern std::vector<ScratchPadHandle*> g_TdSpmObject;
extern scratch_pad_interface_t* g_spmInterface;
extern std::vector<LCAccDeviceHandle*> g_LCAccDeviceHandle;
extern LCAccInterface* g_LCAccInterface;
#endif

#endif // __MEM_RUBY_COMMON_GLOBAL_HH__
