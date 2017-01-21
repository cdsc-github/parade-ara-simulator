/*
  scratch-pad.c - sample code for a Simics device

  Copyright (C) 1998-2005 Virtutech AB, All Rights Reserved

  This program is a component ("Component") of Virtutech Simics and is
  being distributed under Section 1(a)(iv) of the Virtutech Simics
  Software License Agreement (the "Agreement").  You should have
  received a copy of the Agreement with this Component; if not, please
  write to Virtutech AB, Norrtullsgatan 15, 1tr, SE-113 27 STOCKHOLM,
  Sweden for a copy of the Agreement prior to using this Component.

  By using this Component, you agree to be bound by all of the terms of
  the Agreement.  If you do not agree to the terms of the Agreement, you
  may not use, copy or otherwise access the Component or any derivatives
  thereof.  You may create and use derivative works of this Component
  pursuant to the terms the Agreement provided that any such derivative
  works may only be used in conjunction with and as a part of Virtutech
  Simics for use by an authorized licensee of Virtutech.

  THIS COMPONENT AND ANY DERIVATIVES THEREOF ARE PROVIDED ON AN "AS IS"
  BASIS.  VIRTUTECH MAKES NO WARRANTIES WITH RESPECT TO THE COMPONENT OR
  ANY DERIVATIVES THEREOF AND DISCLAIMS ALL IMPLIED WARRANTIES,
  INCLUDING, WITHOUT LIMITATION, WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.

*/

#include <string.h>
#include <cassert>
#include <iostream>
#include "scratch-pad.hh"
#include "../MsgLogger/MsgLogger.hh"

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

void
read_sp(ScratchPadHandle* obj, uint64_t address, void* rd, unsigned int size)
{
  ScratchPadHandle* sh = (ScratchPadHandle*)obj;
  assert(sh);
  assert(sh->sp);
  ScratchPad* s = sh->sp;
  assert(rd);
  uint8_t* buf = (uint8_t*) rd;
  s->CheckSize(address + size);

  for (unsigned int i = 0; i < size; i++) {
    if (!(s->valid[address + i])) {
      buf[i] = s->data[address + i];
    } else {
      buf[i] = s->data[address + i];
    }
  }

  // ML_LOG("g_spm", "read addr 0x" << std::hex << address << " size: " << size);
}

void
write_sp(ScratchPadHandle* obj, uint64_t address, const void* wr, unsigned int size)
{
  assert(wr);
  assert(obj);
  ScratchPadHandle* sh = (ScratchPadHandle*)obj;
  assert(sh->sp);
  ScratchPad* s = sh->sp;
  s->CheckSize(address + size);
  uint8_t* buf = (uint8_t*)wr;

  for (unsigned int i = 0; i < size; i++) {
    s->valid[address + i] = true;
    s->data[address + i] = buf[i];
  }

  // ML_LOG("g_spm", "write addr 0x" << std::hex << address << " size: " << size);
}

void
clear_sp(ScratchPadHandle* obj)
{
  assert(obj);
  ScratchPadHandle* sh = (ScratchPadHandle*)obj;
  assert(sh->sp);
  ScratchPad* s = sh->sp;
  s->Clear();
}

scratch_pad_interface_t*
CreateScratchPadInterface()
{
  scratch_pad_interface_t *scratch_pad_interface =
    (scratch_pad_interface_t*)malloc(sizeof(scratch_pad_interface_t));

  memset(scratch_pad_interface, 0, sizeof(scratch_pad_interface_t));
  scratch_pad_interface->read = read_sp;
  scratch_pad_interface->write = write_sp;
  scratch_pad_interface->clear = clear_sp;
  return scratch_pad_interface;
}
