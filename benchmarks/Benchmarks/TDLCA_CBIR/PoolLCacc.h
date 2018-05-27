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
#ifndef LCACC_BODY_SIG__PoolLCacc__X
#define LCACC_BODY_SIG__PoolLCacc__X
#define LCACC_CLASS_SIG__PoolLCacc__p 1
class InstanceData_sig__PoolLCacc
{
public:
	InterruptArgs GAM_INTERACTION;
	int threadID;
	uint32_t binBufSize;
	unsigned int pendingAccelerators;
	unsigned int reservedAccelerators;
	unsigned int acceleratorVectorLength;
	int allocatedAcceleratorCount__Pool;
	int allocatedAcceleratorIDSet__Pool[1];
	float (*LCAcc_FuncArgs__in);
	float (*LCAcc_FuncArgs__out);
	int LCAcc_FuncVars__Fout;
	int LCAcc_FuncVars__FoutR;
	int LCAcc_FuncVars__FoutC;
	int LCAcc_FuncVars__chunkR;
	int LCAcc_FuncVars__chunkC;
	MicroprogramWriter acceleratorSignature__p;
	LCAccNode node_p;
	InterruptArgs HandlerArgs__p;
	inline void Reset()
	{
		pendingAccelerators = 0;
		allocatedAcceleratorCount__Pool = 0;
		reservedAccelerators = 0;
		allocatedAcceleratorIDSet__Pool[0] = 0;
		HandlerArgs__p.threadID = threadID;
		HandlerArgs__p.status = 0;
		HandlerArgs__p.taskIndex = 0;
		HandlerArgs__p.lcaccMode = LCACC_CLASS_SIG__PoolLCacc__p;
		node_p.Reset();
	}
	inline InstanceData_sig__PoolLCacc() :
		acceleratorSignature__p(true), 
		node_p(250), 
		threadID(0)
	{
		Reset();
	}
};
inline void (*GAMHandler_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance, InterruptArgs* args))(InstanceData_sig__PoolLCacc*) ;
inline void Cleanup_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance);
inline void StartEverythingHandler_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance);
inline void StopEverythingHandler_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance);
inline void ErrorHandler_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance);
inline void Wait_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance);
inline unsigned int DelayEstimate_sig__PoolLCacc(int vectorSize)
{
	return 1;
}
inline void (*ProgressHandler_sig__PoolLCacc__p(InstanceData_sig__PoolLCacc* instance, InterruptArgs* args))(InstanceData_sig__PoolLCacc*);
//This is a procedurally generated interrupt-based program from a RAGraph data structure for function PoolLCacc
inline void Wait_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance)
{
	while(1)
	{
		int thread = instance->threadID;
		InterruptArgs* args = 0;
		void (*progress)(InstanceData_sig__PoolLCacc*);
		while((args = LWI_CheckInterrupt(thread)) == 0);
		switch(args->lcaccMode)
		{
			case(0):
				progress = GAMHandler_sig__PoolLCacc(instance, args);
				break;
			case(LCACC_CLASS_SIG__PoolLCacc__p):
				progress = ProgressHandler_sig__PoolLCacc__p(instance, args);
				break;
			default:
				ErrorHandler_sig__PoolLCacc(instance);
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
inline void (*GAMHandler_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance, InterruptArgs* args))(InstanceData_sig__PoolLCacc*) 
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
				case(250)://Mode: Pool
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
				LCAcc_Reserve(args->threadID, 0, DelayEstimate_sig__PoolLCacc(instance->acceleratorVectorLength));
			}
			return 0;
			break;
		case(LCACC_GAM_GRANT):
			lcaccMode = args->v[0];
			lcaccID = args->v[1];
			switch(lcaccMode)
			{
				case(250):
					if(instance->allocatedAcceleratorCount__Pool == 1)
					{
						return ErrorHandler_sig__PoolLCacc;
					}
					else
					{
						instance->pendingAccelerators++;
						instance->allocatedAcceleratorIDSet__Pool[instance->allocatedAcceleratorCount__Pool] = lcaccID;
						instance->allocatedAcceleratorCount__Pool++;
					}
					break;
				default:
					return ErrorHandler_sig__PoolLCacc;
			}
			if(instance->allocatedAcceleratorCount__Pool == 1)
			{
				return StartEverythingHandler_sig__PoolLCacc;
			}
			else
			{
				return 0;
			}
			break;
		case(LCACC_GAM_ERROR):
			return ErrorHandler_sig__PoolLCacc;
			break;
		case(LCACC_GAM_REVOKE)://assumed thus far never to occur
			return ErrorHandler_sig__PoolLCacc;
			break;
		default:
			return ErrorHandler_sig__PoolLCacc;
	}
}
inline void Cleanup_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance)
{
	int i;
	LWI_UnregisterInterruptHandler(instance->GAM_INTERACTION.threadID, 0);
	if(instance->allocatedAcceleratorIDSet__Pool[0] != 0)
	{
		LCAcc_Free(instance->HandlerArgs__p.threadID, instance->HandlerArgs__p.lcaccID);
		LWI_UnregisterInterruptHandler(instance->HandlerArgs__p.threadID, instance->HandlerArgs__p.lcaccID);
	}
}
inline void StopEverythingHandler_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance)
{
	Cleanup_sig__PoolLCacc(instance);
}
inline void ErrorHandler_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance)
{
	Cleanup_sig__PoolLCacc(instance);
	simics_assert(0);
}
inline void (*ProgressHandler_sig__PoolLCacc__p(InstanceData_sig__PoolLCacc* instance, InterruptArgs* args))(InstanceData_sig__PoolLCacc*)
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
				return StopEverythingHandler_sig__PoolLCacc;
			}
			else
			{
				return 0;
			}
		case(LCACC_STATUS_ERROR):
			return ErrorHandler_sig__PoolLCacc;
		default:
			simics_assert(0);
			return 0;
	}
}
inline void StartEverythingHandler_sig__PoolLCacc(InstanceData_sig__PoolLCacc* instance)
{
	int index;
	instance->HandlerArgs__p.threadID = instance->threadID;
	instance->HandlerArgs__p.lcaccID = instance->allocatedAcceleratorIDSet__Pool[0];
	instance->node_p.SetLCAccID(instance->HandlerArgs__p.lcaccID);
	instance->HandlerArgs__p.status = 0;
	instance->HandlerArgs__p.taskIndex = 0;
	instance->HandlerArgs__p.lcaccMode = LCACC_CLASS_SIG__PoolLCacc__p;
	LWI_RegisterInterruptHandler(&(instance->HandlerArgs__p));
	simics_assert(instance->acceleratorSignature__p.IsFinalized());
	LCAcc_Command(instance->HandlerArgs__p.threadID, instance->HandlerArgs__p.lcaccID, LCACC_CMD_BEGIN_TASK_SIGNATURE, instance->acceleratorSignature__p.GetBuffer(), instance->acceleratorSignature__p.GetBufferSize(), 0, 0);
	Wait_sig__PoolLCacc(instance);// wait for everything to finish
}
inline void CreateBuffer_PoolLCacc_sig(int thread, InstanceData_sig__PoolLCacc* instance, float (*in), float (*out), int Fout, int FoutR, int FoutC, int chunkR, int chunkC)
{
	int index, i;
	instance->binBufSize = 0;
	instance->threadID = thread;
	instance->GAM_INTERACTION.threadID = thread;
	instance->GAM_INTERACTION.lcaccID = 0;
	instance->GAM_INTERACTION.lcaccMode = 0;
	instance->node_p.SetSPMConfig((sizeof(float) * (1) * (chunkR * 2) * (chunkC * 2)) + (sizeof(float) * (1) * (chunkR) * (chunkC)), 3, 0);
	instance->LCAcc_FuncArgs__in = in;
	instance->LCAcc_FuncArgs__out = out;
	instance->LCAcc_FuncVars__Fout = Fout;
	instance->LCAcc_FuncVars__FoutR = FoutR;
	instance->LCAcc_FuncVars__FoutC = FoutC;
	instance->LCAcc_FuncVars__chunkR = chunkR;
	instance->LCAcc_FuncVars__chunkC = chunkC;
	instance->allocatedAcceleratorCount__Pool = 0;
	LCAccNode& VNR_vardecl_0(instance->node_p);
	void* VNR_vardecl_1(instance->LCAcc_FuncArgs__in);
	void* VNR_vardecl_2(instance->LCAcc_FuncArgs__out);
	std::vector<uint32_t> VNR_vardecl_3;
	VNR_vardecl_3.push_back(((1) - (0)) / (1));
	VNR_vardecl_3.push_back(((chunkR * 2 - 1) - (0)) / (2));
	VNR_vardecl_3.push_back(((chunkC * 2 - 1) - (0)) / (2));
	std::vector<int32_t> VNR_vardecl_4;
	VNR_vardecl_4.push_back((1) * (sizeof(float) * (chunkR * 2) * (chunkC * 2)));
	VNR_vardecl_4.push_back((2) * (sizeof(float) * (chunkC * 2)));
	VNR_vardecl_4.push_back((2) * (sizeof(float)));
	MicroprogramWriter::ComputeArgIndex VNR_vardecl_5((0) + ((((0) * (sizeof(float) * (chunkR * 2) * (chunkC * 2))) + ((0) * (sizeof(float) * (chunkC * 2))) + ((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_3, VNR_vardecl_4);
	std::vector<uint32_t> VNR_vardecl_6;
	VNR_vardecl_6.push_back(((1) - (0)) / (1));
	VNR_vardecl_6.push_back(((chunkR * 2 - 1) - (0)) / (2));
	VNR_vardecl_6.push_back(((chunkC * 2) - (1)) / (2));
	MicroprogramWriter::ComputeArgIndex VNR_vardecl_7((0) + ((((0) * (sizeof(float) * (chunkR * 2) * (chunkC * 2))) + ((0) * (sizeof(float) * (chunkC * 2))) + ((1) * (sizeof(float))))), sizeof(float), VNR_vardecl_6, VNR_vardecl_4);
	std::vector<uint32_t> VNR_vardecl_8;
	VNR_vardecl_8.push_back(((1) - (0)) / (1));
	VNR_vardecl_8.push_back(((chunkR * 2) - (1)) / (2));
	VNR_vardecl_8.push_back(((chunkC * 2 - 1) - (0)) / (2));
	MicroprogramWriter::ComputeArgIndex VNR_vardecl_9((0) + ((((0) * (sizeof(float) * (chunkR * 2) * (chunkC * 2))) + ((1) * (sizeof(float) * (chunkC * 2))) + ((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_8, VNR_vardecl_4);
	std::vector<uint32_t> VNR_vardecl_10;
	VNR_vardecl_10.push_back(((1) - (0)) / (1));
	VNR_vardecl_10.push_back(((chunkR * 2) - (1)) / (2));
	VNR_vardecl_10.push_back(((chunkC * 2) - (1)) / (2));
	MicroprogramWriter::ComputeArgIndex VNR_vardecl_11((0) + ((((0) * (sizeof(float) * (chunkR * 2) * (chunkC * 2))) + ((1) * (sizeof(float) * (chunkC * 2))) + ((1) * (sizeof(float))))), sizeof(float), VNR_vardecl_10, VNR_vardecl_4);
	std::vector<uint32_t> VNR_vardecl_12;
	VNR_vardecl_12.push_back(((1) - (0)) / (1));
	VNR_vardecl_12.push_back(((chunkR) - (0)) / (1));
	VNR_vardecl_12.push_back(((chunkC) - (0)) / (1));
	std::vector<int32_t> VNR_vardecl_13;
	VNR_vardecl_13.push_back((1) * (sizeof(float) * (chunkR) * (chunkC)));
	VNR_vardecl_13.push_back((1) * (sizeof(float) * (chunkC)));
	VNR_vardecl_13.push_back((1) * (sizeof(float)));
	MicroprogramWriter::ComputeArgIndex VNR_vardecl_14(((sizeof(float) * (1) * (chunkR * 2) * (chunkC * 2))) + ((((0) * (sizeof(float) * (chunkR) * (chunkC))) + ((0) * (sizeof(float) * (chunkC))) + ((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_12, VNR_vardecl_13);
	std::vector<MicroprogramWriter::ComputeArgIndex> VNR_vardecl_15;
	VNR_vardecl_15.push_back(VNR_vardecl_5);
	VNR_vardecl_15.push_back(VNR_vardecl_7);
	VNR_vardecl_15.push_back(VNR_vardecl_9);
	VNR_vardecl_15.push_back(VNR_vardecl_11);
	VNR_vardecl_15.push_back(VNR_vardecl_14);
	std::vector<uint64_t> VNR_vardecl_16;
	std::vector<uint32_t> VNR_vardecl_17;
	VNR_vardecl_17.push_back(((Fout) - (0)) / (1));
	VNR_vardecl_17.push_back(((FoutR * 2) - (0)) / (chunkR * 2));
	VNR_vardecl_17.push_back(((FoutC * 2) - (0)) / (chunkC * 2));
	std::vector<int32_t> VNR_vardecl_18;
	VNR_vardecl_18.push_back((1) * ((sizeof(float) * (FoutR * 2) * (FoutC * 2))));
	VNR_vardecl_18.push_back((chunkR * 2) * ((sizeof(float) * (FoutC * 2))));
	VNR_vardecl_18.push_back((chunkC * 2) * ((sizeof(float))));
	std::vector<uint32_t> VNR_vardecl_19;
	VNR_vardecl_19.push_back(((1) - (0)) / (1));
	VNR_vardecl_19.push_back(((chunkR * 2) - (0)) / (1));
	VNR_vardecl_19.push_back(((chunkC * 2) - (0)) / (1));
	std::vector<int32_t> VNR_vardecl_20;
	VNR_vardecl_20.push_back((1) * ((sizeof(float) * (FoutR * 2) * (FoutC * 2))));
	VNR_vardecl_20.push_back((1) * ((sizeof(float) * (FoutC * 2))));
	VNR_vardecl_20.push_back((1) * ((sizeof(float))));
	std::vector<int32_t> VNR_vardecl_21;
	VNR_vardecl_21.push_back((1) * ((sizeof(float)) * (chunkR * 2) * (chunkC * 2)));
	VNR_vardecl_21.push_back((1) * ((sizeof(float)) * (chunkC * 2)));
	VNR_vardecl_21.push_back((1) * ((sizeof(float))));
	std::vector<int32_t> VNR_vardecl_22;
	VNR_vardecl_22.push_back((1) * ((sizeof(float)) * (chunkR) * (chunkC)));
	VNR_vardecl_22.push_back((1) * ((sizeof(float)) * (chunkC)));
	VNR_vardecl_22.push_back((1) * ((sizeof(float))));
	std::vector<int32_t> VNR_vardecl_23;
	VNR_vardecl_23.push_back((1) * ((sizeof(float) * (FoutR) * (FoutC))));
	VNR_vardecl_23.push_back((chunkR * 2) * ((sizeof(float) * (FoutC))));
	VNR_vardecl_23.push_back((chunkC * 2) * ((sizeof(float))));
	std::vector<int32_t> VNR_vardecl_24;
	VNR_vardecl_24.push_back((1) * ((sizeof(float) * (FoutR) * (FoutC))));
	VNR_vardecl_24.push_back((1) * ((sizeof(float) * (FoutC))));
	VNR_vardecl_24.push_back((1) * ((sizeof(float))));
	//produce compute set
	//See VNR_vardecl_15 for index variable decl
	//See VNR_vardecl_16 for register set decl
	instance->acceleratorSignature__p.AddCompute(VNR_vardecl_0, VNR_vardecl_15, VNR_vardecl_16);
	//produce transfer set
	//transfer from in to p
	//Search VNR_vardecl_17 for source block size.
	//Search VNR_vardecl_18 for source block stride.
	//Search VNR_vardecl_19 for source element size.
	//Search VNR_vardecl_20 for source element stride.
	//Search VNR_vardecl_19 for destination size.
	//Search VNR_vardecl_21 for destination stride.
	instance->acceleratorSignature__p.AddTransfer(VNR_vardecl_1, VNR_vardecl_17, VNR_vardecl_18, VNR_vardecl_19, VNR_vardecl_20, VNR_vardecl_0, (0) + ((((0) * ((sizeof(float)) * (chunkR * 2) * (chunkC * 2))) + ((0) * ((sizeof(float)) * (chunkC * 2))) + ((0) * ((sizeof(float)))))), VNR_vardecl_19, VNR_vardecl_21, sizeof(float));
	//transfer from p to out
	//Search VNR_vardecl_12 for source size.
	//Search VNR_vardecl_22 for source stride.
	//Search VNR_vardecl_17 for destination block size.
	//Search VNR_vardecl_23 for destination block stride.
	//Search VNR_vardecl_12 for destination element size.
	//Search VNR_vardecl_24 for destination element stride.
	instance->acceleratorSignature__p.AddTransfer(VNR_vardecl_0, ((sizeof(float) * (1) * (chunkR * 2) * (chunkC * 2))) + ((((0) * ((sizeof(float)) * (chunkR) * (chunkC))) + ((0) * ((sizeof(float)) * (chunkC))) + ((0) * ((sizeof(float)))))), VNR_vardecl_12, VNR_vardecl_22, VNR_vardecl_2, VNR_vardecl_17, VNR_vardecl_23, VNR_vardecl_12, VNR_vardecl_24, sizeof(float));
	instance->acceleratorSignature__p.Finalize((((Fout) - (0)) / (1)) * (((FoutR * 2) - (0)) / (chunkR * 2)) * (((FoutC * 2) - (0)) / (chunkC * 2)));
}
inline void PoolLCacc_sig_buf(int threadID, InstanceData_sig__PoolLCacc* instance)
{
	instance->Reset();
	LWI_RegisterInterruptHandler(&(instance->GAM_INTERACTION));
	LCAcc_Request(threadID, 250, 1);//request something of type AcceleratorType
	Wait_sig__PoolLCacc(instance);// wait for everything to finish
}
inline void PoolLCacc_sig(int threadID, float (*in), float (*out), int Fout, int FoutR, int FoutC, int chunkR, int chunkC)
{
	InstanceData_sig__PoolLCacc instance;
	CreateBuffer_PoolLCacc_sig(threadID, &instance, in, out, Fout, FoutR, FoutC, chunkR, chunkC);
	PoolLCacc_sig_buf(threadID, &instance);
}
class BiN_PoolLCacc_Arbitrator_sig
{
	std::vector<InstanceData_sig__PoolLCacc*> instanceSet;
	std::vector<uint32_t> performancePoint;
	std::vector<uint32_t> cachePressureMod;
	std::vector<uint32_t> ops;
	std::vector<uint32_t> count;
	int threadID;
	int allocatedAcceleratorCount__Pool;
	int allocatedAcceleratorIDSet__Pool[1];
	InterruptArgs isr;
public:
	inline BiN_PoolLCacc_Arbitrator_sig(int thread)
	{
		ops.push_back(250);
		count.push_back(1);
		threadID = thread;
	}
	inline void AddConfig(InstanceData_sig__PoolLCacc* inst, uint32_t performance, uint32_t cacheMod)
	{
		simics_assert(inst->acceleratorSignature__p.IsFinalized());
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
		int allocatedAcceleratorCount__Pool = 0;
		LCAcc_Request(threadID, 250, 1);//request something of type AcceleratorType
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
							case(250)://Mode: Pool
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
							case(250):
								simics_assert(allocatedAcceleratorCount__Pool < 1);
								allocatedAcceleratorIDSet__Pool[allocatedAcceleratorCount__Pool] = lcaccID;
								allocatedAcceleratorCount__Pool++;
								break;
							default:
								simics_assert(0);
						}
						if(allocatedAcceleratorCount__Pool == 1)
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
		InstanceData_sig__PoolLCacc* inst = NULL;
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
			inst->allocatedAcceleratorIDSet__Pool[i] = allocatedAcceleratorIDSet__Pool[i];
		}
		inst->allocatedAcceleratorCount__Pool = allocatedAcceleratorCount__Pool;
		inst->pendingAccelerators = 1;
		StartEverythingHandler_sig__PoolLCacc(inst);
	}
};

#endif
#ifndef LCACC_BODY_TD__PoolLCacc__X
#define LCACC_BODY_TD__PoolLCacc__X

inline void Wait_td__PoolLCacc(int thread)
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
inline void CreateBuffer_PoolLCacc_td(uint8_t** buffer, uint32_t* bufferSize, uint8_t** constCluster, int thread, float (*in), float (*out), int Fout, int FoutR, int FoutC, int chunkR, int chunkC)
{
	MicroprogramWriter mw(false);
	void* LCAcc_FuncArgs__in = in;
	void* LCAcc_FuncArgs__out = out;
	{
		void* VNR_vardecl_0(in);
		void* VNR_vardecl_1(out);
		LCAccNode VNR_vardecl_2(250, (sizeof(float) * (1) * (chunkR * 2) * (chunkC * 2)) + (sizeof(float) * (1) * (chunkR) * (chunkC)), 3, 0, 0);
		std::vector<uint32_t> VNR_vardecl_3;
		VNR_vardecl_3.push_back(((1) - (0)) / (1));
		VNR_vardecl_3.push_back(((chunkR * 2 - 1) - (0)) / (2));
		VNR_vardecl_3.push_back(((chunkC * 2 - 1) - (0)) / (2));
		std::vector<int32_t> VNR_vardecl_4;
		VNR_vardecl_4.push_back((1) * (sizeof(float) * (chunkR * 2) * (chunkC * 2)));
		VNR_vardecl_4.push_back((2) * (sizeof(float) * (chunkC * 2)));
		VNR_vardecl_4.push_back((2) * (sizeof(float)));
		MicroprogramWriter::ComputeArgIndex VNR_vardecl_5((0) + ((((0) * (sizeof(float) * (chunkR * 2) * (chunkC * 2))) + ((0) * (sizeof(float) * (chunkC * 2))) + ((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_3, VNR_vardecl_4);
		std::vector<uint32_t> VNR_vardecl_6;
		VNR_vardecl_6.push_back(((1) - (0)) / (1));
		VNR_vardecl_6.push_back(((chunkR * 2 - 1) - (0)) / (2));
		VNR_vardecl_6.push_back(((chunkC * 2) - (1)) / (2));
		MicroprogramWriter::ComputeArgIndex VNR_vardecl_7((0) + ((((0) * (sizeof(float) * (chunkR * 2) * (chunkC * 2))) + ((0) * (sizeof(float) * (chunkC * 2))) + ((1) * (sizeof(float))))), sizeof(float), VNR_vardecl_6, VNR_vardecl_4);
		std::vector<uint32_t> VNR_vardecl_8;
		VNR_vardecl_8.push_back(((1) - (0)) / (1));
		VNR_vardecl_8.push_back(((chunkR * 2) - (1)) / (2));
		VNR_vardecl_8.push_back(((chunkC * 2 - 1) - (0)) / (2));
		MicroprogramWriter::ComputeArgIndex VNR_vardecl_9((0) + ((((0) * (sizeof(float) * (chunkR * 2) * (chunkC * 2))) + ((1) * (sizeof(float) * (chunkC * 2))) + ((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_8, VNR_vardecl_4);
		std::vector<uint32_t> VNR_vardecl_10;
		VNR_vardecl_10.push_back(((1) - (0)) / (1));
		VNR_vardecl_10.push_back(((chunkR * 2) - (1)) / (2));
		VNR_vardecl_10.push_back(((chunkC * 2) - (1)) / (2));
		MicroprogramWriter::ComputeArgIndex VNR_vardecl_11((0) + ((((0) * (sizeof(float) * (chunkR * 2) * (chunkC * 2))) + ((1) * (sizeof(float) * (chunkC * 2))) + ((1) * (sizeof(float))))), sizeof(float), VNR_vardecl_10, VNR_vardecl_4);
		std::vector<uint32_t> VNR_vardecl_12;
		VNR_vardecl_12.push_back(((1) - (0)) / (1));
		VNR_vardecl_12.push_back(((chunkR) - (0)) / (1));
		VNR_vardecl_12.push_back(((chunkC) - (0)) / (1));
		std::vector<int32_t> VNR_vardecl_13;
		VNR_vardecl_13.push_back((1) * (sizeof(float) * (chunkR) * (chunkC)));
		VNR_vardecl_13.push_back((1) * (sizeof(float) * (chunkC)));
		VNR_vardecl_13.push_back((1) * (sizeof(float)));
		MicroprogramWriter::ComputeArgIndex VNR_vardecl_14(((sizeof(float) * (1) * (chunkR * 2) * (chunkC * 2))) + ((((0) * (sizeof(float) * (chunkR) * (chunkC))) + ((0) * (sizeof(float) * (chunkC))) + ((0) * (sizeof(float))))), sizeof(float), VNR_vardecl_12, VNR_vardecl_13);
		std::vector<MicroprogramWriter::ComputeArgIndex> VNR_vardecl_15;
		VNR_vardecl_15.push_back(VNR_vardecl_5);
		VNR_vardecl_15.push_back(VNR_vardecl_7);
		VNR_vardecl_15.push_back(VNR_vardecl_9);
		VNR_vardecl_15.push_back(VNR_vardecl_11);
		VNR_vardecl_15.push_back(VNR_vardecl_14);
		std::vector<uint64_t> VNR_vardecl_16;
		std::vector<uint32_t> VNR_vardecl_17;
		VNR_vardecl_17.push_back(((Fout) - (0)) / (1));
		VNR_vardecl_17.push_back(((FoutR * 2) - (0)) / (chunkR * 2));
		VNR_vardecl_17.push_back(((FoutC * 2) - (0)) / (chunkC * 2));
		std::vector<int32_t> VNR_vardecl_18;
		VNR_vardecl_18.push_back((1) * ((sizeof(float) * (FoutR * 2) * (FoutC * 2))));
		VNR_vardecl_18.push_back((chunkR * 2) * ((sizeof(float) * (FoutC * 2))));
		VNR_vardecl_18.push_back((chunkC * 2) * ((sizeof(float))));
		std::vector<uint32_t> VNR_vardecl_19;
		VNR_vardecl_19.push_back(((1) - (0)) / (1));
		VNR_vardecl_19.push_back(((chunkR * 2) - (0)) / (1));
		VNR_vardecl_19.push_back(((chunkC * 2) - (0)) / (1));
		std::vector<int32_t> VNR_vardecl_20;
		VNR_vardecl_20.push_back((1) * ((sizeof(float) * (FoutR * 2) * (FoutC * 2))));
		VNR_vardecl_20.push_back((1) * ((sizeof(float) * (FoutC * 2))));
		VNR_vardecl_20.push_back((1) * ((sizeof(float))));
		std::vector<int32_t> VNR_vardecl_21;
		VNR_vardecl_21.push_back((1) * ((sizeof(float)) * (chunkR * 2) * (chunkC * 2)));
		VNR_vardecl_21.push_back((1) * ((sizeof(float)) * (chunkC * 2)));
		VNR_vardecl_21.push_back((1) * ((sizeof(float))));
		std::vector<int32_t> VNR_vardecl_22;
		VNR_vardecl_22.push_back((1) * ((sizeof(float)) * (chunkR) * (chunkC)));
		VNR_vardecl_22.push_back((1) * ((sizeof(float)) * (chunkC)));
		VNR_vardecl_22.push_back((1) * ((sizeof(float))));
		std::vector<int32_t> VNR_vardecl_23;
		VNR_vardecl_23.push_back((1) * ((sizeof(float) * (FoutR) * (FoutC))));
		VNR_vardecl_23.push_back((chunkR * 2) * ((sizeof(float) * (FoutC))));
		VNR_vardecl_23.push_back((chunkC * 2) * ((sizeof(float))));
		std::vector<int32_t> VNR_vardecl_24;
		VNR_vardecl_24.push_back((1) * ((sizeof(float) * (FoutR) * (FoutC))));
		VNR_vardecl_24.push_back((1) * ((sizeof(float) * (FoutC))));
		VNR_vardecl_24.push_back((1) * ((sizeof(float))));
		//See VNR_vardecl_15 for index variable decl
		//See VNR_vardecl_16 for register set decl
		mw.AddCompute(VNR_vardecl_2, VNR_vardecl_15, VNR_vardecl_16);
		
		//transfer from in to p
		//Search VNR_vardecl_17 for source block size.
		//Search VNR_vardecl_18 for source block stride.
		//Search VNR_vardecl_19 for source element size.
		//Search VNR_vardecl_20 for source element stride.
		//Search VNR_vardecl_19 for destination size.
		//Search VNR_vardecl_21 for destination stride.
		mw.AddTransfer(VNR_vardecl_0, VNR_vardecl_17, VNR_vardecl_18, VNR_vardecl_19, VNR_vardecl_20, VNR_vardecl_2, (0) + ((((0) * ((sizeof(float)) * (chunkR * 2) * (chunkC * 2))) + ((0) * ((sizeof(float)) * (chunkC * 2))) + ((0) * ((sizeof(float)))))), VNR_vardecl_19, VNR_vardecl_21, sizeof(float));
		
		//transfer from p to out
		//Search VNR_vardecl_12 for source size.
		//Search VNR_vardecl_22 for source stride.
		//Search VNR_vardecl_17 for destination block size.
		//Search VNR_vardecl_23 for destination block stride.
		//Search VNR_vardecl_12 for destination element size.
		//Search VNR_vardecl_24 for destination element stride.
		mw.AddTransfer(VNR_vardecl_2, ((sizeof(float) * (1) * (chunkR * 2) * (chunkC * 2))) + ((((0) * ((sizeof(float)) * (chunkR) * (chunkC))) + ((0) * ((sizeof(float)) * (chunkC))) + ((0) * ((sizeof(float)))))), VNR_vardecl_12, VNR_vardecl_22, VNR_vardecl_1, VNR_vardecl_17, VNR_vardecl_23, VNR_vardecl_12, VNR_vardecl_24, sizeof(float));
		
		mw.SetTaskGrain(0);
		mw.Finalize((((Fout) - (0)) / (1)) * (((FoutR * 2) - (0)) / (chunkR * 2)) * (((FoutC * 2) - (0)) / (chunkC * 2)));
	}
	*buffer = new uint8_t[mw.GetBufferSize()];
	*bufferSize = mw.GetBufferSize();
	memcpy(*buffer, mw.GetBuffer(), mw.GetBufferSize());
	Touch(thread, *buffer, *bufferSize);
}
inline void PoolLCacc_td_buf(uint8_t* buf, uint32_t bufSize, int thread)
{
	InterruptArgs isrArgs;
	isrArgs.threadID = thread;
	isrArgs.lcaccID = 0;
	isrArgs.lcaccMode = 0;
	LWI_RegisterInterruptHandler(&isrArgs);
	LCAcc_Command(thread, isrArgs.lcaccID, LCACC_CMD_BEGIN_PROGRAM, buf, bufSize, 0, 0);
	Wait_td__PoolLCacc(thread);// wait for everything to finish
}
void PoolLCacc_td(int thread, float (*in), float (*out), int Fout, int FoutR, int FoutC, int chunkR, int chunkC)
{
	uint32_t bufSize;
	uint8_t* buffer;
	uint8_t* constCluster = NULL;
	CreateBuffer_PoolLCacc_td(&buffer, &bufSize, &constCluster, thread, in, out, Fout, FoutR, FoutC, chunkR, chunkC);
	PoolLCacc_td_buf(buffer, bufSize, thread);
	if(constCluster)
	{
		delete [] constCluster;
	}
	delete [] buffer;
}
inline uint32_t PoolLCacc_CalculateBiNSize(int Fout, int FoutR, int FoutC, int chunkR, int chunkC)
{
	return 0;
}
class BiN_PoolLCacc_Arbitrator_td
{
	std::vector<uint8_t*> bufSet;
	std::vector<uint32_t> bufSizeSet;
	std::vector<uint32_t> binSizeSet;
	std::vector<uint32_t> performancePoint;
	std::vector<uint32_t> cachePressureMod;
	InterruptArgs isr;
public:
	inline BiN_PoolLCacc_Arbitrator_td(){}
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
		LCAcc_DeclareLCAccUse(threadID, 250, 1); //requests for Pool
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
				PoolLCacc_td_buf(bufSet[i], bufSizeSet[i], threadID);
				return;
			}
		}
		simics_assert(0);
	}
};

#endif
