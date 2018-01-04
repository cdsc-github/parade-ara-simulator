/*
 * Copyright (c) 1999-2013 Mark D. Hill and David A. Wood
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

/*
   This file has been modified by Kevin Moore and Dan Nussbaum of the
   Scalable Systems Research Group at Sun Microsystems Laboratories
   (http://research.sun.com/scalable/) to support the Adaptive
   Transactional Memory Test Platform (ATMTP).

   Please send email to atmtp-interest@sun.com with feedback, questions, or
   to request future announcements about ATMTP.

   ----------------------------------------------------------------------

   File modification date: 2008-02-23

   ----------------------------------------------------------------------
*/

#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>

// 1. Dedicated-ARA version
#define SIM_DEDICATED_ARA

#define SIM_NET_PORTS
#ifdef SIM_NET_PORTS
#include "modules/LCAcc/gem5Interface.hh"
#endif

#include "base/stl_helpers.hh"
#include "base/str.hh"
#include "mem/protocol/MachineType.hh"
#include "mem/protocol/RubyRequest.hh"
#include "mem/ruby/network/Network.hh"
#include "mem/ruby/profiler/AddressProfiler.hh"
#include "mem/ruby/profiler/Profiler.hh"
#include "mem/ruby/system/Sequencer.hh"
#include "mem/ruby/system/System.hh"

#ifdef SIM_VISUAL_TRACE
#include "sim/system.hh"
#include "sim/eventq.hh"
#include <string>
#include <sstream>
static uint64_t visual_trace_period = 10000;
#endif

using namespace std;
using m5::stl_helpers::operator<<;

Profiler::Profiler(const RubySystemParams *p)
  : SimObject(p), m_event(this)
{
#ifdef SIM_VISUAL_TRACE
    trace_filename = p->visual_trace;
    traceFile.open(trace_filename);
#endif

    m_hot_lines = p->hot_lines;
    m_all_instructions = p->all_instructions;

    m_address_profiler_ptr = new AddressProfiler(p->num_of_sequencers);
    m_address_profiler_ptr->setHotLines(m_hot_lines);
    m_address_profiler_ptr->setAllInstructions(m_all_instructions);

    if (m_all_instructions) {
        m_inst_profiler_ptr = new AddressProfiler(p->num_of_sequencers);
        m_inst_profiler_ptr->setHotLines(m_hot_lines);
        m_inst_profiler_ptr->setAllInstructions(m_all_instructions);
    }
}

Profiler::~Profiler()
{
#ifdef SIM_VISUAL_TRACE
  if(traceFile)
  {
    traceFile.close();
  }
#endif
}

