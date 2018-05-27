#ifndef LCACC_HEADER_SECTION
#define LCACC_HEADER_SECTION
#include "SimicsHeader.h"
#include <stdint.h>
#include <cassert>
#include <vector>
#define LCACC_STATUS_TLB_MISS 45
#define LCACC_STATUS_COMPLETED 46
#define LCACC_STATUS_ERROR 3
#define LCACC_STATUS_PENDING 4
#define LCACC_GAM_WAIT 5
#define LCACC_GAM_GRANT 4
#define LCACC_GAM_ERROR 6
#define LCACC_GAM_REVOKE 7
#define LCACC_CMD_BEGIN_TASK 42
#define LCACC_CMD_CANCEL_TASK 43
#define LCACC_CMD_TLB_SERVICE 44
#define LCACC_CMD_BEGIN_TASK_SIGNATURE 47
#define LCACC_CMD_BEGIN_PROGRAM 50
#define BIN_CMD_ARBITRATE_RESPONSE 102
#define LWI_WAIT 2
#define LWI_PROCEED 1
#define LWI_ERROR 0
template <class From, class To>
class TypeConverter
{
public:
	union
	{
		From from;
		To to;
	};
};
template <class From, class To>
inline To ConvertToType(From f)
{
	TypeConverter<From, To> x;
	x.from = f;
	return x.to;
}
template <class From>
class ByteConverter
{
public:
	union
	{
		From from;
		uint8_t bytes[sizeof(From)];
	};
};
template <class From>
inline void ConvertToBytes(std::vector<uint8_t>& dst, size_t index, From f)
{
	ByteConverter<From> c;
	c.from = f;
	for(size_t i = 0; i < sizeof(From); i++)
	{
		assert(i + index < dst.size());
		dst[i + index] = c.bytes[i];
	}
}
class LCAccNode
{
	std::vector<std::vector<uint8_t>*> lcaccIDReplacementBuffer;
	std::vector<size_t> lcaccIDReplacementIndex;
	bool lcaccIDDecided;
public:
	uint32_t lcaccOpCode;
	uint32_t spmWindowSize;
	uint32_t spmWindowCount;
	uint32_t spmWindowOffset;
	uint32_t lcaccID;
	inline LCAccNode(uint32_t init_lcaccOpCode)
	{
		lcaccOpCode = init_lcaccOpCode;
		spmWindowSize = 0;
		spmWindowCount = 0;
		spmWindowOffset = 0;
		lcaccIDDecided = false;
		lcaccID = 0;
	}
	inline LCAccNode(uint32_t init_lcaccOpCode, uint32_t init_spmWindowSize, uint32_t init_spmWindowCount, uint32_t init_spmWindowOffset)
	{
		lcaccOpCode = init_lcaccOpCode;
		spmWindowSize = init_spmWindowSize;
		spmWindowCount = init_spmWindowCount;
		spmWindowOffset = init_spmWindowOffset;
		lcaccIDDecided = false;
		lcaccID = 0;
	}
	inline LCAccNode(uint32_t init_lcaccOpCode, uint32_t init_spmWindowSize, uint32_t init_spmWindowCount, uint32_t init_spmWindowOffset, uint32_t init_lcaccID)
	{
		lcaccOpCode = init_lcaccOpCode;
		spmWindowSize = init_spmWindowSize;
		spmWindowCount = init_spmWindowCount;
		spmWindowOffset = init_spmWindowOffset;
		lcaccIDDecided = true;
		lcaccID = init_lcaccID;
	}
	inline void SetSPMConfig(uint32_t init_spmWindowSize, uint32_t init_spmWindowCount, uint32_t init_spmWindowOffset)
	{
		spmWindowSize = init_spmWindowSize;
		spmWindowCount = init_spmWindowCount;
		spmWindowOffset = init_spmWindowOffset;
	}
	inline void PlaceLCAccID(std::vector<uint8_t>& buffer, size_t index)
	{
		if(lcaccIDDecided)
		{
			ConvertToBytes<uint32_t>(buffer, index, lcaccID);
		}
		lcaccIDReplacementBuffer.push_back(&buffer);
		lcaccIDReplacementIndex.push_back(index);
	}
	inline void SetLCAccID(uint32_t init_lcaccID)
	{
		simics_assert(!lcaccIDDecided);
		lcaccIDDecided = true;
		lcaccID = init_lcaccID;
		for(size_t i = 0; i < lcaccIDReplacementBuffer.size(); i++)
		{
			ConvertToBytes<uint32_t>(*lcaccIDReplacementBuffer[i], lcaccIDReplacementIndex[i], lcaccID);
		}
	}
	inline void Reset()
	{
		lcaccIDDecided = false;
	}
};
class MicroprogramWriter
{
	int16_t computesHaveBeenWritten;
	int16_t transfersHaveBeenWritten;
	bool finalized;
	std::vector<uint8_t> buffer;
	bool signature;
	uint32_t taskGrain;
public:
	class ComputeArgIndex
	{
	public:
		uint32_t baseAddr;
		uint32_t elementSize;
		std::vector<uint32_t> size;
		std::vector<int32_t> stride;
		inline ComputeArgIndex(uint32_t init_baseAddr, uint32_t init_elementSize, const std::vector<uint32_t>& init_size, const std::vector<int32_t>& init_stride)
		{
			baseAddr = init_baseAddr;
			elementSize = init_elementSize;
			size = init_size;
			stride = init_stride;
		}
	};
	inline MicroprogramWriter(bool init_signature)
	{
		finalized = false;
		computesHaveBeenWritten = 0;
		transfersHaveBeenWritten = 0;
		taskGrain = 0;
		signature = init_signature;
		if(signature)
		{
			for(int i = 0; i < sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t); i++)
			{
				buffer.push_back(0);
			}
		}
		else
		{
			for(int i = 0; i < sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t); i++)
			{
				buffer.push_back(0);
			}
		}
	}
	inline void SetTaskGrain(uint32_t tg)
	{
		simics_assert(!finalized);
		simics_assert(!signature);
		taskGrain = tg;
	}
	inline void AddTransfer(LCAccNode& src, uintptr_t srcBaseAddr, const std::vector<uint32_t>& srcSize, const std::vector<int32_t>& srcStride, void* dst, const std::vector<uint32_t>& blockSize, const std::vector<int32_t>& blockStride, const std::vector<uint32_t>& elementSize, const std::vector<int32_t>& elementStride, uint32_t atomSize)
	{
		simics_assert(!finalized);
		simics_assert(computesHaveBeenWritten);
		simics_assert(srcSize.size() == srcStride.size());
		simics_assert(blockSize.size() == blockStride.size());
		simics_assert(elementSize.size() == elementStride.size());
		transfersHaveBeenWritten++;
		int chunkSize = sizeof(uintptr_t) * 2 + sizeof(uint32_t) * (3 + srcSize.size() + blockSize.size() + elementSize.size()) + sizeof(int32_t) * (1 + srcSize.size() + blockSize.size() + elementSize.size()) + sizeof(uint8_t) * 5;
		size_t chunkPosition = buffer.size();
		for(int i = 0; i < chunkSize; i++)
		{
			buffer.push_back(0);
		}
		src.PlaceLCAccID(buffer, chunkPosition); chunkPosition += sizeof(uint32_t);
		ConvertToBytes<uintptr_t>(buffer, chunkPosition, (uintptr_t)(srcBaseAddr));  chunkPosition += sizeof(uintptr_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(1));  chunkPosition += sizeof(uint8_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(srcSize.size()));  chunkPosition += sizeof(uint8_t);
		ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(-1));  chunkPosition += sizeof(uint32_t);
		ConvertToBytes<uintptr_t>(buffer, chunkPosition, (uintptr_t)(dst));  chunkPosition += sizeof(uintptr_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(blockSize.size()));  chunkPosition += sizeof(uint8_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(elementSize.size()));  chunkPosition += sizeof(uint8_t);
		ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(src.spmWindowCount));  chunkPosition += sizeof(uint32_t);//first, position the element in the spm
		ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(src.spmWindowSize));  chunkPosition += sizeof(int32_t);
		for(size_t i = 0; i < srcSize.size(); i++)
		{
			ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(srcSize[i]));  chunkPosition += sizeof(uint32_t);
			ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(srcStride[i]));  chunkPosition += sizeof(int32_t);
		}
		for(size_t i = 0; i < blockSize.size(); i++)
		{
			ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(blockSize[i]));  chunkPosition += sizeof(uint32_t);
			ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(blockStride[i]));  chunkPosition += sizeof(int32_t);
		}
		for(size_t i = 0; i < elementSize.size(); i++)
		{
			ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(elementSize[i]));  chunkPosition += sizeof(uint32_t);
			ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(elementStride[i]));  chunkPosition += sizeof(int32_t);
		}
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(atomSize));  chunkPosition += sizeof(uint8_t);
	}
	inline void AddTransfer(const void* src, const std::vector<uint32_t>& blockSize, const std::vector<int32_t>& blockStride, const std::vector<uint32_t>& elementSize, const std::vector<int32_t>& elementStride, LCAccNode& dst, uintptr_t dstBaseAddr, const std::vector<uint32_t>& dstSize, const std::vector<int32_t>& dstStride, uint32_t atomSize)
	{
		simics_assert(!finalized);
		simics_assert(computesHaveBeenWritten);
		simics_assert(dstSize.size() == dstStride.size());
		simics_assert(blockSize.size() == blockStride.size());
		simics_assert(elementSize.size() == elementStride.size());
		transfersHaveBeenWritten++;
		int chunkSize = sizeof(uintptr_t) * 2 + sizeof(uint32_t) * (3 + dstSize.size() + blockSize.size() + elementSize.size()) + sizeof(int32_t) * (1 + dstSize.size() + blockSize.size() + elementSize.size()) + sizeof(uint8_t) * 5;
		size_t chunkPosition = buffer.size();
		for(int i = 0; i < chunkSize; i++)
		{
			buffer.push_back(0);
		}
		ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(-1));  chunkPosition += sizeof(uint32_t);
		ConvertToBytes<uintptr_t>(buffer, chunkPosition, (uintptr_t)(src));  chunkPosition += sizeof(uintptr_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(blockSize.size()));  chunkPosition += sizeof(uint8_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(elementSize.size()));  chunkPosition += sizeof(uint8_t);
		dst.PlaceLCAccID(buffer, chunkPosition); chunkPosition += sizeof(uint32_t);
		ConvertToBytes<uintptr_t>(buffer, chunkPosition, (uintptr_t)(dstBaseAddr));  chunkPosition += sizeof(uintptr_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(1));  chunkPosition += sizeof(uint8_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(dstSize.size()));  chunkPosition += sizeof(uint8_t);
		for(size_t i = 0; i < blockSize.size(); i++)
		{
			ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(blockSize[i]));  chunkPosition += sizeof(uint32_t);
			ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(blockStride[i]));  chunkPosition += sizeof(int32_t);
		}
		for(size_t i = 0; i < elementSize.size(); i++)
		{
			ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(elementSize[i]));  chunkPosition += sizeof(uint32_t);
			ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(elementStride[i]));  chunkPosition += sizeof(int32_t);
		}
		ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(dst.spmWindowCount));  chunkPosition += sizeof(uint32_t);//first, position the element in the spm
		ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(dst.spmWindowSize));  chunkPosition += sizeof(int32_t);
		for(size_t i = 0; i < dstSize.size(); i++)
		{
			ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(dstSize[i]));  chunkPosition += sizeof(uint32_t);
			ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(dstStride[i]));  chunkPosition += sizeof(int32_t);
		}
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(atomSize));  chunkPosition += sizeof(uint8_t);
	}
	inline void AddTransfer(LCAccNode& src, uint32_t srcBaseAddr, const std::vector<uint32_t>& srcSize, const std::vector<int32_t>& srcStride, LCAccNode& dst, uint32_t dstBaseAddr, const std::vector<uint32_t>& dstSize, const std::vector<int32_t>& dstStride, uint32_t atomSize)
	{
		simics_assert(!finalized);
		simics_assert(computesHaveBeenWritten);
		simics_assert(dstSize.size() == dstStride.size());
		simics_assert(srcSize.size() == srcStride.size());
		transfersHaveBeenWritten++;
		int chunkSize = sizeof(uintptr_t) * 2 + sizeof(uint32_t) * (4 + srcSize.size() + dstSize.size()) + sizeof(int32_t) * (2 + srcSize.size() + dstSize.size()) + sizeof(uint8_t) * 5;
		size_t chunkPosition = buffer.size();
		for(int i = 0; i < chunkSize; i++)
		{
			buffer.push_back(0);
		}
		src.PlaceLCAccID(buffer, chunkPosition); chunkPosition += sizeof(uint32_t);
		ConvertToBytes<uintptr_t>(buffer, chunkPosition, (uintptr_t)(srcBaseAddr));  chunkPosition += sizeof(uintptr_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(1));  chunkPosition += sizeof(uint8_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(srcSize.size()));  chunkPosition += sizeof(uint8_t);
		dst.PlaceLCAccID(buffer, chunkPosition); chunkPosition += sizeof(uint32_t);
		ConvertToBytes<uintptr_t>(buffer, chunkPosition, (uintptr_t)(dstBaseAddr));  chunkPosition += sizeof(uintptr_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(1));  chunkPosition += sizeof(uint8_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(dstSize.size()));  chunkPosition += sizeof(uint8_t);
		//Source first
		ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(src.spmWindowCount));  chunkPosition += sizeof(uint32_t);//first, position the element in the spm
		ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(src.spmWindowSize));  chunkPosition += sizeof(int32_t);
		for(size_t i = 0; i < srcSize.size(); i++)
		{
			ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(srcSize[i]));  chunkPosition += sizeof(uint32_t);
			ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(srcStride[i]));  chunkPosition += sizeof(int32_t);
		}
		//Destination next
		ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(dst.spmWindowCount));  chunkPosition += sizeof(uint32_t);//first, position the element in the spm
		ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(dst.spmWindowSize));  chunkPosition += sizeof(int32_t);
		for(size_t i = 0; i < dstSize.size(); i++)
		{
			ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(dstSize[i]));  chunkPosition += sizeof(uint32_t);
			ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(dstStride[i]));  chunkPosition += sizeof(int32_t);
		}
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(atomSize));  chunkPosition += sizeof(uint8_t);
	}
	inline void AddCompute(LCAccNode& node, const std::vector<ComputeArgIndex>& indexSet, const std::vector<uint64_t>& registers)
	{
		simics_assert(!finalized);
		simics_assert(!transfersHaveBeenWritten);
		computesHaveBeenWritten++;
		int chunkSize = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint8_t) + (sizeof(uint64_t) * registers.size());
		for(size_t i = 0; i < indexSet.size(); i++)
		{
			chunkSize += sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint8_t) + ((sizeof(int32_t) * indexSet[i].stride.size()) + (sizeof(uint32_t) * indexSet[i].size.size()));
		}
		size_t chunkPosition = buffer.size();
		for(int i = 0; i < chunkSize; i++)
		{
			buffer.push_back(0);
		}
		ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(node.lcaccOpCode));  chunkPosition += sizeof(uint32_t);
		ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(node.spmWindowCount));  chunkPosition += sizeof(uint32_t);
		ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(node.spmWindowSize));  chunkPosition += sizeof(uint32_t);
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(indexSet.size()));  chunkPosition += sizeof(uint8_t);
		for(size_t i = 0; i < indexSet.size(); i++)
		{
			simics_assert(indexSet[i].size.size() == indexSet[i].stride.size());
			ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(indexSet[i].baseAddr));  chunkPosition += sizeof(uint32_t);
			ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(indexSet[i].size.size()));  chunkPosition += sizeof(uint8_t);
			for(size_t j = 0; j < indexSet[i].size.size(); j++)
			{
				ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(indexSet[i].size[j]));  chunkPosition += sizeof(uint32_t);
				ConvertToBytes<int32_t>(buffer, chunkPosition, (int32_t)(indexSet[i].stride[j]));  chunkPosition += sizeof(int32_t);
			}
			ConvertToBytes<uint32_t>(buffer, chunkPosition, (uint32_t)(indexSet[i].elementSize));  chunkPosition += sizeof(uint32_t);
		}
		ConvertToBytes<uint8_t>(buffer, chunkPosition, (uint8_t)(registers.size()));  chunkPosition += sizeof(uint8_t);
		for(size_t i = 0; i < registers.size(); i++)
		{
			ConvertToBytes<uint64_t>(buffer, chunkPosition, (uint64_t)(registers[i]));  chunkPosition += sizeof(uint64_t);
		}
	}
	inline void Finalize(uint32_t skipTasks, uint32_t numberOfTasks)
	{
		simics_assert(!finalized);
		if(signature)
		{
			ConvertToBytes<uint8_t>(buffer, 0, transfersHaveBeenWritten);
			ConvertToBytes<uint8_t>(buffer, sizeof(uint8_t), computesHaveBeenWritten);
			ConvertToBytes<uint32_t>(buffer, sizeof(uint8_t) + sizeof(uint8_t), skipTasks);
			ConvertToBytes<uint32_t>(buffer, sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t), numberOfTasks);
		}
		else
		{
			simics_assert(skipTasks == 0);
			ConvertToBytes<uint16_t>(buffer, 0, computesHaveBeenWritten);
			ConvertToBytes<uint16_t>(buffer, sizeof(uint16_t), transfersHaveBeenWritten);
			ConvertToBytes<uint32_t>(buffer, sizeof(uint16_t) + sizeof(uint16_t), taskGrain);
			ConvertToBytes<uint32_t>(buffer, sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint32_t), numberOfTasks);
		}
		finalized = true;
	}
	inline void Finalize(uint32_t numberOfTasks)
	{
		Finalize(0, numberOfTasks);
	}
	inline uint8_t* GetBuffer()
	{
		simics_assert(finalized);
		return &(buffer.at(0));
	}
	inline uint32_t GetBufferSize() const
	{
		simics_assert(finalized);
		return (uint32_t)(buffer.size());
	}
	inline bool IsFinalized() const
	{
		return finalized;
	}
};
inline void PrepareAsync_td(int thread, InterruptArgs& isrArgs)
{
	isrArgs.threadID = thread;
	isrArgs.lcaccID = 0;
	isrArgs.lcaccMode = 0;
	LWI_RegisterInterruptHandler(&isrArgs);
}
inline void FireAsync_td_buf(uint8_t* buf, uint32_t bufSize, int thread)
{
	LCAcc_Command(thread, 0, LCACC_CMD_BEGIN_PROGRAM, buf, bufSize, 0, 0);
}
inline void WaitAsync_td(int thread, InterruptArgs& isrArgs, int count)
{
	for(int i = 0; i < count; i++)
	{
		bool stillWorking = true;
		while(stillWorking)
		{
			InterruptArgs* args = 0;
			while((args = LWI_CheckInterrupt(thread)) == 0);
			simics_assert(args->lcaccMode == 0);
			switch(args->status)
			{
				case(LCACC_STATUS_TLB_MISS):
					LCAcc_Command(args->threadID, args->lcaccID, LCACC_CMD_TLB_SERVICE, (void*)(args->v[0]), 0, 0, 0);
					break;
				case(LCACC_STATUS_COMPLETED):
					stillWorking = false;
					break;
				default:
					simics_assert(0);
					stillWorking = false;
			}
			LWI_ClearInterrupt(thread);
		}
	}
}

