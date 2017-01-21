#ifndef DMACONTROLLER_H
#define DMACONTROLLER_H

#include <stdint.h>
#include <map>
#include <list>
#include <vector>
#include <string>
#include "SPMInterface.hh"
#include "NetworkInterface.hh"
#include "modules/Common/BaseCallbacks.hh"
#include "modules/linked-prefetch-tile/prefetcher-tile.hh"
#include "modules/MsgLogger/MsgLogger.hh"

class prftch_direct_interface;
typedef prftch_direct_interface prftch_direct_interface_t;

class TLBEntry
{
public:
  uint64_t vpBase;
  uint64_t ppBase;
  bool free;
  uint64_t mruTick;
  TLBEntry() : vpBase(0), ppBase(0), free(true), mruTick(0) {}
  void setMRU()
  {
    mruTick = GetSystemTime();
  };
};

class BaseTLBMemory
{
public:
  virtual bool lookup(uint64_t vp_base, uint64_t& pp_base, bool set_mru = true) = 0;
  virtual void insert(uint64_t vp_base, uint64_t pp_base) = 0;
  virtual void flushAll() = 0;
};

class TLBMemory : public BaseTLBMemory
{
  int numEntries;
  int sets;
  int ways;

  TLBEntry **entries;

protected:
  TLBMemory() {}

public:
  TLBMemory(int _numEntries, int associativity) :
    numEntries(_numEntries), sets(associativity)
  {
    if (sets == 0) {
      sets = numEntries;
    }

    assert(numEntries % sets == 0);
    ways = numEntries / sets;
    entries = new TLBEntry*[ways];

    for (int i = 0; i < ways; i++) {
      entries[i] = new TLBEntry[sets];
    }
  }
  virtual ~TLBMemory()
  {
    for (int i = 0; i < sets; i++) {
      delete [] entries[i];
    }

    delete [] entries;
  }

  virtual bool lookup(uint64_t vp_base, uint64_t& pp_base, bool set_mru = true);
  virtual void insert(uint64_t vp_base, uint64_t pp_base);
  virtual void flushAll();
};

class InfiniteTLBMemory : public BaseTLBMemory
{
  std::map<uint64_t, uint64_t> entries;
public:
  InfiniteTLBMemory() {}
  ~InfiniteTLBMemory() {}

  bool lookup(uint64_t vp_base, uint64_t& pp_base, bool set_mru = true)
  {
    auto it = entries.find(vp_base);

    if (it != entries.end()) {
      pp_base = it->second;
      return true;
    } else {
      pp_base = 0;
      return false;
    }
  }
  void insert(uint64_t vp_base, uint64_t pp_base)
  {
    entries[vp_base] = pp_base;
  }
  void flushAll() {}
};

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
  std::vector<SignalEntry> remoteSignals;
  std::vector<SignalEntry> localSignals;

  DMAEngineHandle* dmaDevice;
  prftch_direct_interface_t* dmaInterface;
  SPMInterface* spm;
  NetworkInterface* network;
  Arg1CallbackBase<uint64_t>* onAccViolation;
  Arg1CallbackBase<uint64_t>* onTLBMiss;
  // std::map<uint64_t, uint64_t> tlbMap;
  void OnAccessError(uint64_t logicalAddr);
  void OnNetworkMsg(int src, const void* buffer, unsigned int bufSize);

  typedef Arg1MemberCallback<DMAController, uint64_t, &DMAController::OnAccessError> OnAccessErrorCB;
  typedef Arg3MemberCallback<DMAController, int, const void*, unsigned int, &DMAController::OnNetworkMsg> OnNetworkMsgCB;
public:
  DMAController(NetworkInterface* ni, SPMInterface* spmInterface, Arg1CallbackBase<uint64_t>* TLBMiss, Arg1CallbackBase<uint64_t>* accessViolation);
  ~DMAController();

  void BeginTransfer(int srcSpm, uint64_t srcAddr, const std::vector<unsigned int>& srcSize, const std::vector<int>& srcStride, int dstSpm, uint64_t dstAddr, const std::vector<unsigned int>& dstSize, const std::vector<int>& dstStride, size_t elementSize, CallbackBase* finishedCB);
  void BeginTransfer(int srcSpm, uint64_t srcAddr, const std::vector<unsigned int>& srcSize, const std::vector<int>& srcStride, int dstSpm, uint64_t dstAddr, const std::vector<unsigned int>& dstSize, const std::vector<int>& dstStride, size_t elementSize, int priority, CallbackBase* finishedCB);

  void BeginSingleElementTransfer(int srcSpm, uint64_t src, int dstSpm, uint64_t dst, uint32_t size, CallbackBase* finishedCB);
  void BeginSingleElementTransfer(int srcSpm, uint64_t src, int dstSpm, uint64_t dst, uint32_t size, int priority, CallbackBase* finishedCB);

  void FlushTLB();
  void AddTLBEntry(uint64_t vAddr, uint64_t pAddr);

  inline std::string GetDeviceName()
  {
    char s[100];
    sprintf(s, "taskdistributor.dmacontroller.%02d", network->GetNodeID());
    return s;
  }

protected:
  // Access Stats
  uint64_t hits;
  uint64_t mshrhits;
  uint64_t misses;
  uint64_t flushTlb;

public:
  // private TLB entries
  int numEntries;

  int associativity;

  int hitLatency;

  BaseTLBMemory *tlbMemory;

  std::map<uint64_t, std::list<TransferData*> > MSHRs;

  void beginTranslateTiming(TransferData* td);

  void translateTiming(TransferData* td);

  void finishTranslation(uint64_t vp_base, uint64_t pp_base);

  void flushAll();

  typedef Arg1MemberCallback<DMAController, TransferData*, &DMAController::beginTranslateTiming> beginTranslateTimingCB;
  typedef MemberCallback1<DMAController, TransferData*, &DMAController::translateTiming> translateTimingCB;

  uint64_t getTlbHits()
  {
    return hits + mshrhits;
  }
  uint64_t getTlbMisses()
  {
    return misses;
  }
  uint64_t getTlbFlush()
  {
    return flushTlb;
  }

};

#endif
