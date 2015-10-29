import math
import m5
from m5.objects import *
from m5.defines import buildEnv
from Ruby import send_evicts

def define_options(parser):
    parser.add_option("--accelerators", action="store", type="int", default=2,
                      help="number of accelerators in the system.")
    parser.add_option("--num_tds", action="store", type="int", default=1,
                      help="number of task distributors")
    parser.add_option("--num_networkports", action="store", type="int", default=32,
                      help="number of simics network ports")
    parser.add_option("--visual_trace", action="store", type="string", default="parade-test/visual.txt",
                      help="file name for visualization trace")

def create_accelerators(options, system):
    # For now, use same clock as cpus.
    accelerator = MemoryInterface(clk_domain=system.cpu_clk_domain)
    exec("system.accelerator = accelerator")