#endif
#ifndef LCACC_BODY_SIG__PartialSortLCacc__X
#define LCACC_BODY_SIG__PartialSortLCacc__X
#define LCACC_CLASS_SIG__PartialSortLCacc__ps 1
class InstanceData_sig__PartialSortLCacc
{
public:
	InterruptArgs GAM_INTERACTION;
	int threadID;
	uint32_t binBufSize;
	unsigned int pendingAccelerators;
	unsigned int reservedAccelerators;
	unsigned int acceleratorVectorLength;
	int allocatedAcceleratorCount__PartialSort;
	int allocatedAcceleratorIDSet__PartialSort[1];
	float (*LCAcc_FuncArgs__in_dists);
	float LCAcc_FuncVars__res[100];
	int LCAcc_FuncVars__total_size;
	int LCAcc_FuncVars__chunk;
	MicroprogramWriter acceleratorSignature__ps;
	LCAccNode node_ps;
	InterruptArgs HandlerArgs__ps;
	inline void Reset()
	{
		pendingAccelerators = 0;
		allocatedAcceleratorCount__PartialSort = 0;
		reservedAccelerators = 0;
		allocatedAcceleratorIDSet__PartialSort[0] = 0;
		HandlerArgs__ps.threadID = threadID;
		HandlerArgs__ps.status = 0;
		HandlerArgs__ps.taskIndex = 0;
		HandlerArgs__ps.lcaccMode = LCACC_CLASS_SIG__PartialSortLCacc__ps;
		node_ps.Reset();
	}
	inline InstanceData_sig__PartialSortLCacc() :
		acceleratorSignature__ps(true), 
		node_ps(249), 
		threadID(0)
	{
		Reset();
	}
};
inline void (*GAMHandler_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance, InterruptArgs* args))(InstanceData_sig__PartialSortLCacc*) ;
inline void Cleanup_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance);
inline void StartEverythingHandler_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance);
inline void StopEverythingHandler_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance);
inline void ErrorHandler_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance);
inline void Wait_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance);
inline unsigned int DelayEstimate_sig__PartialSortLCacc(int vectorSize)
{
	return 1;
}
inline void (*ProgressHandler_sig__PartialSortLCacc__ps(InstanceData_sig__PartialSortLCacc* instance, InterruptArgs* args))(InstanceData_sig__PartialSortLCacc*);
//This is a procedurally generated interrupt-based program from a RAGraph data structure for function PartialSortLCacc
inline void Wait_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance)
{
	while(1)
	{
		int thread = instance->threadID;
		InterruptArgs* args = 0;
		void (*progress)(InstanceData_sig__PartialSortLCacc*);
		while((args = LWI_CheckInterrupt(thread)) == 0);
		switch(args->lcaccMode)
		{
			case(0):
				progress = GAMHandler_sig__PartialSortLCacc(instance, args);
				break;
			case(LCACC_CLASS_SIG__PartialSortLCacc__ps):
				progress = ProgressHandler_sig__PartialSortLCacc__ps(instance, args);
				break;
			default:
				ErrorHandler_sig__PartialSortLCacc(instance);
				simics_assert(0);
		}
		LWI_ClearInterrupt(instance->threadID);
		if(progress)
		{
			progress(instance);
			return;
		}
	}
}
inline void (*GAMHandler_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance, InterruptArgs* args))(InstanceData_sig__PartialSortLCacc*) 
{
	int i;
	int lcaccMode;
	int count;
	int lcaccID;
	switch(args->status)
	{
		case(LCACC_GAM_WAIT):
			lcaccMode = args->v[0];
			count = args->v[1];
			switch(lcaccMode)
			{
				case(249)://Mode: PartialSort
					for(i = 0; i < 1; i++)
					{
						instance->reservedAccelerators++;
						LCAcc_Reserve(args->threadID, args->v[2 + 2 * i], 1);
					}
					break;
			}
			if(instance->reservedAccelerators == 1)
			{
				instance->reservedAccelerators = 0;
				LCAcc_Reserve(args->threadID, 0, DelayEstimate_sig__PartialSortLCacc(instance->acceleratorVectorLength));
			}
			return 0;
			break;
		case(LCACC_GAM_GRANT):
			lcaccMode = args->v[0];
			lcaccID = args->v[1];
			switch(lcaccMode)
			{
				case(249):
					if(instance->allocatedAcceleratorCount__PartialSort == 1)
					{
						return ErrorHandler_sig__PartialSortLCacc;
					}
					else
					{
						instance->pendingAccelerators++;
						instance->allocatedAcceleratorIDSet__PartialSort[instance->allocatedAcceleratorCount__PartialSort] = lcaccID;
						instance->allocatedAcceleratorCount__PartialSort++;
					}
					break;
				default:
					return ErrorHandler_sig__PartialSortLCacc;
			}
			if(instance->allocatedAcceleratorCount__PartialSort == 1)
			{
				return StartEverythingHandler_sig__PartialSortLCacc;
			}
			else
			{
				return 0;
			}
			break;
		case(LCACC_GAM_ERROR):
			return ErrorHandler_sig__PartialSortLCacc;
			break;
		case(LCACC_GAM_REVOKE)://assumed thus far never to occur
			return ErrorHandler_sig__PartialSortLCacc;
			break;
		default:
			return ErrorHandler_sig__PartialSortLCacc;
	}
}
inline void Cleanup_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance)
{
	int i;
	LWI_UnregisterInterruptHandler(instance->GAM_INTERACTION.threadID, 0);
	if(instance->allocatedAcceleratorIDSet__PartialSort[0] != 0)
	{
		LCAcc_Free(instance->HandlerArgs__ps.threadID, instance->HandlerArgs__ps.lcaccID);
		LWI_UnregisterInterruptHandler(instance->HandlerArgs__ps.threadID, instance->HandlerArgs__ps.lcaccID);
	}
}
inline void StopEverythingHandler_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance)
{
	Cleanup_sig__PartialSortLCacc(instance);
}
inline void ErrorHandler_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance)
{
	Cleanup_sig__PartialSortLCacc(instance);
	simics_assert(0);
}
inline void (*ProgressHandler_sig__PartialSortLCacc__ps(InstanceData_sig__PartialSortLCacc* instance, InterruptArgs* args))(InstanceData_sig__PartialSortLCacc*)
{
	switch(args->status)
	{
		case(LCACC_STATUS_TLB_MISS):
			LCAcc_Command(args->threadID, args->lcaccID, LCACC_CMD_TLB_SERVICE, (void*)(args->v[0]), 0, 0, 0);
			return 0;
		case(LCACC_STATUS_COMPLETED):
			instance->pendingAccelerators--;
			if(instance->pendingAccelerators == 0)
			{
				return StopEverythingHandler_sig__PartialSortLCacc;
			}
			else
			{
				return 0;
			}
		case(LCACC_STATUS_ERROR):
			return ErrorHandler_sig__PartialSortLCacc;
		default:
			simics_assert(0);
			return 0;
	}
}
inline void StartEverythingHandler_sig__PartialSortLCacc(InstanceData_sig__PartialSortLCacc* instance)
{
	int index;
	instance->HandlerArgs__ps.threadID = instance->threadID;
	instance->HandlerArgs__ps.lcaccID = instance->allocatedAcceleratorIDSet__PartialSort[0];
	instance->node_ps.SetLCAccID(instance->HandlerArgs__ps.lcaccID);
	instance->HandlerArgs__ps.status = 0;
	instance->HandlerArgs__ps.taskIndex = 0;
	instance->HandlerArgs__ps.lcaccMode = LCACC_CLASS_SIG__PartialSortLCacc__ps;
	LWI_RegisterInterruptHandler(&(instance->HandlerArgs__ps));
	simics_assert(instance->acceleratorSignature__ps.IsFinalized());
	LCAcc_Command(instance->HandlerArgs__ps.threadID, instance->HandlerArgs__ps.lcaccID, LCACC_CMD_BEGIN_TASK_SIGNATURE, instance->acceleratorSignature__ps.GetBuffer(), instance->acceleratorSignature__ps.GetBufferSize(), 0, 0);
	Wait_sig__PartialSortLCacc(instance);// wait for everything to finish
}
inline void CreateBuffer_PartialSortLCacc_sig(int thread, InstanceData_sig__PartialSortLCacc* instance, float (*in_dists), float res[100], int total_size, int chunk)
{
	int index, i;
	instance->binBufSize = 0;
	instance->threadID = thread;
	instance->GAM_INTERACTION.threadID = thread;
	instance->GAM_INTERACTION.lcaccID = 0;
	instance->GAM_INTERACTION.lcaccMode = 0;
	instance->node_ps.SetSPMConfig((sizeof(float) * (chunk)), 2, 0);
	instance->LCAcc_FuncArgs__in_dists = in_dists;
	memcpy(instance->LCAcc_FuncVars__res, res, sizeof(float[100]));
	instance->LCAcc_FuncVars__total_size = total_size;
	instance->LCAcc_FuncVars__chunk = chunk;
	instance->allocatedAcceleratorCount__PartialSort = 0;
	LCAccNode& VNR_vardecl_0(instance->node_ps);
	void* VNR_vardecl_1(instance->LCAcc_FuncArgs__in_dists);
	std::vector<uint32_t> VNR_vardecl_2;
	VNR_vardecl_2.push_back(((chunk) - (0)) / (1));
	std::vector<int32_t> VNR_vardecl_3;
	VNR_vardecl_3.push_back((1) * (sizeof(float)));
	MicroprogramWriter::ComputeArgIndex VNR_vardecl_4((0) + ((((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_2, VNR_vardecl_3);
	std::vector<MicroprogramWriter::ComputeArgIndex> VNR_vardecl_5;
	VNR_vardecl_5.push_back(VNR_vardecl_4);
	std::vector<uint64_t> VNR_vardecl_6;
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(0) % (100)]) /*Register topK[0]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(1) % (100)]) /*Register topK[1]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(2) % (100)]) /*Register topK[2]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(3) % (100)]) /*Register topK[3]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(4) % (100)]) /*Register topK[4]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(5) % (100)]) /*Register topK[5]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(6) % (100)]) /*Register topK[6]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(7) % (100)]) /*Register topK[7]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(8) % (100)]) /*Register topK[8]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(9) % (100)]) /*Register topK[9]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(10) % (100)]) /*Register topK[10]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(11) % (100)]) /*Register topK[11]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(12) % (100)]) /*Register topK[12]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(13) % (100)]) /*Register topK[13]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(14) % (100)]) /*Register topK[14]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(15) % (100)]) /*Register topK[15]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(16) % (100)]) /*Register topK[16]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(17) % (100)]) /*Register topK[17]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(18) % (100)]) /*Register topK[18]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(19) % (100)]) /*Register topK[19]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(20) % (100)]) /*Register topK[20]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(21) % (100)]) /*Register topK[21]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(22) % (100)]) /*Register topK[22]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(23) % (100)]) /*Register topK[23]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(24) % (100)]) /*Register topK[24]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(25) % (100)]) /*Register topK[25]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(26) % (100)]) /*Register topK[26]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(27) % (100)]) /*Register topK[27]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(28) % (100)]) /*Register topK[28]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(29) % (100)]) /*Register topK[29]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(30) % (100)]) /*Register topK[30]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(31) % (100)]) /*Register topK[31]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(32) % (100)]) /*Register topK[32]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(33) % (100)]) /*Register topK[33]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(34) % (100)]) /*Register topK[34]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(35) % (100)]) /*Register topK[35]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(36) % (100)]) /*Register topK[36]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(37) % (100)]) /*Register topK[37]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(38) % (100)]) /*Register topK[38]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(39) % (100)]) /*Register topK[39]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(40) % (100)]) /*Register topK[40]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(41) % (100)]) /*Register topK[41]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(42) % (100)]) /*Register topK[42]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(43) % (100)]) /*Register topK[43]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(44) % (100)]) /*Register topK[44]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(45) % (100)]) /*Register topK[45]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(46) % (100)]) /*Register topK[46]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(47) % (100)]) /*Register topK[47]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(48) % (100)]) /*Register topK[48]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(49) % (100)]) /*Register topK[49]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(50) % (100)]) /*Register topK[50]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(51) % (100)]) /*Register topK[51]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(52) % (100)]) /*Register topK[52]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(53) % (100)]) /*Register topK[53]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(54) % (100)]) /*Register topK[54]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(55) % (100)]) /*Register topK[55]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(56) % (100)]) /*Register topK[56]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(57) % (100)]) /*Register topK[57]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(58) % (100)]) /*Register topK[58]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(59) % (100)]) /*Register topK[59]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(60) % (100)]) /*Register topK[60]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(61) % (100)]) /*Register topK[61]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(62) % (100)]) /*Register topK[62]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(63) % (100)]) /*Register topK[63]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(64) % (100)]) /*Register topK[64]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(65) % (100)]) /*Register topK[65]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(66) % (100)]) /*Register topK[66]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(67) % (100)]) /*Register topK[67]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(68) % (100)]) /*Register topK[68]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(69) % (100)]) /*Register topK[69]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(70) % (100)]) /*Register topK[70]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(71) % (100)]) /*Register topK[71]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(72) % (100)]) /*Register topK[72]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(73) % (100)]) /*Register topK[73]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(74) % (100)]) /*Register topK[74]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(75) % (100)]) /*Register topK[75]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(76) % (100)]) /*Register topK[76]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(77) % (100)]) /*Register topK[77]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(78) % (100)]) /*Register topK[78]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(79) % (100)]) /*Register topK[79]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(80) % (100)]) /*Register topK[80]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(81) % (100)]) /*Register topK[81]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(82) % (100)]) /*Register topK[82]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(83) % (100)]) /*Register topK[83]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(84) % (100)]) /*Register topK[84]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(85) % (100)]) /*Register topK[85]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(86) % (100)]) /*Register topK[86]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(87) % (100)]) /*Register topK[87]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(88) % (100)]) /*Register topK[88]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(89) % (100)]) /*Register topK[89]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(90) % (100)]) /*Register topK[90]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(91) % (100)]) /*Register topK[91]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(92) % (100)]) /*Register topK[92]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(93) % (100)]) /*Register topK[93]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(94) % (100)]) /*Register topK[94]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(95) % (100)]) /*Register topK[95]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(96) % (100)]) /*Register topK[96]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(97) % (100)]) /*Register topK[97]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(98) % (100)]) /*Register topK[98]*/);
	VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(99) % (100)]) /*Register topK[99]*/);
	std::vector<uint32_t> VNR_vardecl_7;
	VNR_vardecl_7.push_back(((total_size) - (0)) / (chunk));
	std::vector<int32_t> VNR_vardecl_8;
	VNR_vardecl_8.push_back((chunk) * ((sizeof(float))));
	std::vector<int32_t> VNR_vardecl_9;
	VNR_vardecl_9.push_back((1) * ((sizeof(float))));
	//produce compute set
	//See VNR_vardecl_5 for index variable decl
	//See VNR_vardecl_6 for register set decl
	instance->acceleratorSignature__ps.AddCompute(VNR_vardecl_0, VNR_vardecl_5, VNR_vardecl_6);
	//produce transfer set
	//transfer from in_dists to ps
	//Search VNR_vardecl_7 for source block size.
	//Search VNR_vardecl_8 for source block stride.
	//Search VNR_vardecl_2 for source element size.
	//Search VNR_vardecl_9 for source element stride.
	//Search VNR_vardecl_2 for destination size.
	//Search VNR_vardecl_9 for destination stride.
	instance->acceleratorSignature__ps.AddTransfer(VNR_vardecl_1, VNR_vardecl_7, VNR_vardecl_8, VNR_vardecl_2, VNR_vardecl_9, VNR_vardecl_0, (0) + ((((0) * ((sizeof(float)))))), VNR_vardecl_2, VNR_vardecl_9, sizeof(float));
	instance->acceleratorSignature__ps.Finalize((((total_size) - (0)) / (chunk)));
}
inline void PartialSortLCacc_sig_buf(int threadID, InstanceData_sig__PartialSortLCacc* instance)
{
	instance->Reset();
	LWI_RegisterInterruptHandler(&(instance->GAM_INTERACTION));
	LCAcc_Request(threadID, 249, 1);//request something of type AcceleratorType
	Wait_sig__PartialSortLCacc(instance);// wait for everything to finish
}
inline void PartialSortLCacc_sig(int threadID, float (*in_dists), float res[100], int total_size, int chunk)
{
	InstanceData_sig__PartialSortLCacc instance;
	CreateBuffer_PartialSortLCacc_sig(threadID, &instance, in_dists, res, total_size, chunk);
	PartialSortLCacc_sig_buf(threadID, &instance);
}
class BiN_PartialSortLCacc_Arbitrator_sig
{
	std::vector<InstanceData_sig__PartialSortLCacc*> instanceSet;
	std::vector<uint32_t> performancePoint;
	std::vector<uint32_t> cachePressureMod;
	std::vector<uint32_t> ops;
	std::vector<uint32_t> count;
	int threadID;
	int allocatedAcceleratorCount__PartialSort;
	int allocatedAcceleratorIDSet__PartialSort[1];
	InterruptArgs isr;
public:
	inline BiN_PartialSortLCacc_Arbitrator_sig(int thread)
	{
		ops.push_back(249);
		count.push_back(1);
		threadID = thread;
	}
	inline void AddConfig(InstanceData_sig__PartialSortLCacc* inst, uint32_t performance, uint32_t cacheMod)
	{
		simics_assert(inst->acceleratorSignature__ps.IsFinalized());
		instanceSet.push_back(inst);
		performancePoint.push_back(performance);
		cachePressureMod.push_back(cacheMod);
	}
	inline void Run()
	{
		isr.threadID = threadID;
		isr.lcaccID = 0;
		isr.lcaccMode = 0;
		for(size_t i = 0; i < instanceSet.size(); i++)
		{
			instanceSet[i]->Reset();
		}
		LWI_RegisterInterruptHandler(&isr);
		int allocatedAcceleratorCount__PartialSort = 0;
		LCAcc_Request(threadID, 249, 1);//request something of type AcceleratorType
		bool cont = true;
		uint32_t bufferSize = 0;
		bool bufKnown = false;
		uint32_t bufferID;
		int reserves = 0;
		while(cont)
		{
			InterruptArgs* args = 0;
			while((args = LWI_CheckInterrupt(threadID)) == 0);
			simics_assert(args == &isr);
			switch(args->status)
			{
				case(LCACC_GAM_WAIT):
					{
						int mode = args->v[0];
						switch(mode)
						{
							case(249)://Mode: PartialSort
								for(int i = 0; i < 1; i++)
								{
									reserves++;
									LCAcc_Reserve(threadID, args->v[2 + 2 * i], 1);
								}
								break;
						}
						if(reserves == 1)
						{
							for(size_t i = 0; i < instanceSet.size(); i++)
							{
								LCAcc_SendBiNCurve(threadID, instanceSet[i]->binBufSize, performancePoint[i], cachePressureMod[i]);
							}
							LCAcc_Reserve(threadID, 0, 1);
						}
					}
					break;
				case(LCACC_GAM_GRANT):
					{
						uint32_t lcaccMode = args->v[0];
						uint32_t lcaccID = args->v[1];
						uint32_t bufSize = args->v[3];
						if(!bufKnown)
						{
							bufferSize = bufSize;
							bufKnown = true;
						}
						else
						{
							simics_assert(bufferSize == bufSize);
						}
						switch(lcaccMode)
						{
							case(249):
								simics_assert(allocatedAcceleratorCount__PartialSort < 1);
								allocatedAcceleratorIDSet__PartialSort[allocatedAcceleratorCount__PartialSort] = lcaccID;
								allocatedAcceleratorCount__PartialSort++;
								break;
							default:
								simics_assert(0);
						}
						if(allocatedAcceleratorCount__PartialSort == 1)
						{
							cont = false;
						}
					}
					break;
				default:
					simics_assert(0);
			}
			LWI_ClearInterrupt(threadID);
		}
		simics_assert(bufKnown);
		InstanceData_sig__PartialSortLCacc* inst = NULL;
		for(size_t i = 0; i < instanceSet.size(); i++)
		{
			if(instanceSet[i]->binBufSize == bufferSize)
			{
				inst = instanceSet[i];
				break;
			}
		}
		simics_assert(inst);
		for(int i = 0; i < 1; i++)
		{
			inst->allocatedAcceleratorIDSet__PartialSort[i] = allocatedAcceleratorIDSet__PartialSort[i];
		}
		inst->allocatedAcceleratorCount__PartialSort = allocatedAcceleratorCount__PartialSort;
		inst->pendingAccelerators = 1;
		StartEverythingHandler_sig__PartialSortLCacc(inst);
	}
};

