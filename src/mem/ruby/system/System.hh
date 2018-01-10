/*
 * Copyright (c) 1999-2012 Mark D. Hill and David A. Wood
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
 * Contains all of the various parts of the system we are simulating.
 * Performs allocation, deallocation, and setup of all the major
 * components of the system
 */

#ifndef __MEM_RUBY_SYSTEM_SYSTEM_HH__
#define __MEM_RUBY_SYSTEM_SYSTEM_HH__

#include "base/callback.hh"
#include "base/output.hh"
#include "mem/ruby/profiler/Profiler.hh"
#include "mem/ruby/slicc_interface/AbstractController.hh"
#include "mem/ruby/system/CacheRecorder.hh"
#include "mem/packet.hh"
#include "params/RubySystem.hh"
#include "sim/clocked_object.hh"

#define SIM_NET_PORTS

class Network;

class RubySystem : public ClockedObject
{
  public:
    class RubyEvent : public Event
    {
      public:
        RubyEvent(RubySystem* _ruby_system)
        {
            ruby_system = _ruby_system;
        }
      private:
        void process();

        RubySystem* ruby_system;
    };

    friend class RubyEvent;

    typedef RubySystemParams Params;
    RubySystem(const Params *p);
    ~RubySystem();

    // config accessors
#ifdef SIM_NET_PORTS
    static std::vector<std::string> accTypes;
    static bool m_aim;
    static int m_mem_bandwidth;
    static int m_mem_clock;
    static int m_mem_latency;
    static int m_num_simics_net_ports;
    static int m_num_accelerators;
    static int m_num_acc_instances;
    static int m_num_TDs;
    static bool aim() { return m_aim; }
    static int memBandwidth() { return m_mem_bandwidth; }
    static int memClock() { return m_mem_clock; }
    static int memLatency() { return m_mem_latency; }
    static int numberOfAccelerators() { return m_num_accelerators; }
    static int numberOfgem5NetworkPortPerChip() { return m_num_simics_net_ports; }
    static int numberOfgem5NetworkPortPerChip(NodeID&) {return numberOfgem5NetworkPortPerChip(); }
    static int numberOfgem5NetworkPort() {return numberOfgem5NetworkPortPerChip(); }
    static int numberOfTDs() { return m_num_TDs; }
    static int gem5NetworkPortTransitionsPerCycle() { return 10; }
    static int threadIDtoDeviceID(int threadID);
    static int deviceIDtoThreadID(int deviceID);
    static int accIDtoDeviceID(int accID);
    static int deviceIDtoAccID(int deviceID);
    static int tdIDtoL1CacheID(int tdID);
    static int accIDtoL1CacheID(int accID);
    static int mapPortID(int port);
    static int numberOfAccInstances() { return m_num_acc_instances; }
    Network* getNetwork() {assert(m_network != NULL); return m_network;};
#endif

    static int getRandomSeed() { return m_random_seed; }
    static int getRandomization() { return m_randomization; }
    static uint32_t getBlockSizeBytes() { return m_block_size_bytes; }
    static uint32_t getBlockSizeBits() { return m_block_size_bits; }
    static uint32_t getMemorySizeBits() { return m_memory_size_bits; }

    SimpleMemory *getPhysMem() { return m_phys_mem; }

    // Public Methods
    Profiler*
    getProfiler()
    {
        assert(m_profiler != NULL);
        return m_profiler;
    }

    void regStats() { m_profiler->regStats(name()); }
    void collateStats() { m_profiler->collateStats(); }
    void resetStats();

    void parseAccTypes(std::string acc_types);

    void serialize(std::ostream &os);
    void unserialize(Checkpoint *cp, const std::string &section);
    void process();
    void startup();
    bool functionalRead(Packet *ptr);
    bool functionalWrite(Packet *ptr);

    void registerNetwork(Network*);
    void registerAbstractController(AbstractController*);

    bool eventQueueEmpty() { return eventq->empty(); }
    void enqueueRubyEvent(Tick tick)
    {
        RubyEvent* e = new RubyEvent(this);
        schedule(e, tick);
    }

