#ifndef DMACONTROLLER_H
#define DMACONTROLLER_H

#include <stdint.h>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include "../Common/BaseCallbacks.hh"
#include "SPMInterface.hh"
#include "NetworkInterface.hh"
#include "modules/linked-prefetch-tile/prefetcher-tile.hh"
class prftch_direct_interface;
typedef prftch_direct_interface prftch_direct_interface_t;

class DMAController
{
  class SignalEntry
  {
  public:
    int ID;
    uint64_t dstAddr;
    uint64_t requestedAddr;
    unsigned int size;
    CallbackBase* onFinish;
    bool Matches(const SignalEntry& s)
    {
      return s.ID == ID && s.dstAddr == dstAddr && s.requestedAddr == requestedAddr && s.size == size;
    }
  };
  uint64_t GetPageAddr(uint64_t addr) const;
  uint64_t OnAddressTranslate(uint64_t addr);
  std::vector<SignalEntry> remoteSignals;
  std::vector<SignalEntry> localSignals;
  std::set<uint64_t> pendingTLBMisses;
  DMAEngineHandle* dmaDevice;
  prftch_direct_interface_t* dmaInterface;
  SPMInterface* spm;
  NetworkInterface* network;
  Arg1CallbackBase<uint64_t>* onAccViolation;
  Arg1CallbackBase<uint64_t>* onTLBMiss;
  std::map<uint64_t, uint64_t> tlbMap;
  void OnTLBMiss(uint64_t logicalAddr);
  void OnAccessError(uint64_t logicalAddr);
  void OnNetworkMsg(int src, const void* buffer, unsigned int bufSize);
  typedef Arg1MemberCallbackFunction<DMAController, uint64_t, uint64_t, &DMAController::OnAddressTranslate> OnAddressTranslateCB;
  typedef Arg1MemberCallback<DMAController, uint64_t, &DMAController::OnAccessError> OnAccessErrorCB;
  typedef Arg1MemberCallback<DMAController, uint64_t, &DMAController::OnTLBMiss> OnTLBMissCB;
  typedef Arg3MemberCallback<DMAController, int, const void*, unsigned int, &DMAController::OnNetworkMsg> OnNetworkMsgCB;
public:
  DMAController(NetworkInterface* ni, SPMInterface* spmInterface, Arg1CallbackBase<uint64_t>* TLBMiss, Arg1CallbackBase<uint64_t>* accessViolation);
  ~DMAController();

  void BeginTransfer(int srcSpm, uint64_t srcAddr, const std::vector<unsigned int>& srcSize, const std::vector<int>& srcStride, int dstSpm, uint64_t dstAddr, const std::vector<unsigned int>& dstSize, const std::vector<int>& dstStride, size_t elementSize, CallbackBase* finishedCB);
  void BeginTransfer(int srcSpm, uint64_t srcAddr, const std::vector<unsigned int>& srcSize, const std::vector<int>& srcStride, int dstSpm, uint64_t dstAddr, const std::vector<unsigned int>& dstSize, const std::vector<int>& dstStride, size_t elementSize, int priority, CallbackBase* finishedCB);

  void BeginSingleElementTransfer(int srcSpm, uint64_t src, int dstSpm, uint64_t dst, uint32_t size, CallbackBase* finishedCB);
  void BeginSingleElementTransfer(int srcSpm, uint64_t src, int dstSpm, uint64_t dst, uint32_t size, int priority, CallbackBase* finishedCB);

  void FlushTLB();
  void AddTLBEntry(uint64_t logical, uint64_t physical);
};

#endif