#endif
#ifndef LCACC_BODY_TD__PartialSortLCacc__X
#define LCACC_BODY_TD__PartialSortLCacc__X

inline void Wait_td__PartialSortLCacc(int thread)
{
	int stillWorking = (thread == 0) ? 0 : 1;
	while(stillWorking)
	{
		InterruptArgs* args = 0;
		while((args = LWI_CheckInterrupt(thread)) == 0);
		simics_assert(args->lcaccMode == 0);
		switch(args->status)
		{
			case(LCACC_STATUS_TLB_MISS):
				LCAcc_Command(args->threadID, args->lcaccID, LCACC_CMD_TLB_SERVICE, (void*)(args->v[0]), 0, 0, 0);
				break;
			case(LCACC_STATUS_COMPLETED):
				stillWorking = 0;
				break;
			default:
				simics_assert(0);
				stillWorking = 0;
		}
		LWI_ClearInterrupt(thread);
	}
}
inline void CreateBuffer_PartialSortLCacc_td(uint8_t** buffer, uint32_t* bufferSize, uint8_t** constCluster, int thread, float (*in_dists), float res[100], int total_size, int chunk)
{
	MicroprogramWriter mw(false);
	void* LCAcc_FuncArgs__in_dists = in_dists;
	{
		void* VNR_vardecl_0(in_dists);
		LCAccNode VNR_vardecl_1(249, (sizeof(float) * (chunk)), 2, 0, 0);
		std::vector<uint32_t> VNR_vardecl_2;
		VNR_vardecl_2.push_back(((chunk) - (0)) / (1));
		std::vector<int32_t> VNR_vardecl_3;
		VNR_vardecl_3.push_back((1) * (sizeof(float)));
		MicroprogramWriter::ComputeArgIndex VNR_vardecl_4((0) + ((((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_2, VNR_vardecl_3);
		std::vector<MicroprogramWriter::ComputeArgIndex> VNR_vardecl_5;
		VNR_vardecl_5.push_back(VNR_vardecl_4);
		std::vector<uint64_t> VNR_vardecl_6;
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(0) % (100)]) /*Register topK[0]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(1) % (100)]) /*Register topK[1]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(2) % (100)]) /*Register topK[2]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(3) % (100)]) /*Register topK[3]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(4) % (100)]) /*Register topK[4]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(5) % (100)]) /*Register topK[5]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(6) % (100)]) /*Register topK[6]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(7) % (100)]) /*Register topK[7]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(8) % (100)]) /*Register topK[8]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(9) % (100)]) /*Register topK[9]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(10) % (100)]) /*Register topK[10]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(11) % (100)]) /*Register topK[11]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(12) % (100)]) /*Register topK[12]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(13) % (100)]) /*Register topK[13]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(14) % (100)]) /*Register topK[14]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(15) % (100)]) /*Register topK[15]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(16) % (100)]) /*Register topK[16]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(17) % (100)]) /*Register topK[17]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(18) % (100)]) /*Register topK[18]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(19) % (100)]) /*Register topK[19]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(20) % (100)]) /*Register topK[20]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(21) % (100)]) /*Register topK[21]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(22) % (100)]) /*Register topK[22]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(23) % (100)]) /*Register topK[23]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(24) % (100)]) /*Register topK[24]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(25) % (100)]) /*Register topK[25]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(26) % (100)]) /*Register topK[26]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(27) % (100)]) /*Register topK[27]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(28) % (100)]) /*Register topK[28]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(29) % (100)]) /*Register topK[29]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(30) % (100)]) /*Register topK[30]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(31) % (100)]) /*Register topK[31]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(32) % (100)]) /*Register topK[32]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(33) % (100)]) /*Register topK[33]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(34) % (100)]) /*Register topK[34]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(35) % (100)]) /*Register topK[35]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(36) % (100)]) /*Register topK[36]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(37) % (100)]) /*Register topK[37]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(38) % (100)]) /*Register topK[38]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(39) % (100)]) /*Register topK[39]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(40) % (100)]) /*Register topK[40]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(41) % (100)]) /*Register topK[41]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(42) % (100)]) /*Register topK[42]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(43) % (100)]) /*Register topK[43]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(44) % (100)]) /*Register topK[44]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(45) % (100)]) /*Register topK[45]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(46) % (100)]) /*Register topK[46]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(47) % (100)]) /*Register topK[47]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(48) % (100)]) /*Register topK[48]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(49) % (100)]) /*Register topK[49]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(50) % (100)]) /*Register topK[50]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(51) % (100)]) /*Register topK[51]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(52) % (100)]) /*Register topK[52]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(53) % (100)]) /*Register topK[53]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(54) % (100)]) /*Register topK[54]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(55) % (100)]) /*Register topK[55]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(56) % (100)]) /*Register topK[56]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(57) % (100)]) /*Register topK[57]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(58) % (100)]) /*Register topK[58]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(59) % (100)]) /*Register topK[59]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(60) % (100)]) /*Register topK[60]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(61) % (100)]) /*Register topK[61]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(62) % (100)]) /*Register topK[62]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(63) % (100)]) /*Register topK[63]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(64) % (100)]) /*Register topK[64]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(65) % (100)]) /*Register topK[65]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(66) % (100)]) /*Register topK[66]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(67) % (100)]) /*Register topK[67]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(68) % (100)]) /*Register topK[68]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(69) % (100)]) /*Register topK[69]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(70) % (100)]) /*Register topK[70]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(71) % (100)]) /*Register topK[71]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(72) % (100)]) /*Register topK[72]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(73) % (100)]) /*Register topK[73]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(74) % (100)]) /*Register topK[74]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(75) % (100)]) /*Register topK[75]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(76) % (100)]) /*Register topK[76]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(77) % (100)]) /*Register topK[77]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(78) % (100)]) /*Register topK[78]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(79) % (100)]) /*Register topK[79]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(80) % (100)]) /*Register topK[80]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(81) % (100)]) /*Register topK[81]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(82) % (100)]) /*Register topK[82]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(83) % (100)]) /*Register topK[83]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(84) % (100)]) /*Register topK[84]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(85) % (100)]) /*Register topK[85]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(86) % (100)]) /*Register topK[86]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(87) % (100)]) /*Register topK[87]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(88) % (100)]) /*Register topK[88]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(89) % (100)]) /*Register topK[89]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(90) % (100)]) /*Register topK[90]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(91) % (100)]) /*Register topK[91]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(92) % (100)]) /*Register topK[92]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(93) % (100)]) /*Register topK[93]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(94) % (100)]) /*Register topK[94]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(95) % (100)]) /*Register topK[95]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(96) % (100)]) /*Register topK[96]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(97) % (100)]) /*Register topK[97]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(98) % (100)]) /*Register topK[98]*/);
		VNR_vardecl_6.push_back(ConvertToType<float, uint64_t>(res[(99) % (100)]) /*Register topK[99]*/);
		std::vector<uint32_t> VNR_vardecl_7;
		VNR_vardecl_7.push_back(((total_size) - (0)) / (chunk));
		std::vector<int32_t> VNR_vardecl_8;
		VNR_vardecl_8.push_back((chunk) * ((sizeof(float))));
		std::vector<int32_t> VNR_vardecl_9;
		VNR_vardecl_9.push_back((1) * ((sizeof(float))));
		//See VNR_vardecl_5 for index variable decl
		//See VNR_vardecl_6 for register set decl
		mw.AddCompute(VNR_vardecl_1, VNR_vardecl_5, VNR_vardecl_6);
		
		//transfer from in_dists to ps
		//Search VNR_vardecl_7 for source block size.
		//Search VNR_vardecl_8 for source block stride.
		//Search VNR_vardecl_2 for source element size.
		//Search VNR_vardecl_9 for source element stride.
		//Search VNR_vardecl_2 for destination size.
		//Search VNR_vardecl_9 for destination stride.
		mw.AddTransfer(VNR_vardecl_0, VNR_vardecl_7, VNR_vardecl_8, VNR_vardecl_2, VNR_vardecl_9, VNR_vardecl_1, (0) + ((((0) * ((sizeof(float)))))), VNR_vardecl_2, VNR_vardecl_9, sizeof(float));
		
		mw.SetTaskGrain(0);
		mw.Finalize((((total_size) - (0)) / (chunk)));
	}
	*buffer = new uint8_t[mw.GetBufferSize()];
	*bufferSize = mw.GetBufferSize();
	memcpy(*buffer, mw.GetBuffer(), mw.GetBufferSize());
	Touch(thread, *buffer, *bufferSize);
}
inline void PartialSortLCacc_td_buf(uint8_t* buf, uint32_t bufSize, int thread)
{
	InterruptArgs isrArgs;
	isrArgs.threadID = thread;
	isrArgs.lcaccID = 0;
	isrArgs.lcaccMode = 0;
	LWI_RegisterInterruptHandler(&isrArgs);
	LCAcc_Command(thread, isrArgs.lcaccID, LCACC_CMD_BEGIN_PROGRAM, buf, bufSize, 0, 0);
	Wait_td__PartialSortLCacc(thread);// wait for everything to finish
}
void PartialSortLCacc_td(int thread, float (*in_dists), float res[100], int total_size, int chunk)
{
	uint32_t bufSize;
	uint8_t* buffer;
	uint8_t* constCluster = NULL;
	CreateBuffer_PartialSortLCacc_td(&buffer, &bufSize, &constCluster, thread, in_dists, res, total_size, chunk);
	PartialSortLCacc_td_buf(buffer, bufSize, thread);
	if(constCluster)
	{
		delete [] constCluster;
	}
	delete [] buffer;
}
inline uint32_t PartialSortLCacc_CalculateBiNSize(float res[100], int total_size, int chunk)
{
	return 0;
}
class BiN_PartialSortLCacc_Arbitrator_td
{
	std::vector<uint8_t*> bufSet;
	std::vector<uint32_t> bufSizeSet;
	std::vector<uint32_t> binSizeSet;
	std::vector<uint32_t> performancePoint;
	std::vector<uint32_t> cachePressureMod;
	InterruptArgs isr;
public:
	inline BiN_PartialSortLCacc_Arbitrator_td(){}
	inline void AddConfig(uint8_t* buf, uint32_t bufSize, uint32_t binSize, uint32_t performance, uint32_t cacheMod)
	{
		bufSet.push_back(buf);
		bufSizeSet.push_back(bufSize);
		binSizeSet.push_back(binSize);
		performancePoint.push_back(performance);
		cachePressureMod.push_back(cacheMod);
	}
	inline void Run(int threadID)
	{
		isr.threadID = threadID;
		isr.lcaccID = 0;
		isr.lcaccMode = 0;
		LCAcc_DeclareLCAccUse(threadID, 249, 1); //requests for PartialSort
		for(size_t i = 0; i < binSizeSet.size(); i++)
		{
			LCAcc_SendBiNCurve(threadID, binSizeSet[i], performancePoint[i], cachePressureMod[i]);
		}
		LCAcc_SendBiNCurve(threadID, 0, 0, 0);
		LWI_RegisterInterruptHandler(&isr);
		bool cont = true;
		uint32_t bufferSize = 0;
		InterruptArgs* args = 0;
		while((args = LWI_CheckInterrupt(threadID)) == 0);
		simics_assert(args == &isr);
		switch(args->status)
		{
			case(BIN_CMD_ARBITRATE_RESPONSE):
				{
					bufferSize = args->v[0];
					cont = false;
				}
				break;
			default:
				simics_assert(0);
		}
		LWI_ClearInterrupt(threadID);
		for(size_t i = 0; i < binSizeSet.size(); i++)
		{
			if(binSizeSet[i] == bufferSize)
			{
				PartialSortLCacc_td_buf(bufSet[i], bufSizeSet[i], threadID);
				return;
			}
		}
		simics_assert(0);
	}
};

#endif
