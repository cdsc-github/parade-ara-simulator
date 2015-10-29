#include <cassert>
#include <stdint.h>
#include <iostream>
#include <map>

#include "TLBHack.hh"
#include "arch/vtophys.hh"
#include "arch/isa_traits.hh"

#define SYSTEM_PAGE (TheISA::PageBytes)
#define Round(x, y) (((x) / (y)) * (y))

std::map<int, std::map<logical_address_t, physical_address_t> > largeTLB;

void MagicHandler(void*, ThreadContext* cpu, integer_t op, int thread, logical_address_t lAddr)
{
	if(op == 0xC101)
	{
		lAddr = Round(lAddr, SYSTEM_PAGE);
		//std::cout << "TLB Hack: vAddr = " << std::hex << lAddr << std::endl;
		if(largeTLB[thread].find(lAddr ) == largeTLB[thread].end())
		{
		        physical_address_t pAddr = TheISA::vtophys(cpu, lAddr);
			assert(pAddr % SYSTEM_PAGE == 0);
			std::cout << "TLB HACK: storing " << std::hex << lAddr << "->" << pAddr << " for thread " << thread << "(" << largeTLB[thread].size() * SYSTEM_PAGE << " bytes learned)" << std::endl;
			largeTLB[thread][lAddr] = pAddr;
		}
	}
}

bool PageKnownHandler(int thread, logical_address_t addr)
{
	addr = Round(addr, SYSTEM_PAGE);
	if(largeTLB[thread].find(addr) != largeTLB[thread].end())
	{
		return true;
	}
	return false;
}
physical_address_t LookupHandler(int thread, logical_address_t addr)
{
	logical_address_t pageAddr = Round(addr, SYSTEM_PAGE);
	assert(PageKnownHandler(thread, pageAddr));
std::cout << "Servicing TLB miss for thread " << thread << ", " << addr << "->" << largeTLB[thread][pageAddr] + (addr - pageAddr) << std::endl;
	return largeTLB[thread][pageAddr] + (addr - pageAddr);
}
void AddEntryHandler(int thread, logical_address_t lAddr, physical_address_t pAddr)
{
  lAddr = Round(lAddr, SYSTEM_PAGE);
  assert(largeTLB[thread].find(lAddr) == largeTLB[thread].end());
  pAddr = Round(pAddr, SYSTEM_PAGE);
  std::cout << "TLB HACK: storing " << std::hex << lAddr << "->" << pAddr << " for thread " << thread << "(" << largeTLB[thread].size() * SYSTEM_PAGE << " bytes learned)" << std::endl;
  largeTLB[thread][lAddr] = pAddr;
}
