#ifndef DMA_ENGINE_H
#define DMA_ENGINE_H

#include <string>
#include <cassert>
#include <map>
#include <vector>
#include <queue>
#include "SimicsInterface.hh"
#include "../Common/BaseCallbacks.hh"
#include "../Common/PolyhedralAddresser.hh"
#include "../Common/MemoryDeviceInterface.hh"

#include "../scratch-pad/scratch-pad.hh"
#include "mem/protocol/RubyRequestType.hh"

#define BLOCK_SIZE 64
#define NO_SPM_ID -1

class TransferEndPointDescription
{
public:
  int spm;
  PolyhedralAddresser addrGen;
  TransferEndPointDescription(int spm, uint64_t base,
                              const std::vector<uint32_t>& size, const std::vector<int32_t>& stride)
    : spm(spm), addrGen(base, size, stride) {}
};

class TransferData
{
public:
  int srcSpmID;
  int dstSpmID;

  uint64_t srcLAddr;
  uint64_t dstLAddr;

  uint64_t pAddr;

  size_t elementSize;
  int priority;
  uint64_t timeStamp;
  int buffer;

  CallbackBase* onFinish;

  TransferData(int _srcSpmID, uint64_t _srcLAddr, int _dstSpmID,
               uint64_t _dstLAddr, size_t elemSize, int prio, uint64_t when,
               int _buffer, CallbackBase* _onFinish)
    : srcSpmID(_srcSpmID), dstSpmID(_dstSpmID), srcLAddr(_srcLAddr),
      dstLAddr(_dstLAddr), elementSize(elemSize), priority(prio),
      timeStamp(when), buffer(_buffer), onFinish(_onFinish) {}

  bool isRead()
  {
    return (srcSpmID == NO_SPM_ID);
  }

  void setPaddr(uint64_t paddr)
  {
    pAddr = paddr;
  }

  uint64_t getPaddr()
  {
    return pAddr;
  }

  uint64_t getVaddr()
  {
    return (srcSpmID == NO_SPM_ID) ? srcLAddr : dstLAddr;
  }

  bool operator < (const TransferData& x) const
  {
    if (priority != x.priority) {
      return priority < x.priority;
    } else {
      return timeStamp < x.timeStamp;
    }
  }

};

class TransferSetDesc
{
public:
  size_t transferSize;

  int priority;

  uint64_t timeStamp;
  int buffer;

  CallbackBase* onFinish;

  TransferEndPointDescription start;
  TransferEndPointDescription end;

  unsigned int index;
  size_t pendingSize;

  TransferSetDesc(int spmSrc, uint64_t baseSrc,
                  const std::vector<uint32_t>& sizeSrc,
                  const std::vector<int32_t>& strideSrc, int spmDst, uint64_t baseDst,
                  const std::vector<uint32_t>& sizeDst,
                  const std::vector<int32_t>& strideDst, size_t _transferSize,
                  int prio, uint64_t when, int _buffer, CallbackBase* finish)
    : transferSize(_transferSize), priority(prio), timeStamp(when),
      buffer(_buffer), onFinish(finish),
      start(spmSrc, baseSrc, sizeSrc, strideSrc),
      end(spmDst, baseDst, sizeDst, strideDst)
  {
    index = 0;
    pendingSize = 0;
  }

  void DecrementPending(size_t finishSize);

  typedef MemberCallback1<TransferSetDesc, size_t,
          &TransferSetDesc::DecrementPending> DecrementPendingCB;

  bool MoreTransfers() const
  {
    return start.addrGen.TotalSize() > index;
  }

  TransferData* NextTransfer();

  bool operator < (const TransferSetDesc& x) const
  {
    if (priority != x.priority) {
      return priority < x.priority;
    } else {
      return timeStamp < x.timeStamp;
    }
  }
};

class DMAEngine
{
  template <class T> class PtrLess
  {
  public:
    bool operator() (const T* x, const T* y) const
    {
      return *x < *y;
    }
  };

  uint32_t inflight;
  uint32_t issueWidth;
  uint64_t lastEmit;
  bool scheduled;

  Arg1CallbackBase<uint64_t>* onError;
  Arg1CallbackBase<TransferData*>* beginTranslateTiming;

