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
#ifndef LCACC_BODY_SIG__DispMapIntegSum1LCacc__X
#define LCACC_BODY_SIG__DispMapIntegSum1LCacc__X
#define LCACC_CLASS_SIG__DispMapIntegSum1LCacc__sa 1
class InstanceData_sig__DispMapIntegSum1LCacc
{
public:
	InterruptArgs GAM_INTERACTION;
	int threadID;
	uint32_t binBufSize;
	unsigned int pendingAccelerators;
	unsigned int reservedAccelerators;
	unsigned int acceleratorVectorLength;
	int allocatedAcceleratorCount__DispMapIntegSum;
	int allocatedAcceleratorIDSet__DispMapIntegSum[1];
	float (*LCAcc_FuncArgs__in_val_array);
	float (*LCAcc_FuncArgs__out_sum_array);
	int LCAcc_FuncVars__dataSize;
	int LCAcc_FuncVars__SHIFT;
	int LCAcc_FuncVars__n1;
	int LCAcc_FuncVars__n2;
	int LCAcc_FuncVars__chunk1;
	int LCAcc_FuncVars__chunk2;
	int LCAcc_FuncVars__chunk3;
	int LCAcc_FuncVars__chunk4;
	MicroprogramWriter acceleratorSignature__sa;
	LCAccNode node_sa;
	InterruptArgs HandlerArgs__sa;
	inline void Reset()
	{
		pendingAccelerators = 0;
		allocatedAcceleratorCount__DispMapIntegSum = 0;
		reservedAccelerators = 0;
		allocatedAcceleratorIDSet__DispMapIntegSum[0] = 0;
		HandlerArgs__sa.threadID = threadID;
		HandlerArgs__sa.status = 0;
		HandlerArgs__sa.taskIndex = 0;
		HandlerArgs__sa.lcaccMode = LCACC_CLASS_SIG__DispMapIntegSum1LCacc__sa;
		node_sa.Reset();
	}
	inline InstanceData_sig__DispMapIntegSum1LCacc() :
		acceleratorSignature__sa(true),
		node_sa(831),
		threadID(0)
	{
		Reset();
	}
};
inline void (*GAMHandler_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance, InterruptArgs* args))(InstanceData_sig__DispMapIntegSum1LCacc*) ;
inline void Cleanup_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance);
inline void StartEverythingHandler_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance);
inline void StopEverythingHandler_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance);
inline void ErrorHandler_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance);
inline void Wait_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance);
inline unsigned int DelayEstimate_sig__DispMapIntegSum1LCacc(int vectorSize)
{
	return 1;
}
inline void (*ProgressHandler_sig__DispMapIntegSum1LCacc__sa(InstanceData_sig__DispMapIntegSum1LCacc* instance, InterruptArgs* args))(InstanceData_sig__DispMapIntegSum1LCacc*);
//This is a procedurally generated interrupt-based program from a RAGraph data structure for function DispMapIntegSum1LCacc
inline void Wait_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance)
{
	while(1)
	{
		int thread = instance->threadID;
		InterruptArgs* args = 0;
		void (*progress)(InstanceData_sig__DispMapIntegSum1LCacc*);
		while((args = LWI_CheckInterrupt(thread)) == 0);
		switch(args->lcaccMode)
		{
			case(0):
				progress = GAMHandler_sig__DispMapIntegSum1LCacc(instance, args);
				break;
			case(LCACC_CLASS_SIG__DispMapIntegSum1LCacc__sa):
				progress = ProgressHandler_sig__DispMapIntegSum1LCacc__sa(instance, args);
				break;
			default:
				ErrorHandler_sig__DispMapIntegSum1LCacc(instance);
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
inline void (*GAMHandler_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance, InterruptArgs* args))(InstanceData_sig__DispMapIntegSum1LCacc*)
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
				case(831)://Mode: DispMapIntegSum
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
				LCAcc_Reserve(args->threadID, 0, DelayEstimate_sig__DispMapIntegSum1LCacc(instance->acceleratorVectorLength));
			}
			return 0;
			break;
		case(LCACC_GAM_GRANT):
			lcaccMode = args->v[0];
			lcaccID = args->v[1];
			switch(lcaccMode)
			{
				case(831):
					if(instance->allocatedAcceleratorCount__DispMapIntegSum == 1)
					{
						return ErrorHandler_sig__DispMapIntegSum1LCacc;
					}
					else
					{
						instance->pendingAccelerators++;
						instance->allocatedAcceleratorIDSet__DispMapIntegSum[instance->allocatedAcceleratorCount__DispMapIntegSum] = lcaccID;
						instance->allocatedAcceleratorCount__DispMapIntegSum++;
					}
					break;
				default:
					return ErrorHandler_sig__DispMapIntegSum1LCacc;
			}
			if(instance->allocatedAcceleratorCount__DispMapIntegSum == 1)
			{
				return StartEverythingHandler_sig__DispMapIntegSum1LCacc;
			}
			else
			{
				return 0;
			}
			break;
		case(LCACC_GAM_ERROR):
			return ErrorHandler_sig__DispMapIntegSum1LCacc;
			break;
		case(LCACC_GAM_REVOKE)://assumed thus far never to occur
			return ErrorHandler_sig__DispMapIntegSum1LCacc;
			break;
		default:
			return ErrorHandler_sig__DispMapIntegSum1LCacc;
	}
}
inline void Cleanup_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance)
{
	int i;
	LWI_UnregisterInterruptHandler(instance->GAM_INTERACTION.threadID, 0);
	if(instance->allocatedAcceleratorIDSet__DispMapIntegSum[0] != 0)
	{
		LCAcc_Free(instance->HandlerArgs__sa.threadID, instance->HandlerArgs__sa.lcaccID);
		LWI_UnregisterInterruptHandler(instance->HandlerArgs__sa.threadID, instance->HandlerArgs__sa.lcaccID);
	}
}
inline void StopEverythingHandler_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance)
{
	Cleanup_sig__DispMapIntegSum1LCacc(instance);
}
inline void ErrorHandler_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance)
{
	Cleanup_sig__DispMapIntegSum1LCacc(instance);
	simics_assert(0);
}
inline void (*ProgressHandler_sig__DispMapIntegSum1LCacc__sa(InstanceData_sig__DispMapIntegSum1LCacc* instance, InterruptArgs* args))(InstanceData_sig__DispMapIntegSum1LCacc*)
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
				return StopEverythingHandler_sig__DispMapIntegSum1LCacc;
			}
			else
			{
				return 0;
			}
		case(LCACC_STATUS_ERROR):
			return ErrorHandler_sig__DispMapIntegSum1LCacc;
		default:
			simics_assert(0);
			return 0;
	}
}
inline void StartEverythingHandler_sig__DispMapIntegSum1LCacc(InstanceData_sig__DispMapIntegSum1LCacc* instance)
{
	int index;
	instance->HandlerArgs__sa.threadID = instance->threadID;
	instance->HandlerArgs__sa.lcaccID = instance->allocatedAcceleratorIDSet__DispMapIntegSum[0];
	instance->node_sa.SetLCAccID(instance->HandlerArgs__sa.lcaccID);
	instance->HandlerArgs__sa.status = 0;
	instance->HandlerArgs__sa.taskIndex = 0;
	instance->HandlerArgs__sa.lcaccMode = LCACC_CLASS_SIG__DispMapIntegSum1LCacc__sa;
	LWI_RegisterInterruptHandler(&(instance->HandlerArgs__sa));
	simics_assert(instance->acceleratorSignature__sa.IsFinalized());
	LCAcc_Command(instance->HandlerArgs__sa.threadID, instance->HandlerArgs__sa.lcaccID, LCACC_CMD_BEGIN_TASK_SIGNATURE, instance->acceleratorSignature__sa.GetBuffer(), instance->acceleratorSignature__sa.GetBufferSize(), 0, 0);
	Wait_sig__DispMapIntegSum1LCacc(instance);// wait for everything to finish
}
inline void CreateBuffer_DispMapIntegSum1LCacc_sig(int thread, InstanceData_sig__DispMapIntegSum1LCacc* instance, float (*in_val_array), float (*out_sum_array), int dataSize, int SHIFT, int n1, int n2, int chunk1, int chunk2, int chunk3, int chunk4)
{
	simics_assert(dataSize > 0);
	simics_assert(SHIFT > 0);
	simics_assert(n1 > 0);
	simics_assert(n2 > 0);
	simics_assert(chunk1 > 0);
	simics_assert(chunk2 > 0);
	simics_assert(chunk3 > 0);
	simics_assert(chunk4 > 0);
	simics_assert(dataSize % chunk1 == 0);
	simics_assert(SHIFT % chunk2 == 0);
	simics_assert(n1 % chunk3 == 0);
	simics_assert(n2 % chunk4 == 0);
	int index, i;
	instance->binBufSize = 0;
	instance->threadID = thread;
	instance->GAM_INTERACTION.threadID = thread;
	instance->GAM_INTERACTION.lcaccID = 0;
	instance->GAM_INTERACTION.lcaccMode = 0;
	instance->node_sa.SetSPMConfig((sizeof(float) * (chunk1) * (chunk2) * (chunk3) * (chunk4)) + (sizeof(float) * (chunk1) * (chunk2) * (chunk3) * (chunk4)), 3, 0);
	instance->LCAcc_FuncArgs__in_val_array = in_val_array;
	instance->LCAcc_FuncArgs__out_sum_array = out_sum_array;
	instance->LCAcc_FuncVars__dataSize = dataSize;
	instance->LCAcc_FuncVars__SHIFT = SHIFT;
	instance->LCAcc_FuncVars__n1 = n1;
	instance->LCAcc_FuncVars__n2 = n2;
	instance->LCAcc_FuncVars__chunk1 = chunk1;
	instance->LCAcc_FuncVars__chunk2 = chunk2;
	instance->LCAcc_FuncVars__chunk3 = chunk3;
	instance->LCAcc_FuncVars__chunk4 = chunk4;
	instance->allocatedAcceleratorCount__DispMapIntegSum = 0;
	LCAccNode& VNR_vardecl_0(instance->node_sa);
	void* VNR_vardecl_1(instance->LCAcc_FuncArgs__in_val_array);
	void* VNR_vardecl_2(instance->LCAcc_FuncArgs__out_sum_array);
	std::vector<uint32_t> VNR_vardecl_3;
	VNR_vardecl_3.push_back(((chunk1) - (0)) / (1));
	VNR_vardecl_3.push_back(((chunk2) - (0)) / (1));
	VNR_vardecl_3.push_back(((chunk3) - (0)) / (1));
	VNR_vardecl_3.push_back(((chunk4) - (0)) / (1));
	std::vector<int32_t> VNR_vardecl_4;
	VNR_vardecl_4.push_back((1) * (sizeof(float) * (chunk2) * (chunk3) * (chunk4)));
	VNR_vardecl_4.push_back((1) * (sizeof(float) * (chunk3) * (chunk4)));
	VNR_vardecl_4.push_back((1) * (sizeof(float) * (chunk4)));
	VNR_vardecl_4.push_back((1) * (sizeof(float)));
	MicroprogramWriter::ComputeArgIndex VNR_vardecl_5((0) + ((((0) * (sizeof(float) * (chunk2) * (chunk3) * (chunk4))) + ((0) * (sizeof(float) * (chunk3) * (chunk4))) + ((0) * (sizeof(float) * (chunk4))) + ((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_3, VNR_vardecl_4);
	MicroprogramWriter::ComputeArgIndex VNR_vardecl_6(((sizeof(float) * (chunk1) * (chunk2) * (chunk3) * (chunk4))) + ((((0) * (sizeof(float) * (chunk2) * (chunk3) * (chunk4))) + ((0) * (sizeof(float) * (chunk3) * (chunk4))) + ((0) * (sizeof(float) * (chunk4))) + ((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_3, VNR_vardecl_4);
	std::vector<MicroprogramWriter::ComputeArgIndex> VNR_vardecl_7;
	VNR_vardecl_7.push_back(VNR_vardecl_5);
	VNR_vardecl_7.push_back(VNR_vardecl_6);
	std::vector<uint64_t> VNR_vardecl_8;
	VNR_vardecl_8.push_back(ConvertToType<float, uint64_t>(0)/*Register internal_sum*/);
	std::vector<uint32_t> VNR_vardecl_9;
	VNR_vardecl_9.push_back(((dataSize) - (0)) / (chunk1));
	VNR_vardecl_9.push_back(((SHIFT) - (0)) / (chunk2));
	VNR_vardecl_9.push_back(((n1) - (0)) / (chunk3));
	VNR_vardecl_9.push_back(((n2) - (0)) / (chunk4));
	std::vector<int32_t> VNR_vardecl_10;
	VNR_vardecl_10.push_back((chunk1) * ((sizeof(float) * (SHIFT) * (n1) * (n2))));
	VNR_vardecl_10.push_back((chunk2) * ((sizeof(float) * (n1) * (n2))));
	VNR_vardecl_10.push_back((chunk3) * ((sizeof(float) * (n2))));
	VNR_vardecl_10.push_back((chunk4) * ((sizeof(float))));
	std::vector<int32_t> VNR_vardecl_11;
	VNR_vardecl_11.push_back((1) * ((sizeof(float) * (SHIFT) * (n1) * (n2))));
	VNR_vardecl_11.push_back((1) * ((sizeof(float) * (n1) * (n2))));
	VNR_vardecl_11.push_back((1) * ((sizeof(float) * (n2))));
	VNR_vardecl_11.push_back((1) * ((sizeof(float))));
	std::vector<int32_t> VNR_vardecl_12;
	VNR_vardecl_12.push_back((1) * ((sizeof(float)) * (chunk2) * (chunk3) * (chunk4)));
	VNR_vardecl_12.push_back((1) * ((sizeof(float)) * (chunk3) * (chunk4)));
	VNR_vardecl_12.push_back((1) * ((sizeof(float)) * (chunk4)));
	VNR_vardecl_12.push_back((1) * ((sizeof(float))));
	//produce compute set
	//See VNR_vardecl_7 for index variable decl
	//See VNR_vardecl_8 for register set decl
	instance->acceleratorSignature__sa.AddCompute(VNR_vardecl_0, VNR_vardecl_7, VNR_vardecl_8);
	//produce transfer set
	//transfer from in_val_array to sa
	//Search VNR_vardecl_9 for source block size.
	//Search VNR_vardecl_10 for source block stride.
	//Search VNR_vardecl_3 for source element size.
	//Search VNR_vardecl_11 for source element stride.
	//Search VNR_vardecl_3 for destination size.
	//Search VNR_vardecl_12 for destination stride.
	instance->acceleratorSignature__sa.AddTransfer(VNR_vardecl_1, VNR_vardecl_9, VNR_vardecl_10, VNR_vardecl_3, VNR_vardecl_11, VNR_vardecl_0, (0) + ((((0) * ((sizeof(float)) * (chunk2) * (chunk3) * (chunk4))) + ((0) * ((sizeof(float)) * (chunk3) * (chunk4))) + ((0) * ((sizeof(float)) * (chunk4))) + ((0) * ((sizeof(float)))))), VNR_vardecl_3, VNR_vardecl_12, sizeof(float));
	//transfer from sa to out_sum_array
	//Search VNR_vardecl_3 for source size.
	//Search VNR_vardecl_12 for source stride.
	//Search VNR_vardecl_9 for destination block size.
	//Search VNR_vardecl_10 for destination block stride.
	//Search VNR_vardecl_3 for destination element size.
	//Search VNR_vardecl_11 for destination element stride.
	instance->acceleratorSignature__sa.AddTransfer(VNR_vardecl_0, ((sizeof(float) * (chunk1) * (chunk2) * (chunk3) * (chunk4))) + ((((0) * ((sizeof(float)) * (chunk2) * (chunk3) * (chunk4))) + ((0) * ((sizeof(float)) * (chunk3) * (chunk4))) + ((0) * ((sizeof(float)) * (chunk4))) + ((0) * ((sizeof(float)))))), VNR_vardecl_3, VNR_vardecl_12, VNR_vardecl_2, VNR_vardecl_9, VNR_vardecl_10, VNR_vardecl_3, VNR_vardecl_11, sizeof(float));
	instance->acceleratorSignature__sa.Finalize((((dataSize) - (0)) / (chunk1)) * (((SHIFT) - (0)) / (chunk2)) * (((n1) - (0)) / (chunk3)) * (((n2) - (0)) / (chunk4)));
}
inline void DispMapIntegSum1LCacc_sig_buf(int threadID, InstanceData_sig__DispMapIntegSum1LCacc* instance)
{
	instance->Reset();
	LWI_RegisterInterruptHandler(&(instance->GAM_INTERACTION));
	LCAcc_Request(threadID, 831, 1);//request something of type AcceleratorType
	Wait_sig__DispMapIntegSum1LCacc(instance);// wait for everything to finish
}
inline void DispMapIntegSum1LCacc_sig(int threadID, float (*in_val_array), float (*out_sum_array), int dataSize, int SHIFT, int n1, int n2, int chunk1, int chunk2, int chunk3, int chunk4)
{
	InstanceData_sig__DispMapIntegSum1LCacc instance;
	CreateBuffer_DispMapIntegSum1LCacc_sig(threadID, &instance, in_val_array, out_sum_array, dataSize, SHIFT, n1, n2, chunk1, chunk2, chunk3, chunk4);
	DispMapIntegSum1LCacc_sig_buf(threadID, &instance);
}
class BiN_DispMapIntegSum1LCacc_Arbitrator_sig
{
	std::vector<InstanceData_sig__DispMapIntegSum1LCacc*> instanceSet;
	std::vector<uint32_t> performancePoint;
	std::vector<uint32_t> cachePressureMod;
	std::vector<uint32_t> ops;
	std::vector<uint32_t> count;
	int threadID;
	int allocatedAcceleratorCount__DispMapIntegSum;
	int allocatedAcceleratorIDSet__DispMapIntegSum[1];
	InterruptArgs isr;
public:
	inline BiN_DispMapIntegSum1LCacc_Arbitrator_sig(int thread)
	{
		ops.push_back(831);
		count.push_back(1);
		threadID = thread;
	}
	inline void AddConfig(InstanceData_sig__DispMapIntegSum1LCacc* inst, uint32_t performance, uint32_t cacheMod)
	{
		simics_assert(inst->acceleratorSignature__sa.IsFinalized());
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
		int allocatedAcceleratorCount__DispMapIntegSum = 0;
		LCAcc_Request(threadID, 831, 1);//request something of type AcceleratorType
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
							case(831)://Mode: DispMapIntegSum
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
							case(831):
								simics_assert(allocatedAcceleratorCount__DispMapIntegSum < 1);
								allocatedAcceleratorIDSet__DispMapIntegSum[allocatedAcceleratorCount__DispMapIntegSum] = lcaccID;
								allocatedAcceleratorCount__DispMapIntegSum++;
								break;
							default:
								simics_assert(0);
						}
						if(allocatedAcceleratorCount__DispMapIntegSum == 1)
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
		InstanceData_sig__DispMapIntegSum1LCacc* inst = NULL;
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
			inst->allocatedAcceleratorIDSet__DispMapIntegSum[i] = allocatedAcceleratorIDSet__DispMapIntegSum[i];
		}
		inst->allocatedAcceleratorCount__DispMapIntegSum = allocatedAcceleratorCount__DispMapIntegSum;
		inst->pendingAccelerators = 1;
		StartEverythingHandler_sig__DispMapIntegSum1LCacc(inst);
	}
};

#endif
#ifndef LCACC_BODY_TD__DispMapIntegSum1LCacc__X
#define LCACC_BODY_TD__DispMapIntegSum1LCacc__X

inline void Wait_td__DispMapIntegSum1LCacc(int thread)
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
inline void CreateBuffer_DispMapIntegSum1LCacc_td(uint8_t** buffer, uint32_t* bufferSize, uint8_t** constCluster, int thread, float (*in_val_array), float (*out_sum_array), int dataSize, int SHIFT, int n1, int n2, int chunk1, int chunk2, int chunk3, int chunk4)
{
	simics_assert(dataSize > 0);
	simics_assert(SHIFT > 0);
	simics_assert(n1 > 0);
	simics_assert(n2 > 0);
	simics_assert(chunk1 > 0);
	simics_assert(chunk2 > 0);
	simics_assert(chunk3 > 0);
	simics_assert(chunk4 > 0);
	simics_assert(dataSize % chunk1 == 0);
	simics_assert(SHIFT % chunk2 == 0);
	simics_assert(n1 % chunk3 == 0);
	simics_assert(n2 % chunk4 == 0);
	MicroprogramWriter mw(false);
	void* LCAcc_FuncArgs__in_val_array = in_val_array;
	void* LCAcc_FuncArgs__out_sum_array = out_sum_array;
	{
		void* VNR_vardecl_0(in_val_array);
		void* VNR_vardecl_1(out_sum_array);
		LCAccNode VNR_vardecl_2(831, (sizeof(float) * (chunk1) * (chunk2) * (chunk3) * (chunk4)) + (sizeof(float) * (chunk1) * (chunk2) * (chunk3) * (chunk4)), 2, 0, 0);
		std::vector<uint32_t> VNR_vardecl_3;
		VNR_vardecl_3.push_back(((chunk1) - (0)) / (1));
		VNR_vardecl_3.push_back(((chunk2) - (0)) / (1));
		VNR_vardecl_3.push_back(((chunk3) - (0)) / (1));
		VNR_vardecl_3.push_back(((chunk4) - (0)) / (1));
		std::vector<int32_t> VNR_vardecl_4;
		VNR_vardecl_4.push_back((1) * (sizeof(float) * (chunk2) * (chunk3) * (chunk4)));
		VNR_vardecl_4.push_back((1) * (sizeof(float) * (chunk3) * (chunk4)));
		VNR_vardecl_4.push_back((1) * (sizeof(float) * (chunk4)));
		VNR_vardecl_4.push_back((1) * (sizeof(float)));
		MicroprogramWriter::ComputeArgIndex VNR_vardecl_5((0) + ((((0) * (sizeof(float) * (chunk2) * (chunk3) * (chunk4))) + ((0) * (sizeof(float) * (chunk3) * (chunk4))) + ((0) * (sizeof(float) * (chunk4))) + ((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_3, VNR_vardecl_4);
		MicroprogramWriter::ComputeArgIndex VNR_vardecl_6(((sizeof(float) * (chunk1) * (chunk2) * (chunk3) * (chunk4))) + ((((0) * (sizeof(float) * (chunk2) * (chunk3) * (chunk4))) + ((0) * (sizeof(float) * (chunk3) * (chunk4))) + ((0) * (sizeof(float) * (chunk4))) + ((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_3, VNR_vardecl_4);
		std::vector<MicroprogramWriter::ComputeArgIndex> VNR_vardecl_7;
		VNR_vardecl_7.push_back(VNR_vardecl_5);
		VNR_vardecl_7.push_back(VNR_vardecl_6);
		std::vector<uint64_t> VNR_vardecl_8;
		VNR_vardecl_8.push_back(ConvertToType<float, uint64_t>(0)/*Register internal_sum*/);
		std::vector<uint32_t> VNR_vardecl_9;
		VNR_vardecl_9.push_back(((dataSize) - (0)) / (chunk1));
		VNR_vardecl_9.push_back(((SHIFT) - (0)) / (chunk2));
		VNR_vardecl_9.push_back(((n1) - (0)) / (chunk3));
		VNR_vardecl_9.push_back(((n2) - (0)) / (chunk4));
		std::vector<int32_t> VNR_vardecl_10;
		VNR_vardecl_10.push_back((chunk1) * ((sizeof(float) * (SHIFT) * (n1) * (n2))));
		VNR_vardecl_10.push_back((chunk2) * ((sizeof(float) * (n1) * (n2))));
		VNR_vardecl_10.push_back((chunk3) * ((sizeof(float) * (n2))));
		VNR_vardecl_10.push_back((chunk4) * ((sizeof(float))));
		std::vector<int32_t> VNR_vardecl_11;
		VNR_vardecl_11.push_back((1) * ((sizeof(float) * (SHIFT) * (n1) * (n2))));
		VNR_vardecl_11.push_back((1) * ((sizeof(float) * (n1) * (n2))));
		VNR_vardecl_11.push_back((1) * ((sizeof(float) * (n2))));
		VNR_vardecl_11.push_back((1) * ((sizeof(float))));
		std::vector<int32_t> VNR_vardecl_12;
		VNR_vardecl_12.push_back((1) * ((sizeof(float)) * (chunk2) * (chunk3) * (chunk4)));
		VNR_vardecl_12.push_back((1) * ((sizeof(float)) * (chunk3) * (chunk4)));
		VNR_vardecl_12.push_back((1) * ((sizeof(float)) * (chunk4)));
		VNR_vardecl_12.push_back((1) * ((sizeof(float))));
		//See VNR_vardecl_7 for index variable decl
		//See VNR_vardecl_8 for register set decl
		mw.AddCompute(VNR_vardecl_2, VNR_vardecl_7, VNR_vardecl_8);

		//transfer from in_val_array to sa
		//Search VNR_vardecl_9 for source block size.
		//Search VNR_vardecl_10 for source block stride.
		//Search VNR_vardecl_3 for source element size.
		//Search VNR_vardecl_11 for source element stride.
		//Search VNR_vardecl_3 for destination size.
		//Search VNR_vardecl_12 for destination stride.
		mw.AddTransfer(VNR_vardecl_0, VNR_vardecl_9, VNR_vardecl_10, VNR_vardecl_3, VNR_vardecl_11, VNR_vardecl_2, (0) + ((((0) * ((sizeof(float)) * (chunk2) * (chunk3) * (chunk4))) + ((0) * ((sizeof(float)) * (chunk3) * (chunk4))) + ((0) * ((sizeof(float)) * (chunk4))) + ((0) * ((sizeof(float)))))), VNR_vardecl_3, VNR_vardecl_12, sizeof(float));

		//transfer from sa to out_sum_array
		//Search VNR_vardecl_3 for source size.
		//Search VNR_vardecl_12 for source stride.
		//Search VNR_vardecl_9 for destination block size.
		//Search VNR_vardecl_10 for destination block stride.
		//Search VNR_vardecl_3 for destination element size.
		//Search VNR_vardecl_11 for destination element stride.
		mw.AddTransfer(VNR_vardecl_2, ((sizeof(float) * (chunk1) * (chunk2) * (chunk3) * (chunk4))) + ((((0) * ((sizeof(float)) * (chunk2) * (chunk3) * (chunk4))) + ((0) * ((sizeof(float)) * (chunk3) * (chunk4))) + ((0) * ((sizeof(float)) * (chunk4))) + ((0) * ((sizeof(float)))))), VNR_vardecl_3, VNR_vardecl_12, VNR_vardecl_1, VNR_vardecl_9, VNR_vardecl_10, VNR_vardecl_3, VNR_vardecl_11, sizeof(float));

		mw.SetTaskGrain(0);
		mw.Finalize((((dataSize) - (0)) / (chunk1)) * (((SHIFT) - (0)) / (chunk2)) * (((n1) - (0)) / (chunk3)) * (((n2) - (0)) / (chunk4)));
	}
	*buffer = new uint8_t[mw.GetBufferSize()];
	*bufferSize = mw.GetBufferSize();
	memcpy(*buffer, mw.GetBuffer(), mw.GetBufferSize());
	Touch(thread, *buffer, *bufferSize);
}
inline void DispMapIntegSum1LCacc_td_buf(uint8_t* buf, uint32_t bufSize, int thread)
{
	InterruptArgs isrArgs;
	isrArgs.threadID = thread;
	isrArgs.lcaccID = 0;
	isrArgs.lcaccMode = 0;
	LWI_RegisterInterruptHandler(&isrArgs);
	LCAcc_Command(thread, isrArgs.lcaccID, LCACC_CMD_BEGIN_PROGRAM, buf, bufSize, 0, 0);
	Wait_td__DispMapIntegSum1LCacc(thread);// wait for everything to finish
}
void DispMapIntegSum1LCacc_td(int thread, float (*in_val_array), float (*out_sum_array), int dataSize, int SHIFT, int n1, int n2, int chunk1, int chunk2, int chunk3, int chunk4)
{
	uint32_t bufSize;
	uint8_t* buffer;
	uint8_t* constCluster = NULL;
	CreateBuffer_DispMapIntegSum1LCacc_td(&buffer, &bufSize, &constCluster, thread, in_val_array, out_sum_array, dataSize, SHIFT, n1, n2, chunk1, chunk2, chunk3, chunk4);
	DispMapIntegSum1LCacc_td_buf(buffer, bufSize, thread);
	if(constCluster)
	{
		delete [] constCluster;
	}
	delete [] buffer;
}
inline uint32_t DispMapIntegSum1LCacc_CalculateBiNSize(int dataSize, int SHIFT, int n1, int n2, int chunk1, int chunk2, int chunk3, int chunk4)
{
	return 0;
}
class BiN_DispMapIntegSum1LCacc_Arbitrator_td
{
	std::vector<uint8_t*> bufSet;
	std::vector<uint32_t> bufSizeSet;
	std::vector<uint32_t> binSizeSet;
	std::vector<uint32_t> performancePoint;
	std::vector<uint32_t> cachePressureMod;
	InterruptArgs isr;
public:
	inline BiN_DispMapIntegSum1LCacc_Arbitrator_td(){}
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
		LCAcc_DeclareLCAccUse(threadID, 831, 1); //requests for DispMapIntegSum
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
				DispMapIntegSum1LCacc_td_buf(bufSet[i], bufSizeSet[i], threadID);
				return;
			}
		}
		simics_assert(0);
	}
};

#endif
