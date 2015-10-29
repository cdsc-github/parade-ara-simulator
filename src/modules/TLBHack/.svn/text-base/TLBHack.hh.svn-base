#ifndef TLB_HACK_H
#define TLB_HACK_H

#include "../../mem/ruby/common/TypeDefines.hh"
#include "../../cpu/thread_context.hh"

class TLBHackInterface
{
public:
	bool (*PageKnown)(int thread, logical_address_t addr);
	physical_address_t (*Lookup)(int thread, logical_address_t addr);
        void (*AddEntry)( int thread, logical_address_t lAddr, physical_address_t pAddr);
};

void MagicHandler(void*, ThreadContext* cpu, integer_t op, int thread, logical_address_t lAddr);
bool PageKnownHandler(int thread, logical_address_t addr);
physical_address_t LookupHandler(int thread, logical_address_t addr);
void AddEntryHandler(int thread, logical_address_t lAddr, physical_address_t pAddr);

#endif