  int nodeID;
  int spmID;
  ScratchPadHandle* spm;
  scratch_pad_interface_t* spmInterface;
  MeteredMemory_Interface::SimicsInterface* memObject;
  MemoryDeviceInterface* memInterface;

  // key address, value cycle time
  std::map<uint64_t, uint64_t> emitTime;
  // key physical addr, value CB to mark completion & delete transfer data
  std::map<uint64_t, CallbackBase*> pendingReads;
  // key physical addr, value CB to mark completion & delete transfer data
  std::map<uint64_t, CallbackBase*> pendingWrites;

  std::priority_queue<TransferData*, std::vector<TransferData*>, PtrLess<TransferData> > waitingTransfers;
  std::priority_queue<TransferSetDesc*, std::vector<TransferSetDesc*>, PtrLess<TransferSetDesc> > waitingTransferSets;

  template<class T> T AddrRound(T addr, int mod)
  {
    return (addr / (T)mod) * (T)mod;
  }
  //adding redirects for port retargetting
  bool IsRedirectingToMemory() const;
  bool IsReady(uint64_t lAddr, uint64_t pAddr, RubyRequestType direction);
  bool IsBufferReady(int bufferId, uint64_t addr, uint64_t dstAddr, RubyRequestType direction);
  void MakeRequest(uint64_t lAddr, uint64_t pAddr, RubyRequestType direction, CallbackBase* cb);
  void MakePrefetch(uint64_t lAddr, uint64_t pAddr);
  void MakeBufferCopy(uint64_t cacheAddr, uint64_t bufferAddr, int bufferId, RubyRequestType direction, CallbackBase* cb);
  void MemDevInterfaceIntercept(const void*, CallbackBase* cb);
  typedef Stored1Arg1MemberCallback<DMAEngine, const void*, CallbackBase*, &DMAEngine::MemDevInterfaceIntercept> MemDevInterfaceInterceptCB;
  //end redirects for port retargetting
  void reFinishTranslation(TransferData* td);
  void TryTransfers();
  void OnMemoryResponse(uint64_t addr, uint64_t emitTime);
  void WriteBlock(uint64_t spmAddr, uint64_t pMemAddr, uint64_t lMemAddr, size_t size);
  void ReadBlock(uint64_t pMemAddr, uint64_t lMemAddr, uint64_t spmAddr, size_t size);
  void Splice(CallbackBase* cb1, CallbackBase* cb2);
  typedef MemberCallback2<DMAEngine, CallbackBase*, CallbackBase*, &DMAEngine::Splice> SpliceCB;
  typedef MemberCallback2<DMAEngine, uint64_t, uint64_t, &DMAEngine::OnMemoryResponse> OnMemoryResponseCB;
  typedef MemberCallback1<DMAEngine, TransferData*, &DMAEngine::reFinishTranslation> reFinishTranslationCB;
  typedef MemberCallback4<DMAEngine, uint64_t, uint64_t, uint64_t, size_t, &DMAEngine::WriteBlock> WriteBlockCB;
  typedef MemberCallback4<DMAEngine, uint64_t, uint64_t, uint64_t, size_t, &DMAEngine::ReadBlock> ReadBlockCB;
  typedef MemberCallback0<DMAEngine, &DMAEngine::TryTransfers> TryTransfersCB;

public:
  void AddTransferSet(int srcDevice, uint64_t srcAddr, unsigned int srcDimensions, const unsigned int* srcElementSize, const int* srcElementStride, int dstDevice, uint64_t dstAddr, unsigned int dstDimensions, const unsigned int* dstElementSize, const int* dstElementStride, size_t transferSize, int priority, int buffer, CallbackBase* onFinish);
  void PrefetchSet(uint64_t addr, unsigned int dimensions, const unsigned int* elementSize, const int* elementStride, size_t transferSize);
  void AddSingleTransfer(int srcDevice, uint64_t srcAddr, int dstDevice, uint64_t dstAddr, size_t transferSize, int priority, int buffer, CallbackBase* onFinish);
  void HookToMemoryPort(const char* deviceName);
  void UnhookMemoryPort();
  DMAEngine();
  ~DMAEngine();
  void Configure(int node, int spxm, Arg1CallbackBase<TransferData*>* beginTranslateTiming, Arg1CallbackBase<uint64_t>* onError);

  void finishTranslation(TransferData* td);

  bool transferLeft();
};

#endif