void
Profiler::regStats(const std::string &pName)
{
    if (!m_all_instructions) {
        m_address_profiler_ptr->regStats(pName);
    }

    if (m_all_instructions) {
        m_inst_profiler_ptr->regStats(pName);
    }

    delayHistogram
        .init(10)
        .name(pName + ".delayHist")
        .desc("delay histogram for all message")
        .flags(Stats::nozero | Stats::pdf | Stats::oneline);

    uint32_t numVNets = Network::getNumberOfVirtualNetworks();
    for (int i = 0; i < numVNets; i++) {
        delayVCHistogram.push_back(new Stats::Histogram());
        delayVCHistogram[i]
            ->init(10)
            .name(pName + csprintf(".delayVCHist.vnet_%i", i))
            .desc(csprintf("delay histogram for vnet_%i", i))
            .flags(Stats::nozero | Stats::pdf | Stats::oneline);
    }

    m_outstandReqHist
        .init(10)
        .name(pName + ".outstanding_req_hist")
        .desc("")
        .flags(Stats::nozero | Stats::pdf | Stats::oneline);

    m_latencyHist
        .init(10)
        .name(pName + ".latency_hist")
        .desc("")
        .flags(Stats::nozero | Stats::pdf | Stats::oneline);

    m_hitLatencyHist
        .init(10)
        .name(pName + ".hit_latency_hist")
        .desc("")
        .flags(Stats::nozero | Stats::pdf | Stats::oneline);

    m_missLatencyHist
        .init(10)
        .name(pName + ".miss_latency_hist")
        .desc("")
        .flags(Stats::nozero | Stats::pdf | Stats::oneline);

    for (int i = 0; i < RubyRequestType_NUM; i++) {
        m_typeLatencyHist.push_back(new Stats::Histogram());
        m_typeLatencyHist[i]
            ->init(10)
            .name(pName + csprintf(".%s.latency_hist",
                                    RubyRequestType(i)))
            .desc("")
            .flags(Stats::nozero | Stats::pdf | Stats::oneline);

        m_hitTypeLatencyHist.push_back(new Stats::Histogram());
        m_hitTypeLatencyHist[i]
            ->init(10)
            .name(pName + csprintf(".%s.hit_latency_hist",
                                    RubyRequestType(i)))
            .desc("")
            .flags(Stats::nozero | Stats::pdf | Stats::oneline);

        m_missTypeLatencyHist.push_back(new Stats::Histogram());
        m_missTypeLatencyHist[i]
            ->init(10)
            .name(pName + csprintf(".%s.miss_latency_hist",
                                    RubyRequestType(i)))
            .desc("")
            .flags(Stats::nozero | Stats::pdf | Stats::oneline);
    }

    for (int i = 0; i < MachineType_NUM; i++) {
        m_hitMachLatencyHist.push_back(new Stats::Histogram());
        m_hitMachLatencyHist[i]
            ->init(10)
            .name(pName + csprintf(".%s.hit_mach_latency_hist",
                                    MachineType(i)))
            .desc("")
            .flags(Stats::nozero | Stats::pdf | Stats::oneline);

        m_missMachLatencyHist.push_back(new Stats::Histogram());
        m_missMachLatencyHist[i]
            ->init(10)
            .name(pName + csprintf(".%s.miss_mach_latency_hist",
                                    MachineType(i)))
            .desc("")
            .flags(Stats::nozero | Stats::pdf | Stats::oneline);

        m_IssueToInitialDelayHist.push_back(new Stats::Histogram());
        m_IssueToInitialDelayHist[i]
            ->init(10)
            .name(pName + csprintf(
                ".%s.miss_latency_hist.issue_to_initial_request",
                MachineType(i)))
            .desc("")
            .flags(Stats::nozero | Stats::pdf | Stats::oneline);

        m_InitialToForwardDelayHist.push_back(new Stats::Histogram());
        m_InitialToForwardDelayHist[i]
            ->init(10)
            .name(pName + csprintf(".%s.miss_latency_hist.initial_to_forward",
                                   MachineType(i)))
            .desc("")
            .flags(Stats::nozero | Stats::pdf | Stats::oneline);

        m_ForwardToFirstResponseDelayHist.push_back(new Stats::Histogram());
        m_ForwardToFirstResponseDelayHist[i]
            ->init(10)
            .name(pName + csprintf(
                ".%s.miss_latency_hist.forward_to_first_response",
                MachineType(i)))
            .desc("")
            .flags(Stats::nozero | Stats::pdf | Stats::oneline);

        m_FirstResponseToCompletionDelayHist.push_back(new Stats::Histogram());
        m_FirstResponseToCompletionDelayHist[i]
            ->init(10)
            .name(pName + csprintf(
                ".%s.miss_latency_hist.first_response_to_completion",
                MachineType(i)))
            .desc("")
            .flags(Stats::nozero | Stats::pdf | Stats::oneline);

        m_IncompleteTimes[i]
            .name(pName + csprintf(".%s.incomplete_times", MachineType(i)))
            .desc("")
            .flags(Stats::nozero);
    }

#ifdef SIM_NET_PORTS
    m_spmReads
      .name(pName + ".num_spm_reads")
      .desc("")
      ;
    m_spmWrites
      .name(pName + ".num_spm_writes")
      .desc("")
      ;
#endif

    for (int i = 0; i < RubyRequestType_NUM; i++) {
        m_hitTypeMachLatencyHist.push_back(std::vector<Stats::Histogram *>());
        m_missTypeMachLatencyHist.push_back(std::vector<Stats::Histogram *>());

        for (int j = 0; j < MachineType_NUM; j++) {
            m_hitTypeMachLatencyHist[i].push_back(new Stats::Histogram());
            m_hitTypeMachLatencyHist[i][j]
                ->init(10)
                .name(pName + csprintf(".%s.%s.hit_type_mach_latency_hist",
                                       RubyRequestType(i), MachineType(j)))
                .desc("")
                .flags(Stats::nozero | Stats::pdf | Stats::oneline);

            m_missTypeMachLatencyHist[i].push_back(new Stats::Histogram());
            m_missTypeMachLatencyHist[i][j]
                ->init(10)
                .name(pName + csprintf(".%s.%s.miss_type_mach_latency_hist",
                                       RubyRequestType(i), MachineType(j)))
                .desc("")
                .flags(Stats::nozero | Stats::pdf | Stats::oneline);
        }
    }

#ifdef SIM_VISUAL_TRACE
    m_L1Cache_read = 0;
    m_L1Cache_write = 0;
    m_L1Cache_miss = 0;
    m_L2Cache_read = 0;
    m_L2Cache_write = 0;
    m_L2Cache_miss = 0;
    m_MC_read = 0;
    m_MC_write = 0;

    std::vector<System *>::iterator system_iterator = System::systemList.begin();
    System *m5_system = *system_iterator;
    assert(m5_system);
    int num_thread_contexts = m5_system->numContexts();
    int num_L1Cache = num_thread_contexts + RubySystem::numberOfTDs() +
        RubySystem::numberOfAccelerators() * RubySystem::numberOfAccInstances();

    m_bank_specific_L1Cache_active.resize(num_L1Cache);
    m_bank_specific_L1Cache_read.resize(num_L1Cache);
    m_bank_specific_L1Cache_write.resize(num_L1Cache);
    m_bank_specific_L1Cache_miss.resize(num_L1Cache);
    for(int i = 0; i < m_bank_specific_L1Cache_active.size(); i++) {
      m_bank_specific_L1Cache_active[i] = 0;
      m_bank_specific_L1Cache_read[i] = 0;
      m_bank_specific_L1Cache_write[i] = 0;
      m_bank_specific_L1Cache_miss[i] = 0;
    }

    m_bank_specific_L2Cache_active.resize(RubySystem::numberOfgem5NetworkPort());
    m_bank_specific_L2Cache_read.resize(RubySystem::numberOfgem5NetworkPort());
    m_bank_specific_L2Cache_write.resize(RubySystem::numberOfgem5NetworkPort());
    m_bank_specific_L2Cache_miss.resize(RubySystem::numberOfgem5NetworkPort());
    for(int i = 0; i < m_bank_specific_L2Cache_active.size(); i++) {
      m_bank_specific_L2Cache_active[i] = 0;
      m_bank_specific_L2Cache_read[i] = 0;
      m_bank_specific_L2Cache_write[i] = 0;
      m_bank_specific_L2Cache_miss[i] = 0;
    }

    m_bank_specific_MC_read.resize(DirectoryMemory::getNumDirectories());
    m_bank_specific_MC_write.resize(DirectoryMemory::getNumDirectories());
    for(int i = 0; i < m_bank_specific_MC_read.size(); i++) {
      m_bank_specific_MC_read[i] = 0;
      m_bank_specific_MC_write[i] = 0;
    }
#endif
}

