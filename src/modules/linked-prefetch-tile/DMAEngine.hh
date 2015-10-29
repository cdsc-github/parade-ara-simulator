#ifndef DMA_ENGINE_H
#define DMA_ENGINE_H

#include <string>
#include <cassert>
#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include "gem5Interface.hh"
#include "../Common/BaseCallbacks.hh"
#include "../Common/PolyhedralAddresser.hh"
#include "../Common/MemoryDeviceInterface.hh"
#include "../scratch-pad/scratch-pad.hh"
#include "mem/protocol/RubyRequestType.hh"

class TransferEndPointDescription
{
public:
	int spm;
	PolyhedralAddresser addrGen;
	TransferEndPointDescription(int spm, uint64_t base, const std::vector<uint32_t>& size, const std::vector<int32_t>& stride);
};
class TransferData
{
public:
	uint64_t srcLAddr;
	int srcSpmID;
	uint64_t dstLAddr;
	int dstSpmID;
	size_t elementSize;
	int priority;
	uint64_t timeStamp;
	int buffer;
	CallbackBase* onFinish;
	TransferData(int srcSpmID, uint64_t srcLAddr, int dstSpmID, uint64_t dstLAddr, size_t elemSize, int prio, uint64_t when, int buffer, CallbackBase* onFinish);
	bool operator < (const TransferData& x) const
	{
		if(priority != x.priority)
		{
			return priority < x.priority;
		}
		else
		{
			return timeStamp < x.timeStamp;
		}
	}
	
};
class TransferSetDesc
{
	unsigned int index;
	std::queue<TransferData> awaitingReissue;
	TransferEndPointDescription start;
	TransferEndPointDescription end;
	size_t transferSize;
	CallbackBase* onFinish;
	uint32_t pendingAccesses;
	bool finished;
	uint64_t timeStamp;
	int buffer;
	void DecrementPending();
	typedef MemberCallback0<TransferSetDesc, &TransferSetDesc::DecrementPending> DecrementPendingCB;
public:
	int priority;
	TransferSetDesc(int spmSrc, uint64_t baseSrc, const std::vector<uint32_t>& sizeSrc, const std::vector<int32_t>& strideSrc, int spmDst, uint64_t baseDst, const std::vector<uint32_t>& sizeDst, const std::vector<int32_t>& strideDst, size_t transferSize, int prio, uint64_t when, int buffer, CallbackBase* finish);
	bool MoreTransfers() const;
	TransferData* NextTransfer() ;
	bool IsFinished() const;
	bool operator < (const TransferSetDesc& x) const;
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
	uint64_t lastEmit;
        MeteredMemory_Interface::gem5Interface* memObject;
	MemoryDeviceInterface* memInterface;
	Arg1CallbackBase<uint64_t>* onTLBMiss;
	Arg1CallbackFunctionBase<uint64_t, uint64_t>* translateAddress;
	Arg1CallbackBase<uint64_t>* onError;
	int nodeID;
	int spmID;
	std::map<uint64_t, uint64_t> emitTime; // key address, value cycle time
	scratch_pad_interface_t* spmInterface;
        ScratchPadHandle* spm;
	std::map<uint64_t, std::vector<TransferData*> > waitingOnTLB; // key logical addr, value transfer set
	std::map<uint64_t, CallbackBase*> pendingReads;//key physical addr, value CB to mark completion & delete transfer data
	std::map<uint64_t, CallbackBase*> pendingWrites;//key physical addr, value CB to mark completion & delete transfer data
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
	bool TranslateAddress(logical_address_t la, physical_address_t& pa);
	bool EnqueueTransfer(TransferData* td);
	void ReEnqueueTransfer(TransferData* td);
	void TryTransfers();
	void OnMemoryResponse(uint64_t addr, uint64_t emitTime);
	void WriteBlock(uint64_t spmAddr, uint64_t pMemAddr, uint64_t lMemAddr, size_t size);
	void ReadBlock(uint64_t pMemAddr, uint64_t lMemAddr, uint64_t spmAddr, size_t size);
	void Splice(CallbackBase* cb1, CallbackBase* cb2);
	typedef MemberCallback2<DMAEngine, CallbackBase*, CallbackBase*, &DMAEngine::Splice> SpliceCB;
	typedef MemberCallback2<DMAEngine, uint64_t, uint64_t, &DMAEngine::OnMemoryResponse> OnMemoryResponseCB;
	typedef MemberCallback1<DMAEngine, TransferData*, &DMAEngine::ReEnqueueTransfer> ReEnqueueTransferCB;
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
	void Configure(int node, int spxm, Arg1CallbackBase<uint64_t>* onTLBMiss, Arg1CallbackFunctionBase<uint64_t, uint64_t>* translateAddress, Arg1CallbackBase<uint64_t>* onError);
	void Restart();
};

#endif