  private:
    // Private copy constructor and assignment operator
    RubySystem(const RubySystem& obj);
    RubySystem& operator=(const RubySystem& obj);

    void readCompressedTrace(std::string filename,
                             uint8_t *&raw_data,
                             uint64& uncompressed_trace_size);
    void writeCompressedTrace(uint8_t *raw_data, std::string file,
                              uint64 uncompressed_trace_size);

  private:
    // configuration parameters
    static int m_random_seed;
    static bool m_randomization;
    static uint32_t m_block_size_bytes;
    static uint32_t m_block_size_bits;
    static uint32_t m_memory_size_bits;
    SimpleMemory *m_phys_mem;

    Network* m_network;
    std::vector<AbstractController *> m_abs_cntrl_vec;

    std::map<std::string, std::vector<std::string>> m_acc_dict = {
        {"BlackScholes", {"blackScholes"}},
        {"BlackScholes_DIMM", {"blackScholes_dimm"}},
        {"Deblur_Modified", {"denoise1Mega", "blur1Mega", "deblur1Mega", "deblur2Mega"}},
        {"Denoise", {"denoise1Mega", "denoise2Mega"}},
        {"Denoise_DIMM", {"denoise1_dimm", "denoise2_dimm", "denoise1", "denoise2"}},
        {"FFT_1D_DIMM", {"fft_dimm", "fft"}},
        {"FluidAnimate_DIMM", {"fluidAnimate1_dimm", "fluidAnimate1"}},
        {"Registration_DIMM", {"registration_dimm", "blur_dimm", "registration"}},
        {"Registration_Modified", {"registration1Mega", "blur1Mega"}},
        {"Segmentation", {"segmentation1Mega"}},
        {"Segmentation_DIMM", {"segmentation_dimm", "segmentation"}},
        {"StreamCluster", {"streamCluster1", "streamCluster3", "streamCluster4", "streamCluster5", "streamCluster6"}},
        {"StreamCluster_DIMM", {"streamCluster1_dimm", "streamCluster2_dimm", "streamCluster3_dimm"}},
        {"Swaptions", {"swaptions1", "swaptions2", "swaptions3", "swaptions4"}},
        {"LPCIP_Desc", {"LPCIP"}},
        {"SURF_Desc", {"SURF1Mega", "SURF2Mega", "SURF3Mega", "SURF4Mega", "SURF5Mega"}},
        {"Texture_Synthesis", {"TexSynth1",
                               "TexSynth2", "TexSynth2", "TexSynth2", "TexSynth2", "TexSynth2", "TexSynth2", "TexSynth2", "TexSynth2", "TexSynth2",
                               "TexSynth3", "TexSynth3", "TexSynth3", "TexSynth3",
                               "TexSynth4", "TexSynth4", "TexSynth4", "TexSynth4",
                               "TexSynth5"}},
        {"Robot_Localization", {"RobLoc"}},
        {"Robot_Localization_DIMM", {"RobLoc_dimm"}},
        {"Disparity_Map", {"DispMapCompSAD", "DispMapFindDisp", "DispMapIntegSum"}},
        {"EKF_SLAM", {"Jacobians", "SphericalCoords"}},
        {"EKF_SLAM_DIMM", {"Jacobians_dimm", "SphericalCoords_dimm"}},
        {"KNN", {"ManhattanDist", "MergeSort"}}
    };

  public:
    Profiler* m_profiler;
    bool m_warmup_enabled;
    bool m_cooldown_enabled;
    CacheRecorder* m_cache_recorder;
};

class RubyStatsCallback : public Callback
{
  private:
    RubySystem *ruby_system;

  public:
    virtual ~RubyStatsCallback() {}
    RubyStatsCallback(RubySystem *system) : ruby_system(system) {}
    void process() { ruby_system->collateStats(); }
};

#endif // __MEM_RUBY_SYSTEM_SYSTEM_HH__
