#ifndef DMACONTROLLER_H
#define DMACONTROLLER_H

#include <stdint.h>
#include <map>
#include <vector>
#include <set>
#include <iostream>
#include <string>
#include "../Common/BaseCallbacks.hh"
#include "SPMInterface.hh"
#include "NetworkInterface.hh"
#include "../linked-prefetch-tile/prefetcher-tile.hh"

namespace LCAcc
{
class DMAController
{
public:
  class AccessType
  {
  public:
    static const int Read;
    static const int Write;
    static const int ReadLock;
    static const int WriteLock;
    static const int WriteUnlock;
    static const int Unlock;
  };
private:
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
  DMAEngineHandle *dmaDevice;
  prftch_direct_interface_t* dmaInterface;
  SPMInterface* spm;
  NetworkInterface* network;
  Arg1CallbackBase<uint64_t>* onAccViolation;
  Arg1CallbackBase<uint64_t>* onTLBMiss;
  std::map<uint64_t, uint64_t> tlbMap;
  int buffer;
  void OnTLBMiss(uint64_t logicalAddr);
  void OnAccessError(uint64_t logicalAddr);
  void OnNetworkMsg(int src, const void* buffer, unsigned int bufSize);
  typedef Arg1MemberCallbackFunction<DMAController, uint64_t, uint64_t, &DMAController::OnAddressTranslate> OnAddressTranslateCB;
  typedef Arg1MemberCallback<DMAController, uint64_t, &DMAController::OnAccessError> OnAccessErrorCB;
  typedef Arg1MemberCallback<DMAController, uint64_t, &DMAController::OnTLBMiss> OnTLBMissCB;
  typedef Arg3MemberCallback<DMAController, int, const void*, unsigned int, &DMAController::OnNetworkMsg> OnNetworkMsgCB;
  bool isHookedToMemory;
public:
  DMAController(NetworkInterface* ni, SPMInterface* spmInterface, Arg1CallbackBase<uint64_t>* TLBMiss, Arg1CallbackBase<uint64_t>* accessViolation);
  ~DMAController();

  void BeginTransfer(int srcSpm, uint64_t srcAddr, const std::vector<unsigned int>& srcSize, const std::vector<int>& srcStride, int dstSpm, uint64_t dstAddr, const std::vector<unsigned int>& dstSize, const std::vector<int>& dstStride, size_t elementSize, CallbackBase* finishedCB);
  void BeginTransfer(int srcSpm, uint64_t srcAddr, const std::vector<unsigned int>& srcSize, const std::vector<int>& srcStride, int dstSpm, uint64_t dstAddr, const std::vector<unsigned int>& dstSize, const std::vector<int>& dstStride, size_t elementSize, int priority, CallbackBase* finishedCB);
  void PrefetchMemory(uint64_t baseAddr, const std::vector<unsigned int>& size, const std::vector<int>& stride, size_t elementSize);

  void BeginSingleElementTransfer(int mySPM, uint64_t src, uint64_t dst, uint32_t size, int type, CallbackBase* finishedCB);
  void SetBuffer(int buf);
  void FlushTLB();
  void AddTLBEntry(uint64_t logical, uint64_t physical);
  void HookToMemoryController(const std::string& deviceName);
};
}

#endif
