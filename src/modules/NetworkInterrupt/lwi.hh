#ifndef LWI_MESSAGING_H
#define LWI_MESSAGING_H

#include <stdint.h>
#include <iostream>
#define SIM_ENHANCE
#include "../../mem/ruby/common/TypeDefines.hh"
#ifdef SIM_ENHANCE
class LWI_StoredMessage
{
public:
  std::vector<uint8_t> packet;
  int thread;
  int lcacc;
  LWI_StoredMessage() {}
  LWI_StoredMessage(int t, int acc, const void* args, int size)
  {
    const uint8_t* buf = (const uint8_t*)args;

    for (int i = 0; i < size; i++) {
      packet.push_back(buf[i]);
    }

    thread = t;
    lcacc = acc;
  }
};
class LWI_MessageAccepter
{
public:
  int thread;
  int lcacc;
  std::vector<physical_address_t> pa_args;
  std::vector<logical_address_t> la_args;
  LWI_MessageAccepter() {}
  LWI_MessageAccepter(int t, int acc, physical_address_t pa, logical_address_t la)
  {
    assert(pa);
    assert(la);
    thread = t;
    lcacc = acc;
    pa_args.push_back(pa);
    la_args.push_back(la);
  }
  void Extend(physical_address_t pa, logical_address_t la, int index)
  {
    assert(pa_args.size() == index);
    assert(pa_args.size() == la_args.size());
    assert(la_args[la_args.size() - 1] + 1 == la);
    assert(la);
    assert(pa);
    assert((la - (index % 4)) % 4 == 0); //words are word aligned
    assert((pa - (index % 4)) % 4 == 0); //words are word aligned

    if (pa != pa_args[pa_args.size() - 1] + 1) {
      std::cout << "interrupt arg set spans TLB page boundary on " << index << std::endl;
      assert(index % 4 == 0);
    }

    pa_args.push_back(pa);
    la_args.push_back(la);
  }
};
logical_address_t LWI_FetchLogicalAddr(int thread, int index);
void LWI_RegisterAccepter(int thread, int lcacc, physical_address_t pa, logical_address_t la, int index);
void LWI_UnregisterAccepter(int thread, int lcacc);
bool LWI_GetMessagePair(int thread, LWI_StoredMessage& msg, LWI_MessageAccepter& accepter);
void LWI_EndInterruptHandling(int thread);
#endif

#endif
