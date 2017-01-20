#include "SPMInterface.hh"
#include "SimicsInterface.hh"
#include <cassert>
#include "mem/ruby/common/Global.hh"
#include <iostream>

SPMInterface::SPMInterface(int identifier, int size)
{
    id = identifier;
    num_reads = 0;
    num_writes = 0;
    spmObject = g_TdSpmObject[id]; //TODO: replace with id or this pointer.
    assert(spmObject);
    spmInterface = g_spmInterface;
    assert(spmInterface);
}
SPMInterface::~SPMInterface()
{
    free(spmInterface);
}
void SPMInterface::Write(unsigned int addr, unsigned int size, void* buffer)
{
    num_writes++;
    spmInterface->write(spmObject, addr, buffer, size);
}
void SPMInterface::Read(unsigned int addr, unsigned int size, void* buffer)
{
    num_reads++;
    spmInterface->read(spmObject, addr, buffer, size);
}
