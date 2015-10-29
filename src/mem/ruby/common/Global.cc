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

#include "mem/ruby/common/Global.hh"

using namespace std;

RubySystem* g_system_ptr = 0;
vector<map<uint32_t, AbstractController *> > g_abs_controls;
Cycles g_ruby_start;
#ifdef SIM_NET_PORTS
lwInt_ifc_t* g_lwInt_interface;
gem5NetworkPortInterface* g_networkPort_interface;
vector<NetworkInterruptHandle*> g_network_interrupt_handle;
vector<NetworkInterrupts*> g_network_interrupts;
TLBHackInterface* g_TLBHack_interface;
TaskDistributorInterface* g_TDInterface;
vector<TDHandle*> g_TDHandle;
MeteredMemory_Interface::gem5Interface* g_memObject;
MemoryDeviceInterface* g_memInterface;
vector<DMAEngineHandle*> g_dmaDevice;
vector<DMAEngineHandle*> g_TdDmaDevice;
prftch_direct_interface_t* g_dmaInterface;
vector<ScratchPadHandle*> g_spmObject;
vector<ScratchPadHandle*> g_TdSpmObject;
scratch_pad_interface_t* g_spmInterface;
vector<LCAccDeviceHandle*> g_LCAccDeviceHandle;
LCAccInterface* g_LCAccInterface;
#endif