void
Profiler::collateStats()
{
    if (!m_all_instructions) {
        m_address_profiler_ptr->collateStats();
    }

    if (m_all_instructions) {
        m_inst_profiler_ptr->collateStats();
    }

    uint32_t numVNets = Network::getNumberOfVirtualNetworks();
    for (uint32_t i = 0; i < MachineType_NUM; i++) {
        for (map<uint32_t, AbstractController*>::iterator it =
                  g_abs_controls[i].begin();
             it != g_abs_controls[i].end(); ++it) {

            AbstractController *ctr = (*it).second;
            delayHistogram.add(ctr->getDelayHist());

            for (uint32_t i = 0; i < numVNets; i++) {
                delayVCHistogram[i]->add(ctr->getDelayVCHist(i));
            }
        }
    }

    for (uint32_t i = 0; i < MachineType_NUM; i++) {
        for (map<uint32_t, AbstractController*>::iterator it =
                g_abs_controls[i].begin();
                it != g_abs_controls[i].end(); ++it) {

            AbstractController *ctr = (*it).second;
            Sequencer *seq = ctr->getSequencer();
            if (seq != NULL) {
                m_outstandReqHist.add(seq->getOutstandReqHist());
            }
        }
    }

    for (uint32_t i = 0; i < MachineType_NUM; i++) {
        for (map<uint32_t, AbstractController*>::iterator it =
                g_abs_controls[i].begin();
                it != g_abs_controls[i].end(); ++it) {

            AbstractController *ctr = (*it).second;
            Sequencer *seq = ctr->getSequencer();
            if (seq != NULL) {
                // add all the latencies
                m_latencyHist.add(seq->getLatencyHist());
                m_hitLatencyHist.add(seq->getHitLatencyHist());
                m_missLatencyHist.add(seq->getMissLatencyHist());

                // add the per request type latencies
                for (uint32_t j = 0; j < RubyRequestType_NUM; ++j) {
                    m_typeLatencyHist[j]
                        ->add(seq->getTypeLatencyHist(j));
                    m_hitTypeLatencyHist[j]
                        ->add(seq->getHitTypeLatencyHist(j));
                    m_missTypeLatencyHist[j]
                        ->add(seq->getMissTypeLatencyHist(j));
                }

                // add the per machine type miss latencies
                for (uint32_t j = 0; j < MachineType_NUM; ++j) {
                    m_hitMachLatencyHist[j]
                        ->add(seq->getHitMachLatencyHist(j));
                    m_missMachLatencyHist[j]
                        ->add(seq->getMissMachLatencyHist(j));

                    m_IssueToInitialDelayHist[j]->add(
                        seq->getIssueToInitialDelayHist(MachineType(j)));

                    m_InitialToForwardDelayHist[j]->add(
                        seq->getInitialToForwardDelayHist(MachineType(j)));
                    m_ForwardToFirstResponseDelayHist[j]->add(seq->
                        getForwardRequestToFirstResponseHist(MachineType(j)));

                    m_FirstResponseToCompletionDelayHist[j]->add(seq->
                        getFirstResponseToCompletionDelayHist(
                            MachineType(j)));
                    m_IncompleteTimes[j] +=
                        seq->getIncompleteTimes(MachineType(j));
                }

                // add the per (request, machine) type miss latencies
                for (uint32_t j = 0; j < RubyRequestType_NUM; j++) {
                    for (uint32_t k = 0; k < MachineType_NUM; k++) {
                        m_hitTypeMachLatencyHist[j][k]->add(
                                seq->getHitTypeMachLatencyHist(j,k));
                        m_missTypeMachLatencyHist[j][k]->add(
                                seq->getMissTypeMachLatencyHist(j,k));
                    }
                }
            }
        }
    }
#if defined(SIM_DEDICATED_ARA)
    uint64_t total_spmReads = 0;
    uint64_t total_spmWrites = 0;
    printf("spmSet.size() = %lld\n", LCAcc::gem5Interface::manager.deviceSet.size());
    for(int i = 0; i < LCAcc::gem5Interface::manager.deviceSet.size(); i++) {
      total_spmReads += LCAcc::gem5Interface::manager.deviceSet.at(i)->getSPM()->get_num_spm_reads();
      total_spmWrites += LCAcc::gem5Interface::manager.deviceSet.at(i)->getSPM()->get_num_spm_writes();
    }
    printf("total_spmReads = %lld\n", total_spmReads);
    printf("total_spmWrites = %lld\n", total_spmWrites);
    m_spmReads = total_spmReads;
    m_spmWrites = total_spmWrites;
#endif
}

