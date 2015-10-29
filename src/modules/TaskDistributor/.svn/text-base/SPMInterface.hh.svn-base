#ifndef SPMINTERFACE_H
#define SPMINTERFACE_H

#include <stdint.h>

class scratch_pad_interface;
typedef scratch_pad_interface scratch_pad_interface_t;
#include "../scratch-pad/scratch-pad.hh"

class SPMInterface
{
	int id;
        uint64_t num_reads;
        uint64_t num_writes;
	ScratchPadHandle* spmObject;
	scratch_pad_interface_t* spmInterface;
public:
	SPMInterface(int id, int size);
	~SPMInterface();
	void Write(unsigned int addr, unsigned int size, void* buffer);
	void Read(unsigned int addr, unsigned int size, void* buffer);
	int GetID(){ return id; }
        uint64_t get_num_spm_reads() {return num_reads;}
        uint64_t get_num_spm_writes() {return num_writes;}
};

#endif
