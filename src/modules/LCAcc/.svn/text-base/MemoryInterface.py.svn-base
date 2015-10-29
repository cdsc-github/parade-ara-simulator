from m5.params import *
from m5.proxy import *
from MemObject import MemObject

class MemoryInterface(MemObject):
    type = 'MemoryInterface'
    cxx_header = "modules/LCAcc/memInterface.hh"

    master_port = MasterPort("Accelerator Master Port")
    system = Param.System(Parent.any, "system")