void
Profiler::addAddressTraceSample(const RubyRequest& msg, NodeID id)
{
    if (msg.getType() != RubyRequestType_IFETCH) {
        // Note: The following line should be commented out if you
        // want to use the special profiling that is part of the GS320
        // protocol

        // NOTE: Unless PROFILE_HOT_LINES is enabled, nothing will be
        // profiled by the AddressProfiler
        m_address_profiler_ptr->
            addTraceSample(msg.getLineAddress(), msg.getProgramCounter(),
                           msg.getType(), msg.getAccessMode(), id, false);
    }
}

#ifdef SIM_VISUAL_TRACE
std::string int_to_string(int n);
std::string int_to_string(int n)
{
  stringstream ss;
  ss << n;
  return ss.str();
}
void Profiler::controllerActive(MachineID machID)
{
  if(machID.type == MachineType_L1Cache)
  {
    traceProfile_L1Cache_active(machID.num);
  }
  else if(machID.type == MachineType_L2Cache)
  {
    traceProfile_L2Cache_active(machID.num);
  }
}
void Profiler::reportStatEvent(const std::string& deviceName, const std::string& attributeTitle, int value)
{
  traceFile << "Tick$" << g_system_ptr->curCycle() << "$System::" << deviceName << "$" << attributeTitle << "$" << value << endl;
}

