import math
import m5
from m5.objects import *
from m5.defines import buildEnv
from Ruby import send_evicts

acc_types = [
    'Deblur_Modified',
    'Denoise',
    'Registration_Modified',
    'Segmentation',
    'BlackScholes',
    'StreamCluster',
    'Swaptions',
    'LPCIP_Desc',
    # 'SURF_Desc',
    'Texture_Synthesis',
    'Robot_Localization',
    'Disparity_Map',
    'EKF_SLAM'
]

def get(name):
    if name in acc_types:
        return name
    else:
        print >>sys.stderr, "invalid accelerator type '%s'" % name
        sys.exit(1)

def define_options(parser):
    parser.add_option("--num_accinstances", action="store", type="int",
                      default=1,
                      help="number of accelerator instances in the system.")
    # parser.add_option("--accelerators", action="store", type="int", default=2,
    #                   help="number of accelerators in the system.")
    parser.add_option("--num_tds", action="store", type="int", default=1,
                      help="number of task distributors")
    parser.add_option("--num_networkports", action="store", type="int", default=32,
                      help="number of simics network ports")
    parser.add_option("--visual_trace", action="store", type="string", default="parade-test/visual.txt",
                      help="file name for visualization trace")
    parser.add_option("--td_tlb_size", action="store", type="int", default=64,
                      help="number of TD TLB entries")
    parser.add_option("--lcacc_tlb_size", action="store", type="int", default=16,
                      help="number of LCAcc TLB entries")
    parser.add_option("--tlb_hack", action="store_true",
                      help="touch pages before accelerator execution")
    parser.add_option("--host_ptw_latency", action="store", type="int", default=1,
                      help="latency to trigger host page table walk")
    parser.add_option("--ideal_mmu", action="store_true",
                      help="impossible to implement mmu with minimal latency")
    parser.add_option("--acc_types", metavar="ACC[,ACC]", action="store",
                      type="string", default="BlackScholes",
                      help="accelerator types to be instantiated in the system")
    parser.add_option("--lcacc_tlb_mshr", action="store", type="int", default=1,
                      help="number of MSHRs in LCAcc TLBs")
    parser.add_option("--iommu", action="store_true",
                      help="simulate the behavior of an IOMMU")
    parser.add_option("--dma_issue_width", action="store", type="int", default=64,
                      help="LCAcc DMA issue width")
    parser.add_option("--td_tlb_latency", action="store", type="int", default=3,
                      help="TD TLB lookup latency")
    parser.add_option("--lcacc_tlb_latency", action="store", type="int", default=1,
                      help="LCAcc TLB lookup latency")


def create_accelerators(options, system):
    # For now, use same clock as cpus.
    accelerator = MemoryInterface(clk_domain=system.cpu_clk_domain)
    exec("system.accelerator = accelerator")
