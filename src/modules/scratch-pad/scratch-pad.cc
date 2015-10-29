#include <string.h>
#include <cassert>
#include <iostream>
#include "scratch-pad.hh"

void read_sp(ScratchPadHandle* obj, uint64_t address, void* rd, unsigned int size);
void write_sp(ScratchPadHandle* obj, uint64_t address, const void* wr, unsigned int size);
void clear_sp(ScratchPadHandle* obj);

ScratchPadHandle* CreateNewScratchPad()
{
        ScratchPadHandle *s = (ScratchPadHandle*)malloc(sizeof(ScratchPadHandle));
	memset(s, 0, sizeof(ScratchPadHandle));
	s->sp = new ScratchPad();
	return s;
}
int DeleteScratchPad(ScratchPadHandle* obj)
{
	assert(obj);
	ScratchPadHandle* sh = (ScratchPadHandle*)obj;
	assert(sh->sp);
	delete sh->sp;
	return 0;
}
void read_sp(ScratchPadHandle* obj, uint64_t address, void* rd, unsigned int size)
{
	ScratchPadHandle* sh = (ScratchPadHandle*)obj;
	assert(sh);
	assert(sh->sp);
	ScratchPad* s = sh->sp;
	assert(rd);
	uint8_t* buf = (uint8_t*) rd;
	s->CheckSize(address + size);
	for(unsigned int i = 0; i < size; i++)
	{
		if(!(s->valid[address + i]))
		{
		  buf[i] = s->data[address+i];
		}
		else
		  buf[i] = s->data[address + i];
	}
}
void write_sp(ScratchPadHandle* obj, uint64_t address, const void* wr, unsigned int size)
{
	assert(wr);
	assert(obj);
        ScratchPadHandle* sh = (ScratchPadHandle*)obj;
	assert(sh->sp);
	ScratchPad* s = sh->sp;
	s->CheckSize(address + size);
        uint8_t* buf = (uint8_t*)wr;
        for(unsigned int i = 0; i < size; i++)
        {
                s->valid[address + i] = true;
                s->data[address + i] = buf[i];
        }
}
void clear_sp(ScratchPadHandle* obj)
{
	assert(obj);
	ScratchPadHandle* sh = (ScratchPadHandle*)obj;
	assert(sh->sp);
	ScratchPad* s = sh->sp;
	s->Clear();
}
scratch_pad_interface_t* CreateScratchPadInterface()
{
        scratch_pad_interface_t *scratch_pad_interface = (scratch_pad_interface_t*)malloc(sizeof(scratch_pad_interface_t));

        memset(scratch_pad_interface, 0, sizeof(scratch_pad_interface_t));
        scratch_pad_interface->read = read_sp;
        scratch_pad_interface->write = write_sp;
	scratch_pad_interface->clear = clear_sp;
	return scratch_pad_interface;
}