void Profiler::reportStatEvent(const std::string& deviceName, const std::string& attributeTitle, float value)
{
  traceFile << "Tick$" << g_system_ptr->curCycle() << "$System::" << deviceName << "$" << attributeTitle << "$" << value << endl;
}

void Profiler::reportStatEvent(const std::string& deviceName, const std::string& attributeTitle, double value)
{
  traceFile << "Tick$" << g_system_ptr->curCycle() << "$System::" << deviceName << "$" << attributeTitle << "$" << value << endl;
}

void Profiler::reportStatEvent(const std::string& deviceName, const std::string& attributeTitle, const std::string& value)
{
  traceFile << "Tick$" << g_system_ptr->curCycle() << "$System::" << deviceName << "$" << attributeTitle << "$" << value << endl;
}

void Profiler::traceProfile_L1Cache_active(int node) {
  assert(node >= 0);
  assert(node < m_bank_specific_L1Cache_active.size());
  m_bank_specific_L1Cache_active[node]++;
}

void Profiler::traceProfile_L1Cache_read(int node) {
  assert(node >= 0);
  assert(node < m_bank_specific_L1Cache_read.size());
  m_bank_specific_L1Cache_read[node]++;
  m_L1Cache_read++;
}

void Profiler::traceProfile_L1Cache_write(int node) {
  assert(node >= 0);
  assert(node < m_bank_specific_L1Cache_write.size());
  m_bank_specific_L1Cache_write[node]++;
  m_L1Cache_write++;
}

void Profiler::traceProfile_L1Cache_miss(int node) {
  assert(node >= 0);
  assert(node < m_bank_specific_L1Cache_miss.size());
  m_bank_specific_L1Cache_miss[node]++;
  m_L1Cache_miss++;
}

void Profiler::traceProfile_L2Cache_active(int node) {
  assert(node >= 0);
  assert(node < m_bank_specific_L2Cache_active.size());
  m_bank_specific_L2Cache_active[node]++;
}

void Profiler::traceProfile_L2Cache_read(int node) {
  assert(node >= 0);
  assert(node < m_bank_specific_L2Cache_read.size());
  m_bank_specific_L2Cache_read[node]++;
  m_L2Cache_read++;
}

void Profiler::traceProfile_L2Cache_write(int node) {
  assert(node >= 0);
  assert(node < m_bank_specific_L2Cache_write.size());
  m_bank_specific_L2Cache_write[node]++;
  m_L2Cache_write++;
}

void Profiler::traceProfile_L2Cache_miss(int node) {
  assert(node >= 0);
  assert(node < m_bank_specific_L2Cache_miss.size());
  m_bank_specific_L2Cache_miss[node]++;
  m_L2Cache_miss++;
}

