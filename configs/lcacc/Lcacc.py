import math
import m5
from m5.objects import *
from m5.defines import buildEnv
from Ruby import send_evicts

acc_types = [
    'Deblur_Modified',
    'Denoise',
    'Denoise_DIMM',
    'FFT_1D_DIMM',
    'FluidAnimate_DIMM',
    'Registration_DIMM',
    'Registration_Modified',
    'Segmentation',
    'Segmentation_DIMM',
    'BlackScholes',
    'BlackScholes_DIMM',
    'StreamCluster',
    'StreamCluster_DIMM',
    'Swaptions',
    'LPCIP_Desc',
    # 'SURF_Desc',
    'Texture_Synthesis',
    'Robot_Localization',
    'Robot_Localization_DIMM',
    'Disparity_Map',
    'EKF_SLAM',
    'EKF_SLAM_DIMM',
    'KNN',
    'MatMul',
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
                      help="number of accelerator instances in the system")
    parser.add_option("--num_pes", action="store", type="int", default=1,
                      help="number of accelerators sharing the same port")
    parser.add_option("--num_tds", action="store", type="int", default=1,
                      help="number of task distributors")
    parser.add_option("--num_networkports", action="store", type="int", default=32,
                      help="number of simics network ports")
    parser.add_option("--visual_trace", action="store", type="string", default="parade-test/visual.txt",
                      help="file name for visualization trace")
    parser.add_option("--acc_types", metavar="ACC[,ACC]", action="store",
                      type="string", default="BlackScholes",
                      help="accelerator types to be instantiated in the system")
    parser.add_option("--aim", action="store_true",
                      help="modeling accelerator-interposed memory")
    parser.add_option("--aim_mem_bandwidth", action="store", type="int", default=100,
                      help="aim memory object bandwidth in bytes per second")
    parser.add_option("--aim_mem_clock", action="store", type="int", default=100,
                      help="aim memory object clock normalized to cpu clock")
    parser.add_option("--aim_mem_latency", action="store", type="int", default=100,
                      help="aim memory object latency in cpu cycles")

def create_accelerators(options, system):
    # For now, use same clock as cpus.
    accelerator = MemoryInterface(clk_domain=system.cpu_clk_domain)
    exec("system.accelerator = accelerator")
