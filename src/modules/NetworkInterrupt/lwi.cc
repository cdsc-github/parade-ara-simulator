#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include "lwi.hh"
#include "lwint.hh"
#include "modules/MsgLogger/MsgLogger.hh"
#include "base/misc.hh"

#ifdef SIM_ENHANCE
std::map<int, LWI_StoredMessage> LWI_pendingMessages; //key thread
std::set<int> LWI_inInterrupt; //key thread
std::map<int, std::map<int, LWI_MessageAccepter> > LWI_pendingAccepters; //key thread, second key lcacc
#endif
extern "C"
{
  int isReady(int thread)
  {
#ifdef SIM_ENHANCE
    return (LWI_pendingMessages.find(thread) == LWI_pendingMessages.end()) ? 1 : 0;
#else
    fatal("System was not compiled with SIM_ENHANCE\n");
#endif
  }
  void raiseLightWeightInt(int thread, void* args, int argSize, int interrupting_lcacc)
  {
#ifdef SIM_ENHANCE
    assert(isReady(thread));
    // std::cout << "Raised! for thread " << thread << " from lcacc " << interrupting_lcacc << std::endl;
    LWI_pendingMessages[thread] = LWI_StoredMessage(thread, interrupting_lcacc, args, argSize);
#else
    fatal("System was not compiled with SIM_ENHANCE\n");
#endif
  }
}
#ifdef SIM_ENHANCE
logical_address_t LWI_FetchLogicalAddr(int thread, int index)
{
  if (LWI_pendingMessages.find(thread) == LWI_pendingMessages.end()) {
    return 0;
  }

  if (LWI_pendingAccepters.find(thread) == LWI_pendingAccepters.end()) {
    return 0;
  }

  if (LWI_pendingAccepters[thread].find(LWI_pendingMessages[thread].lcacc) == LWI_pendingAccepters[thread].end()) {
    return 0;
  }

  if (LWI_inInterrupt.find(thread) != LWI_inInterrupt.end()) {
    return 0;
  }

  assert(LWI_pendingMessages.find(thread) != LWI_pendingMessages.end());
  assert(LWI_pendingAccepters[thread].find(LWI_pendingMessages[thread].lcacc) != LWI_pendingAccepters[thread].end());
  assert(LWI_pendingAccepters[thread][LWI_pendingMessages[thread].lcacc].la_args[0]);
  return LWI_pendingAccepters[thread][LWI_pendingMessages[thread].lcacc].la_args[0];
}
void LWI_RegisterAccepter(int thread, int lcacc, physical_address_t pa, logical_address_t la, int index)
{
  if (index == 0) {
    ML_LOG("lwi", "registering accepter for userthread " << thread);
    LWI_pendingAccepters[thread][lcacc] = LWI_MessageAccepter(thread, lcacc, pa, la);
  } else {
    LWI_pendingAccepters[thread][lcacc].Extend(pa, la, index);
  }
}
void LWI_UnregisterAccepter(int thread, int lcacc)
{
  assert(LWI_pendingAccepters.find(thread) != LWI_pendingAccepters.end());
  assert(LWI_pendingAccepters[thread].find(lcacc) != LWI_pendingAccepters[thread].end());
  LWI_pendingAccepters[thread].erase(lcacc);

  if (LWI_pendingAccepters[thread].empty()) {
    LWI_pendingAccepters.erase(thread);
  }
}
bool LWI_GetMessagePair(int thread, LWI_StoredMessage& msg, LWI_MessageAccepter& accepter)
{
  if (LWI_inInterrupt.find(thread) != LWI_inInterrupt.end()) {
    return false;
  }

  if (LWI_pendingMessages.find(thread) == LWI_pendingMessages.end()) {
    return false;
  }

  if (LWI_pendingAccepters.find(thread) == LWI_pendingAccepters.end()) {
    return false;
  }

  if (LWI_pendingAccepters[thread].find(LWI_pendingMessages[thread].lcacc) == LWI_pendingAccepters[thread].end()) {
    return false;
  }

  std::stringstream ss;
  ss << "opalthread_" << thread;
  ML_LOG(ss.str(), "BEGIN Interrupt Handling");//*/
  msg = LWI_pendingMessages[thread];
  accepter = LWI_pendingAccepters[thread][LWI_pendingMessages[thread].lcacc];
  LWI_inInterrupt.insert(thread);
  return true;
}
void LWI_EndInterruptHandling(int thread)
{
  //std::cout << "Ending interrupt handling on " << thread << std::endl;
  assert(LWI_pendingMessages.find(thread) != LWI_pendingMessages.end());
  assert(LWI_pendingAccepters.find(thread) != LWI_pendingAccepters.end());
  assert(LWI_pendingAccepters[thread].find(LWI_pendingMessages[thread].lcacc) != LWI_pendingAccepters[thread].end());
  assert(LWI_inInterrupt.find(thread) != LWI_inInterrupt.end());
  std::stringstream ss;
  ss << "opalthread_" << thread;
  ML_LOG(ss.str(), "END Interrupt Handling");//*/
  LWI_pendingMessages.erase(thread);
  LWI_inInterrupt.erase(thread);
}

#endif