void Profiler::traceProfile_MC_read(int node) {
  assert(node >= 0);
  assert(node < m_bank_specific_MC_read.size());
  m_bank_specific_MC_read[node]++;
  m_MC_read++;
}

void Profiler::traceProfile_MC_write(int node) {
  assert(node >= 0);
  assert(node < m_bank_specific_MC_write.size());
  m_bank_specific_MC_write[node]++;
  m_MC_write++;
}

void Profiler::wakeup() {
  //std::cout << "[Profiler::wakeup] @ " << g_system_ptr->curCycle() << std::endl;
  g_system_ptr->getNetwork()->dumpStatsTick();
  for(int i = 0; i < m_bank_specific_L1Cache_active.size(); i++) {
    reportStatEvent("L1Cache_" + int_to_string(i), "Active", (double)(m_bank_specific_L1Cache_active[i]) / (double)(visual_trace_period));
    m_bank_specific_L1Cache_active[i] = 0;
  }
  for(int i = 0; i < m_bank_specific_L1Cache_read.size(); i++) {
    reportStatEvent("L1Cache_" + int_to_string(i), "Read_Frequency", (double)(m_bank_specific_L1Cache_read[i]) / (double)(visual_trace_period));
    m_bank_specific_L1Cache_read[i] = 0;
  }
  for(int i = 0; i < m_bank_specific_L1Cache_write.size(); i++) {
    reportStatEvent("L1Cache_" + int_to_string(i), "Write_Frequency", (double)(m_bank_specific_L1Cache_write[i]) / (double)(visual_trace_period));
    m_bank_specific_L1Cache_write[i] = 0;
  }
  for(int i = 0; i < m_bank_specific_L1Cache_miss.size(); i++) {
    reportStatEvent("L1Cache_" + int_to_string(i), "Miss_Frequency", (double)(m_bank_specific_L1Cache_miss[i]) / (double)(visual_trace_period));
    m_bank_specific_L1Cache_miss[i] = 0;
  }
  for(int i = 0; i < m_bank_specific_L2Cache_active.size(); i++) {
    reportStatEvent("L2Cache_" + int_to_string(i), "Active", (double)(m_bank_specific_L2Cache_active[i]) / (double)(visual_trace_period));
    m_bank_specific_L2Cache_active[i] = 0;
  }
  for(int i = 0; i < m_bank_specific_L2Cache_read.size(); i++) {
    reportStatEvent("L2Cache_" + int_to_string(i), "Read_Frequency", (double)(m_bank_specific_L2Cache_read[i]) / (double)(visual_trace_period));
    m_bank_specific_L2Cache_read[i] = 0;
  }
  for(int i = 0; i < m_bank_specific_L2Cache_write.size(); i++) {
    reportStatEvent("L2Cache_" + int_to_string(i), "Write_Frequency", (double)(m_bank_specific_L2Cache_write[i]) / (double)(visual_trace_period));
    m_bank_specific_L2Cache_write[i] = 0;
  }
  for(int i = 0; i < m_bank_specific_L2Cache_miss.size(); i++) {
    reportStatEvent("L2Cache_" + int_to_string(i), "Miss_Frequency", (double)(m_bank_specific_L2Cache_miss[i]) / (double)(visual_trace_period));
    m_bank_specific_L2Cache_miss[i] = 0;
  }
  for(int i = 0; i < m_bank_specific_MC_read.size(); i++) {
    reportStatEvent("MC_" + int_to_string(i), "Read_Frequency", (double)(m_bank_specific_MC_read[i]) / (double)(visual_trace_period));
    m_bank_specific_MC_read[i] = 0;
  }
  for(int i = 0; i < m_bank_specific_MC_write.size(); i++) {
    reportStatEvent("MC_" + int_to_string(i), "Write_Frequency", (double)(m_bank_specific_MC_write[i]) / (double)(visual_trace_period));
    m_bank_specific_MC_write[i] = 0;
  }
  schedule(m_event, g_system_ptr->clockEdge(Cycles(visual_trace_period)));
}

#endif
