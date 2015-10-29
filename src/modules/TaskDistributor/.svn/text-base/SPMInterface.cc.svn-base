#include "SPMInterface.hh"
#include "gem5Interface.hh"
#include <cassert>
#include "mem/ruby/common/Global.hh"
#include <iostream>

SPMInterface::SPMInterface(int identifier, int size)
{
	id = identifier;
	num_reads = 0;
	num_writes = 0;
	std::cout << "TD:SPMInterface using g_TdSpmObject " << id << std::endl;
	spmObject = g_TdSpmObject[id];
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
