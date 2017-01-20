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
#ifndef LCACC_BODY_SIG__TexSynth1LCacc__X
#define LCACC_BODY_SIG__TexSynth1LCacc__X
#define LCACC_CLASS_SIG__TexSynth1LCacc__t 1
class InstanceData_sig__TexSynth1LCacc
{
public:
	InterruptArgs GAM_INTERACTION;
	int threadID;
	uint32_t binBufSize;
	unsigned int pendingAccelerators;
	unsigned int reservedAccelerators;
	unsigned int acceleratorVectorLength;
	int allocatedAcceleratorCount__TexSynth1;
	int allocatedAcceleratorIDSet__TexSynth1[1];
	int (*LCAcc_FuncArgs__atlas)[2];
	float (*LCAcc_FuncArgs__resultImage);
	uint32_t LCAcc_FuncVars__randSeed[100];
	int LCAcc_FuncVars__inHeight;
	int LCAcc_FuncVars__inWidth;
	int LCAcc_FuncVars__outHeight;
	int LCAcc_FuncVars__outWidth;
	int LCAcc_FuncVars__imageCount;
	intptr_t LCAcc_FuncVars__imageArrayStart;
	int LCAcc_FuncVars__chunk;
	MicroprogramWriter acceleratorSignature__t;
	LCAccNode node_t;
	InterruptArgs HandlerArgs__t;
	inline void Reset()
	{
		pendingAccelerators = 0;
		allocatedAcceleratorCount__TexSynth1 = 0;
		reservedAccelerators = 0;
		allocatedAcceleratorIDSet__TexSynth1[0] = 0;
		HandlerArgs__t.threadID = threadID;
		HandlerArgs__t.status = 0;
		HandlerArgs__t.taskIndex = 0;
		HandlerArgs__t.lcaccMode = LCACC_CLASS_SIG__TexSynth1LCacc__t;
		node_t.Reset();
	}
	inline InstanceData_sig__TexSynth1LCacc() :
		acceleratorSignature__t(true), 
		node_t(910), 
		threadID(0)
	{
		Reset();
	}
};
inline void (*GAMHandler_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance, InterruptArgs* args))(InstanceData_sig__TexSynth1LCacc*) ;
inline void Cleanup_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance);
inline void StartEverythingHandler_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance);
inline void StopEverythingHandler_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance);
inline void ErrorHandler_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance);
inline void Wait_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance);
inline unsigned int DelayEstimate_sig__TexSynth1LCacc(int vectorSize)
{
	return 1;
}
inline void (*ProgressHandler_sig__TexSynth1LCacc__t(InstanceData_sig__TexSynth1LCacc* instance, InterruptArgs* args))(InstanceData_sig__TexSynth1LCacc*);
//This is a procedurally generated interrupt-based program from a RAGraph data structure for function TexSynth1LCacc
inline void Wait_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance)
{
	while(1)
	{
		int thread = instance->threadID;
		InterruptArgs* args = 0;
		void (*progress)(InstanceData_sig__TexSynth1LCacc*);
		while((args = LWI_CheckInterrupt(thread)) == 0);
		switch(args->lcaccMode)
		{
			case(0):
				progress = GAMHandler_sig__TexSynth1LCacc(instance, args);
				break;
			case(LCACC_CLASS_SIG__TexSynth1LCacc__t):
				progress = ProgressHandler_sig__TexSynth1LCacc__t(instance, args);
				break;
			default:
				ErrorHandler_sig__TexSynth1LCacc(instance);
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
inline void (*GAMHandler_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance, InterruptArgs* args))(InstanceData_sig__TexSynth1LCacc*) 
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
				case(910)://Mode: TexSynth1
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
				LCAcc_Reserve(args->threadID, 0, DelayEstimate_sig__TexSynth1LCacc(instance->acceleratorVectorLength));
			}
			return 0;
			break;
		case(LCACC_GAM_GRANT):
			lcaccMode = args->v[0];
			lcaccID = args->v[1];
			switch(lcaccMode)
			{
				case(910):
					if(instance->allocatedAcceleratorCount__TexSynth1 == 1)
					{
						return ErrorHandler_sig__TexSynth1LCacc;
					}
					else
					{
						instance->pendingAccelerators++;
						instance->allocatedAcceleratorIDSet__TexSynth1[instance->allocatedAcceleratorCount__TexSynth1] = lcaccID;
						instance->allocatedAcceleratorCount__TexSynth1++;
					}
					break;
				default:
					return ErrorHandler_sig__TexSynth1LCacc;
			}
			if(instance->allocatedAcceleratorCount__TexSynth1 == 1)
			{
				return StartEverythingHandler_sig__TexSynth1LCacc;
			}
			else
			{
				return 0;
			}
			break;
		case(LCACC_GAM_ERROR):
			return ErrorHandler_sig__TexSynth1LCacc;
			break;
		case(LCACC_GAM_REVOKE)://assumed thus far never to occur
			return ErrorHandler_sig__TexSynth1LCacc;
			break;
		default:
			return ErrorHandler_sig__TexSynth1LCacc;
	}
}
inline void Cleanup_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance)
{
	int i;
	LWI_UnregisterInterruptHandler(instance->GAM_INTERACTION.threadID, 0);
	if(instance->allocatedAcceleratorIDSet__TexSynth1[0] != 0)
	{
		LCAcc_Free(instance->HandlerArgs__t.threadID, instance->HandlerArgs__t.lcaccID);
		LWI_UnregisterInterruptHandler(instance->HandlerArgs__t.threadID, instance->HandlerArgs__t.lcaccID);
	}
}
inline void StopEverythingHandler_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance)
{
	Cleanup_sig__TexSynth1LCacc(instance);
}
inline void ErrorHandler_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance)
{
	Cleanup_sig__TexSynth1LCacc(instance);
	simics_assert(0);
}
inline void (*ProgressHandler_sig__TexSynth1LCacc__t(InstanceData_sig__TexSynth1LCacc* instance, InterruptArgs* args))(InstanceData_sig__TexSynth1LCacc*)
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
				return StopEverythingHandler_sig__TexSynth1LCacc;
			}
			else
			{
				return 0;
			}
		case(LCACC_STATUS_ERROR):
			return ErrorHandler_sig__TexSynth1LCacc;
		default:
			simics_assert(0);
			return 0;
	}
}
inline void StartEverythingHandler_sig__TexSynth1LCacc(InstanceData_sig__TexSynth1LCacc* instance)
{
	int index;
	instance->HandlerArgs__t.threadID = instance->threadID;
	instance->HandlerArgs__t.lcaccID = instance->allocatedAcceleratorIDSet__TexSynth1[0];
	instance->node_t.SetLCAccID(instance->HandlerArgs__t.lcaccID);
	instance->HandlerArgs__t.status = 0;
	instance->HandlerArgs__t.taskIndex = 0;
	instance->HandlerArgs__t.lcaccMode = LCACC_CLASS_SIG__TexSynth1LCacc__t;
	LWI_RegisterInterruptHandler(&(instance->HandlerArgs__t));
	simics_assert(instance->acceleratorSignature__t.IsFinalized());
	LCAcc_Command(instance->HandlerArgs__t.threadID, instance->HandlerArgs__t.lcaccID, LCACC_CMD_BEGIN_TASK_SIGNATURE, instance->acceleratorSignature__t.GetBuffer(), instance->acceleratorSignature__t.GetBufferSize(), 0, 0);
	Wait_sig__TexSynth1LCacc(instance);// wait for everything to finish
}
inline void CreateBuffer_TexSynth1LCacc_sig(int thread, InstanceData_sig__TexSynth1LCacc* instance, int (*atlas)[2], float (*resultImage), uint32_t randSeed[100], int inHeight, int inWidth, int outHeight, int outWidth, int imageCount, intptr_t imageArrayStart, int chunk)
{
	int index, i;
	instance->binBufSize = ((sizeof(float) * (1) * (1) * (chunk)) + (sizeof(int[2]) * (1) * (1) * (chunk))) * (3);
	instance->threadID = thread;
	instance->GAM_INTERACTION.threadID = thread;
	instance->GAM_INTERACTION.lcaccID = 0;
	instance->GAM_INTERACTION.lcaccMode = 0;
	instance->node_t.SetSPMConfig((sizeof(float) * (1) * (1) * (chunk)) + (sizeof(int[2]) * (1) * (1) * (chunk)), 3, 0);
	instance->LCAcc_FuncArgs__atlas = atlas;
	instance->LCAcc_FuncArgs__resultImage = resultImage;
	memcpy(instance->LCAcc_FuncVars__randSeed, randSeed, sizeof(uint32_t[100]));
	instance->LCAcc_FuncVars__inHeight = inHeight;
	instance->LCAcc_FuncVars__inWidth = inWidth;
	instance->LCAcc_FuncVars__outHeight = outHeight;
	instance->LCAcc_FuncVars__outWidth = outWidth;
	instance->LCAcc_FuncVars__imageCount = imageCount;
	instance->LCAcc_FuncVars__imageArrayStart = imageArrayStart;
	instance->LCAcc_FuncVars__chunk = chunk;
	instance->allocatedAcceleratorCount__TexSynth1 = 0;
	LCAccNode& VNR_vardecl_0(instance->node_t);
	void* VNR_vardecl_1(instance->LCAcc_FuncArgs__resultImage);
	void* VNR_vardecl_2(instance->LCAcc_FuncArgs__atlas);
	std::vector<uint32_t> VNR_vardecl_3;
	VNR_vardecl_3.push_back(((1) - (0)) / (1));
	VNR_vardecl_3.push_back(((1) - (0)) / (1));
	VNR_vardecl_3.push_back(((chunk) - (0)) / (1));
	std::vector<int32_t> VNR_vardecl_4;
	VNR_vardecl_4.push_back((1) * (sizeof(int[2]) * (1) * (chunk)));
	VNR_vardecl_4.push_back((1) * (sizeof(int[2]) * (chunk)));
	VNR_vardecl_4.push_back((1) * (sizeof(int[2])));
	MicroprogramWriter::ComputeArgIndex VNR_vardecl_5(((sizeof(float) * (1) * (1) * (chunk))) + ((((0) * (sizeof(int[2]) * (1) * (chunk))) + ((0) * (sizeof(int[2]) * (chunk))) + ((0) * (sizeof(int[2]))))), sizeof(int), VNR_vardecl_3, VNR_vardecl_4);
	std::vector<MicroprogramWriter::ComputeArgIndex> VNR_vardecl_6;
	VNR_vardecl_6.push_back(VNR_vardecl_5);
	std::vector<uint64_t> VNR_vardecl_7;
	VNR_vardecl_7.push_back(ConvertToType<int, uint64_t>(inWidth)/*Register WIDTHin*/);
	VNR_vardecl_7.push_back(ConvertToType<int, uint64_t>(inHeight)/*Register HEIGHTin*/);
	VNR_vardecl_7.push_back(ConvertToType<int, uint64_t>(outWidth)/*Register WIDTHout*/);
	VNR_vardecl_7.push_back(ConvertToType<int, uint64_t>(outHeight)/*Register HEIGHTout*/);
	VNR_vardecl_7.push_back(ConvertToType<intptr_t, uint64_t>(imageArrayStart)/*Register inputImage*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>((0))/*Register outputResult*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(0) % (100)]) /*Register in_rand_seeds[0]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(1) % (100)]) /*Register in_rand_seeds[1]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(2) % (100)]) /*Register in_rand_seeds[2]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(3) % (100)]) /*Register in_rand_seeds[3]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(4) % (100)]) /*Register in_rand_seeds[4]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(5) % (100)]) /*Register in_rand_seeds[5]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(6) % (100)]) /*Register in_rand_seeds[6]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(7) % (100)]) /*Register in_rand_seeds[7]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(8) % (100)]) /*Register in_rand_seeds[8]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(9) % (100)]) /*Register in_rand_seeds[9]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(10) % (100)]) /*Register in_rand_seeds[10]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(11) % (100)]) /*Register in_rand_seeds[11]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(12) % (100)]) /*Register in_rand_seeds[12]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(13) % (100)]) /*Register in_rand_seeds[13]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(14) % (100)]) /*Register in_rand_seeds[14]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(15) % (100)]) /*Register in_rand_seeds[15]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(16) % (100)]) /*Register in_rand_seeds[16]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(17) % (100)]) /*Register in_rand_seeds[17]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(18) % (100)]) /*Register in_rand_seeds[18]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(19) % (100)]) /*Register in_rand_seeds[19]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(20) % (100)]) /*Register in_rand_seeds[20]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(21) % (100)]) /*Register in_rand_seeds[21]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(22) % (100)]) /*Register in_rand_seeds[22]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(23) % (100)]) /*Register in_rand_seeds[23]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(24) % (100)]) /*Register in_rand_seeds[24]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(25) % (100)]) /*Register in_rand_seeds[25]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(26) % (100)]) /*Register in_rand_seeds[26]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(27) % (100)]) /*Register in_rand_seeds[27]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(28) % (100)]) /*Register in_rand_seeds[28]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(29) % (100)]) /*Register in_rand_seeds[29]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(30) % (100)]) /*Register in_rand_seeds[30]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(31) % (100)]) /*Register in_rand_seeds[31]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(32) % (100)]) /*Register in_rand_seeds[32]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(33) % (100)]) /*Register in_rand_seeds[33]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(34) % (100)]) /*Register in_rand_seeds[34]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(35) % (100)]) /*Register in_rand_seeds[35]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(36) % (100)]) /*Register in_rand_seeds[36]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(37) % (100)]) /*Register in_rand_seeds[37]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(38) % (100)]) /*Register in_rand_seeds[38]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(39) % (100)]) /*Register in_rand_seeds[39]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(40) % (100)]) /*Register in_rand_seeds[40]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(41) % (100)]) /*Register in_rand_seeds[41]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(42) % (100)]) /*Register in_rand_seeds[42]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(43) % (100)]) /*Register in_rand_seeds[43]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(44) % (100)]) /*Register in_rand_seeds[44]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(45) % (100)]) /*Register in_rand_seeds[45]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(46) % (100)]) /*Register in_rand_seeds[46]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(47) % (100)]) /*Register in_rand_seeds[47]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(48) % (100)]) /*Register in_rand_seeds[48]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(49) % (100)]) /*Register in_rand_seeds[49]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(50) % (100)]) /*Register in_rand_seeds[50]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(51) % (100)]) /*Register in_rand_seeds[51]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(52) % (100)]) /*Register in_rand_seeds[52]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(53) % (100)]) /*Register in_rand_seeds[53]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(54) % (100)]) /*Register in_rand_seeds[54]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(55) % (100)]) /*Register in_rand_seeds[55]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(56) % (100)]) /*Register in_rand_seeds[56]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(57) % (100)]) /*Register in_rand_seeds[57]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(58) % (100)]) /*Register in_rand_seeds[58]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(59) % (100)]) /*Register in_rand_seeds[59]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(60) % (100)]) /*Register in_rand_seeds[60]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(61) % (100)]) /*Register in_rand_seeds[61]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(62) % (100)]) /*Register in_rand_seeds[62]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(63) % (100)]) /*Register in_rand_seeds[63]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(64) % (100)]) /*Register in_rand_seeds[64]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(65) % (100)]) /*Register in_rand_seeds[65]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(66) % (100)]) /*Register in_rand_seeds[66]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(67) % (100)]) /*Register in_rand_seeds[67]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(68) % (100)]) /*Register in_rand_seeds[68]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(69) % (100)]) /*Register in_rand_seeds[69]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(70) % (100)]) /*Register in_rand_seeds[70]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(71) % (100)]) /*Register in_rand_seeds[71]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(72) % (100)]) /*Register in_rand_seeds[72]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(73) % (100)]) /*Register in_rand_seeds[73]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(74) % (100)]) /*Register in_rand_seeds[74]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(75) % (100)]) /*Register in_rand_seeds[75]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(76) % (100)]) /*Register in_rand_seeds[76]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(77) % (100)]) /*Register in_rand_seeds[77]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(78) % (100)]) /*Register in_rand_seeds[78]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(79) % (100)]) /*Register in_rand_seeds[79]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(80) % (100)]) /*Register in_rand_seeds[80]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(81) % (100)]) /*Register in_rand_seeds[81]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(82) % (100)]) /*Register in_rand_seeds[82]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(83) % (100)]) /*Register in_rand_seeds[83]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(84) % (100)]) /*Register in_rand_seeds[84]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(85) % (100)]) /*Register in_rand_seeds[85]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(86) % (100)]) /*Register in_rand_seeds[86]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(87) % (100)]) /*Register in_rand_seeds[87]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(88) % (100)]) /*Register in_rand_seeds[88]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(89) % (100)]) /*Register in_rand_seeds[89]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(90) % (100)]) /*Register in_rand_seeds[90]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(91) % (100)]) /*Register in_rand_seeds[91]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(92) % (100)]) /*Register in_rand_seeds[92]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(93) % (100)]) /*Register in_rand_seeds[93]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(94) % (100)]) /*Register in_rand_seeds[94]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(95) % (100)]) /*Register in_rand_seeds[95]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(96) % (100)]) /*Register in_rand_seeds[96]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(97) % (100)]) /*Register in_rand_seeds[97]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(98) % (100)]) /*Register in_rand_seeds[98]*/);
	VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(99) % (100)]) /*Register in_rand_seeds[99]*/);
	std::vector<int32_t> VNR_vardecl_8;
	VNR_vardecl_8.push_back((1) * ((sizeof(float)) * (1) * (chunk)));
	VNR_vardecl_8.push_back((1) * ((sizeof(float)) * (chunk)));
	VNR_vardecl_8.push_back((1) * ((sizeof(float))));
	std::vector<uint32_t> VNR_vardecl_9;
	VNR_vardecl_9.push_back(((imageCount) - (0)) / (1));
	VNR_vardecl_9.push_back(((outHeight) - (0)) / (1));
	VNR_vardecl_9.push_back(((outWidth) - (0)) / (chunk));
	std::vector<int32_t> VNR_vardecl_10;
	VNR_vardecl_10.push_back((1) * ((sizeof(float) * (outHeight) * (outWidth))));
	VNR_vardecl_10.push_back((1) * ((sizeof(float) * (outWidth))));
	VNR_vardecl_10.push_back((chunk) * ((sizeof(float))));
	std::vector<int32_t> VNR_vardecl_11;
	VNR_vardecl_11.push_back((1) * ((sizeof(float) * (outHeight) * (outWidth))));
	VNR_vardecl_11.push_back((1) * ((sizeof(float) * (outWidth))));
	VNR_vardecl_11.push_back((1) * ((sizeof(float))));
	std::vector<uint32_t> VNR_vardecl_12;
	VNR_vardecl_12.push_back(((1) - (0)) / (1));
	VNR_vardecl_12.push_back(((1) - (0)) / (1));
	VNR_vardecl_12.push_back(((chunk) - (0)) / (1));
	VNR_vardecl_12.push_back((2));
	std::vector<int32_t> VNR_vardecl_13;
	VNR_vardecl_13.push_back((1) * ((sizeof(int[2])) * (1) * (chunk)));
	VNR_vardecl_13.push_back((1) * ((sizeof(int[2])) * (chunk)));
	VNR_vardecl_13.push_back((1) * ((sizeof(int[2]))));
	VNR_vardecl_13.push_back((sizeof(int)));
	std::vector<int32_t> VNR_vardecl_14;
	VNR_vardecl_14.push_back((1) * ((sizeof(int[2]) * (outHeight) * (outWidth))));
	VNR_vardecl_14.push_back((1) * ((sizeof(int[2]) * (outWidth))));
	VNR_vardecl_14.push_back((chunk) * ((sizeof(int[2]))));
	std::vector<int32_t> VNR_vardecl_15;
	VNR_vardecl_15.push_back((1) * ((sizeof(int[2]) * (outHeight) * (outWidth))));
	VNR_vardecl_15.push_back((1) * ((sizeof(int[2]) * (outWidth))));
	VNR_vardecl_15.push_back((1) * ((sizeof(int[2]))));
	VNR_vardecl_15.push_back((sizeof(int)));
	//produce compute set
	//See VNR_vardecl_6 for index variable decl
	//See VNR_vardecl_7 for register set decl
	instance->acceleratorSignature__t.AddCompute(VNR_vardecl_0, VNR_vardecl_6, VNR_vardecl_7);
	//produce transfer set
	//transfer from t to resultImage
	//Search VNR_vardecl_3 for source size.
	//Search VNR_vardecl_8 for source stride.
	//Search VNR_vardecl_9 for destination block size.
	//Search VNR_vardecl_10 for destination block stride.
	//Search VNR_vardecl_3 for destination element size.
	//Search VNR_vardecl_11 for destination element stride.
	instance->acceleratorSignature__t.AddTransfer(VNR_vardecl_0, (0) + ((((0) * ((sizeof(float)) * (1) * (chunk))) + ((0) * ((sizeof(float)) * (chunk))) + ((0) * ((sizeof(float)))))), VNR_vardecl_3, VNR_vardecl_8, VNR_vardecl_1, VNR_vardecl_9, VNR_vardecl_10, VNR_vardecl_3, VNR_vardecl_11, sizeof(float));
	//transfer from t to atlas
	//Search VNR_vardecl_12 for source size.
	//Search VNR_vardecl_13 for source stride.
	//Search VNR_vardecl_9 for destination block size.
	//Search VNR_vardecl_14 for destination block stride.
	//Search VNR_vardecl_12 for destination element size.
	//Search VNR_vardecl_15 for destination element stride.
	instance->acceleratorSignature__t.AddTransfer(VNR_vardecl_0, ((sizeof(float) * (1) * (1) * (chunk))) + ((((0) * ((sizeof(int[2])) * (1) * (chunk))) + ((0) * ((sizeof(int[2])) * (chunk))) + ((0) * ((sizeof(int[2])))))), VNR_vardecl_12, VNR_vardecl_13, VNR_vardecl_2, VNR_vardecl_9, VNR_vardecl_14, VNR_vardecl_12, VNR_vardecl_15, sizeof(int));
	instance->acceleratorSignature__t.Finalize((((imageCount) - (0)) / (1)) * (((outHeight) - (0)) / (1)) * (((outWidth) - (0)) / (chunk)));
}
inline void TexSynth1LCacc_sig_buf(int threadID, InstanceData_sig__TexSynth1LCacc* instance)
{
	instance->Reset();
	LWI_RegisterInterruptHandler(&(instance->GAM_INTERACTION));
	LCAcc_Request(threadID, 910, 1);//request something of type AcceleratorType
	Wait_sig__TexSynth1LCacc(instance);// wait for everything to finish
}
inline void TexSynth1LCacc_sig(int threadID, int (*atlas)[2], float (*resultImage), uint32_t randSeed[100], int inHeight, int inWidth, int outHeight, int outWidth, int imageCount, intptr_t imageArrayStart, int chunk)
{
	InstanceData_sig__TexSynth1LCacc instance;
	CreateBuffer_TexSynth1LCacc_sig(threadID, &instance, atlas, resultImage, randSeed, inHeight, inWidth, outHeight, outWidth, imageCount, imageArrayStart, chunk);
	TexSynth1LCacc_sig_buf(threadID, &instance);
}
class BiN_TexSynth1LCacc_Arbitrator_sig
{
	std::vector<InstanceData_sig__TexSynth1LCacc*> instanceSet;
	std::vector<uint32_t> performancePoint;
	std::vector<uint32_t> cachePressureMod;
	std::vector<uint32_t> ops;
	std::vector<uint32_t> count;
	int threadID;
	int allocatedAcceleratorCount__TexSynth1;
	int allocatedAcceleratorIDSet__TexSynth1[1];
	InterruptArgs isr;
public:
	inline BiN_TexSynth1LCacc_Arbitrator_sig(int thread)
	{
		ops.push_back(910);
		count.push_back(1);
		threadID = thread;
	}
	inline void AddConfig(InstanceData_sig__TexSynth1LCacc* inst, uint32_t performance, uint32_t cacheMod)
	{
		simics_assert(inst->acceleratorSignature__t.IsFinalized());
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
		int allocatedAcceleratorCount__TexSynth1 = 0;
		LCAcc_Request(threadID, 910, 1);//request something of type AcceleratorType
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
							case(910)://Mode: TexSynth1
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
							case(910):
								simics_assert(allocatedAcceleratorCount__TexSynth1 < 1);
								allocatedAcceleratorIDSet__TexSynth1[allocatedAcceleratorCount__TexSynth1] = lcaccID;
								allocatedAcceleratorCount__TexSynth1++;
								break;
							default:
								simics_assert(0);
						}
						if(allocatedAcceleratorCount__TexSynth1 == 1)
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
		InstanceData_sig__TexSynth1LCacc* inst = NULL;
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
			inst->allocatedAcceleratorIDSet__TexSynth1[i] = allocatedAcceleratorIDSet__TexSynth1[i];
		}
		inst->allocatedAcceleratorCount__TexSynth1 = allocatedAcceleratorCount__TexSynth1;
		inst->pendingAccelerators = 1;
		StartEverythingHandler_sig__TexSynth1LCacc(inst);
	}
};

#endif
#ifndef LCACC_BODY_TD__TexSynth1LCacc__X
#define LCACC_BODY_TD__TexSynth1LCacc__X

inline void Wait_td__TexSynth1LCacc(int thread)
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
inline void CreateBuffer_TexSynth1LCacc_td(uint8_t** buffer, uint32_t* bufferSize, uint8_t** constCluster, int thread, int (*atlas)[2], float (*resultImage), uint32_t randSeed[100], int inHeight, int inWidth, int outHeight, int outWidth, int imageCount, intptr_t imageArrayStart, int chunk)
{
	MicroprogramWriter mw(false);
	void* LCAcc_FuncArgs__atlas = atlas;
	void* LCAcc_FuncArgs__resultImage = resultImage;
	{
		void* VNR_vardecl_0(atlas);
		void* VNR_vardecl_1(resultImage);
		LCAccNode VNR_vardecl_2(910, (sizeof(float) * (1) * (1) * (chunk)) + (sizeof(int[2]) * (1) * (1) * (chunk)), 3, 0, 0);
		std::vector<uint32_t> VNR_vardecl_3;
		VNR_vardecl_3.push_back(((1) - (0)) / (1));
		VNR_vardecl_3.push_back(((1) - (0)) / (1));
		VNR_vardecl_3.push_back(((chunk) - (0)) / (1));
		std::vector<int32_t> VNR_vardecl_4;
		VNR_vardecl_4.push_back((1) * (sizeof(int[2]) * (1) * (chunk)));
		VNR_vardecl_4.push_back((1) * (sizeof(int[2]) * (chunk)));
		VNR_vardecl_4.push_back((1) * (sizeof(int[2])));
		MicroprogramWriter::ComputeArgIndex VNR_vardecl_5(((sizeof(float) * (1) * (1) * (chunk))) + ((((0) * (sizeof(int[2]) * (1) * (chunk))) + ((0) * (sizeof(int[2]) * (chunk))) + ((0) * (sizeof(int[2]))))), sizeof(int), VNR_vardecl_3, VNR_vardecl_4);
		std::vector<MicroprogramWriter::ComputeArgIndex> VNR_vardecl_6;
		VNR_vardecl_6.push_back(VNR_vardecl_5);
		std::vector<uint64_t> VNR_vardecl_7;
		VNR_vardecl_7.push_back(ConvertToType<int, uint64_t>(inWidth)/*Register WIDTHin*/);
		VNR_vardecl_7.push_back(ConvertToType<int, uint64_t>(inHeight)/*Register HEIGHTin*/);
		VNR_vardecl_7.push_back(ConvertToType<int, uint64_t>(outWidth)/*Register WIDTHout*/);
		VNR_vardecl_7.push_back(ConvertToType<int, uint64_t>(outHeight)/*Register HEIGHTout*/);
		VNR_vardecl_7.push_back(ConvertToType<intptr_t, uint64_t>(imageArrayStart)/*Register inputImage*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>((0))/*Register outputResult*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(0) % (100)]) /*Register in_rand_seeds[0]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(1) % (100)]) /*Register in_rand_seeds[1]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(2) % (100)]) /*Register in_rand_seeds[2]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(3) % (100)]) /*Register in_rand_seeds[3]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(4) % (100)]) /*Register in_rand_seeds[4]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(5) % (100)]) /*Register in_rand_seeds[5]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(6) % (100)]) /*Register in_rand_seeds[6]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(7) % (100)]) /*Register in_rand_seeds[7]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(8) % (100)]) /*Register in_rand_seeds[8]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(9) % (100)]) /*Register in_rand_seeds[9]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(10) % (100)]) /*Register in_rand_seeds[10]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(11) % (100)]) /*Register in_rand_seeds[11]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(12) % (100)]) /*Register in_rand_seeds[12]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(13) % (100)]) /*Register in_rand_seeds[13]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(14) % (100)]) /*Register in_rand_seeds[14]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(15) % (100)]) /*Register in_rand_seeds[15]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(16) % (100)]) /*Register in_rand_seeds[16]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(17) % (100)]) /*Register in_rand_seeds[17]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(18) % (100)]) /*Register in_rand_seeds[18]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(19) % (100)]) /*Register in_rand_seeds[19]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(20) % (100)]) /*Register in_rand_seeds[20]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(21) % (100)]) /*Register in_rand_seeds[21]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(22) % (100)]) /*Register in_rand_seeds[22]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(23) % (100)]) /*Register in_rand_seeds[23]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(24) % (100)]) /*Register in_rand_seeds[24]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(25) % (100)]) /*Register in_rand_seeds[25]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(26) % (100)]) /*Register in_rand_seeds[26]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(27) % (100)]) /*Register in_rand_seeds[27]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(28) % (100)]) /*Register in_rand_seeds[28]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(29) % (100)]) /*Register in_rand_seeds[29]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(30) % (100)]) /*Register in_rand_seeds[30]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(31) % (100)]) /*Register in_rand_seeds[31]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(32) % (100)]) /*Register in_rand_seeds[32]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(33) % (100)]) /*Register in_rand_seeds[33]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(34) % (100)]) /*Register in_rand_seeds[34]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(35) % (100)]) /*Register in_rand_seeds[35]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(36) % (100)]) /*Register in_rand_seeds[36]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(37) % (100)]) /*Register in_rand_seeds[37]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(38) % (100)]) /*Register in_rand_seeds[38]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(39) % (100)]) /*Register in_rand_seeds[39]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(40) % (100)]) /*Register in_rand_seeds[40]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(41) % (100)]) /*Register in_rand_seeds[41]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(42) % (100)]) /*Register in_rand_seeds[42]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(43) % (100)]) /*Register in_rand_seeds[43]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(44) % (100)]) /*Register in_rand_seeds[44]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(45) % (100)]) /*Register in_rand_seeds[45]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(46) % (100)]) /*Register in_rand_seeds[46]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(47) % (100)]) /*Register in_rand_seeds[47]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(48) % (100)]) /*Register in_rand_seeds[48]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(49) % (100)]) /*Register in_rand_seeds[49]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(50) % (100)]) /*Register in_rand_seeds[50]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(51) % (100)]) /*Register in_rand_seeds[51]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(52) % (100)]) /*Register in_rand_seeds[52]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(53) % (100)]) /*Register in_rand_seeds[53]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(54) % (100)]) /*Register in_rand_seeds[54]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(55) % (100)]) /*Register in_rand_seeds[55]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(56) % (100)]) /*Register in_rand_seeds[56]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(57) % (100)]) /*Register in_rand_seeds[57]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(58) % (100)]) /*Register in_rand_seeds[58]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(59) % (100)]) /*Register in_rand_seeds[59]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(60) % (100)]) /*Register in_rand_seeds[60]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(61) % (100)]) /*Register in_rand_seeds[61]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(62) % (100)]) /*Register in_rand_seeds[62]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(63) % (100)]) /*Register in_rand_seeds[63]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(64) % (100)]) /*Register in_rand_seeds[64]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(65) % (100)]) /*Register in_rand_seeds[65]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(66) % (100)]) /*Register in_rand_seeds[66]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(67) % (100)]) /*Register in_rand_seeds[67]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(68) % (100)]) /*Register in_rand_seeds[68]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(69) % (100)]) /*Register in_rand_seeds[69]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(70) % (100)]) /*Register in_rand_seeds[70]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(71) % (100)]) /*Register in_rand_seeds[71]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(72) % (100)]) /*Register in_rand_seeds[72]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(73) % (100)]) /*Register in_rand_seeds[73]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(74) % (100)]) /*Register in_rand_seeds[74]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(75) % (100)]) /*Register in_rand_seeds[75]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(76) % (100)]) /*Register in_rand_seeds[76]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(77) % (100)]) /*Register in_rand_seeds[77]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(78) % (100)]) /*Register in_rand_seeds[78]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(79) % (100)]) /*Register in_rand_seeds[79]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(80) % (100)]) /*Register in_rand_seeds[80]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(81) % (100)]) /*Register in_rand_seeds[81]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(82) % (100)]) /*Register in_rand_seeds[82]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(83) % (100)]) /*Register in_rand_seeds[83]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(84) % (100)]) /*Register in_rand_seeds[84]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(85) % (100)]) /*Register in_rand_seeds[85]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(86) % (100)]) /*Register in_rand_seeds[86]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(87) % (100)]) /*Register in_rand_seeds[87]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(88) % (100)]) /*Register in_rand_seeds[88]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(89) % (100)]) /*Register in_rand_seeds[89]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(90) % (100)]) /*Register in_rand_seeds[90]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(91) % (100)]) /*Register in_rand_seeds[91]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(92) % (100)]) /*Register in_rand_seeds[92]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(93) % (100)]) /*Register in_rand_seeds[93]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(94) % (100)]) /*Register in_rand_seeds[94]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(95) % (100)]) /*Register in_rand_seeds[95]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(96) % (100)]) /*Register in_rand_seeds[96]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(97) % (100)]) /*Register in_rand_seeds[97]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(98) % (100)]) /*Register in_rand_seeds[98]*/);
		VNR_vardecl_7.push_back(ConvertToType<uint32_t, uint64_t>(randSeed[(99) % (100)]) /*Register in_rand_seeds[99]*/);
		std::vector<int32_t> VNR_vardecl_8;
		VNR_vardecl_8.push_back((1) * ((sizeof(float)) * (1) * (chunk)));
		VNR_vardecl_8.push_back((1) * ((sizeof(float)) * (chunk)));
		VNR_vardecl_8.push_back((1) * ((sizeof(float))));
		std::vector<uint32_t> VNR_vardecl_9;
		VNR_vardecl_9.push_back(((imageCount) - (0)) / (1));
		VNR_vardecl_9.push_back(((outHeight) - (0)) / (1));
		VNR_vardecl_9.push_back(((outWidth) - (0)) / (chunk));
		std::vector<int32_t> VNR_vardecl_10;
		VNR_vardecl_10.push_back((1) * ((sizeof(float) * (outHeight) * (outWidth))));
		VNR_vardecl_10.push_back((1) * ((sizeof(float) * (outWidth))));
		VNR_vardecl_10.push_back((chunk) * ((sizeof(float))));
		std::vector<int32_t> VNR_vardecl_11;
		VNR_vardecl_11.push_back((1) * ((sizeof(float) * (outHeight) * (outWidth))));
		VNR_vardecl_11.push_back((1) * ((sizeof(float) * (outWidth))));
		VNR_vardecl_11.push_back((1) * ((sizeof(float))));
		std::vector<uint32_t> VNR_vardecl_12;
		VNR_vardecl_12.push_back(((1) - (0)) / (1));
		VNR_vardecl_12.push_back(((1) - (0)) / (1));
		VNR_vardecl_12.push_back(((chunk) - (0)) / (1));
		VNR_vardecl_12.push_back((2));
		std::vector<int32_t> VNR_vardecl_13;
		VNR_vardecl_13.push_back((1) * ((sizeof(int[2])) * (1) * (chunk)));
		VNR_vardecl_13.push_back((1) * ((sizeof(int[2])) * (chunk)));
		VNR_vardecl_13.push_back((1) * ((sizeof(int[2]))));
		VNR_vardecl_13.push_back((sizeof(int)));
		std::vector<int32_t> VNR_vardecl_14;
		VNR_vardecl_14.push_back((1) * ((sizeof(int[2]) * (outHeight) * (outWidth))));
		VNR_vardecl_14.push_back((1) * ((sizeof(int[2]) * (outWidth))));
		VNR_vardecl_14.push_back((chunk) * ((sizeof(int[2]))));
		std::vector<int32_t> VNR_vardecl_15;
		VNR_vardecl_15.push_back((1) * ((sizeof(int[2]) * (outHeight) * (outWidth))));
		VNR_vardecl_15.push_back((1) * ((sizeof(int[2]) * (outWidth))));
		VNR_vardecl_15.push_back((1) * ((sizeof(int[2]))));
		VNR_vardecl_15.push_back((sizeof(int)));
		//See VNR_vardecl_6 for index variable decl
		//See VNR_vardecl_7 for register set decl
		mw.AddCompute(VNR_vardecl_2, VNR_vardecl_6, VNR_vardecl_7);
		
		//transfer from t to resultImage
		//Search VNR_vardecl_3 for source size.
		//Search VNR_vardecl_8 for source stride.
		//Search VNR_vardecl_9 for destination block size.
		//Search VNR_vardecl_10 for destination block stride.
		//Search VNR_vardecl_3 for destination element size.
		//Search VNR_vardecl_11 for destination element stride.
		mw.AddTransfer(VNR_vardecl_2, (0) + ((((0) * ((sizeof(float)) * (1) * (chunk))) + ((0) * ((sizeof(float)) * (chunk))) + ((0) * ((sizeof(float)))))), VNR_vardecl_3, VNR_vardecl_8, VNR_vardecl_1, VNR_vardecl_9, VNR_vardecl_10, VNR_vardecl_3, VNR_vardecl_11, sizeof(float));
		
		//transfer from t to atlas
		//Search VNR_vardecl_12 for source size.
		//Search VNR_vardecl_13 for source stride.
		//Search VNR_vardecl_9 for destination block size.
		//Search VNR_vardecl_14 for destination block stride.
		//Search VNR_vardecl_12 for destination element size.
		//Search VNR_vardecl_15 for destination element stride.
		mw.AddTransfer(VNR_vardecl_2, ((sizeof(float) * (1) * (1) * (chunk))) + ((((0) * ((sizeof(int[2])) * (1) * (chunk))) + ((0) * ((sizeof(int[2])) * (chunk))) + ((0) * ((sizeof(int[2])))))), VNR_vardecl_12, VNR_vardecl_13, VNR_vardecl_0, VNR_vardecl_9, VNR_vardecl_14, VNR_vardecl_12, VNR_vardecl_15, sizeof(int));
		
		mw.SetTaskGrain(0);
		mw.Finalize((((imageCount) - (0)) / (1)) * (((outHeight) - (0)) / (1)) * (((outWidth) - (0)) / (chunk)));
	}
	*buffer = new uint8_t[mw.GetBufferSize()];
	*bufferSize = mw.GetBufferSize();
	memcpy(*buffer, mw.GetBuffer(), mw.GetBufferSize());
	Touch(thread, *buffer, *bufferSize);
}
inline void TexSynth1LCacc_td_buf(uint8_t* buf, uint32_t bufSize, int thread)
{
	InterruptArgs isrArgs;
	isrArgs.threadID = thread;
	isrArgs.lcaccID = 0;
	isrArgs.lcaccMode = 0;
	LWI_RegisterInterruptHandler(&isrArgs);
	LCAcc_Command(thread, isrArgs.lcaccID, LCACC_CMD_BEGIN_PROGRAM, buf, bufSize, 0, 0);
	Wait_td__TexSynth1LCacc(thread);// wait for everything to finish
}
void TexSynth1LCacc_td(int thread, int (*atlas)[2], float (*resultImage), uint32_t randSeed[100], int inHeight, int inWidth, int outHeight, int outWidth, int imageCount, intptr_t imageArrayStart, int chunk)
{
	uint32_t bufSize;
	uint8_t* buffer;
	uint8_t* constCluster = NULL;
	CreateBuffer_TexSynth1LCacc_td(&buffer, &bufSize, &constCluster, thread, atlas, resultImage, randSeed, inHeight, inWidth, outHeight, outWidth, imageCount, imageArrayStart, chunk);
	TexSynth1LCacc_td_buf(buffer, bufSize, thread);
	if(constCluster)
	{
		delete [] constCluster;
	}
	delete [] buffer;
}
inline uint32_t TexSynth1LCacc_CalculateBiNSize(uint32_t randSeed[100], int inHeight, int inWidth, int outHeight, int outWidth, int imageCount, intptr_t imageArrayStart, int chunk)
{
	return ((sizeof(float) * (1) * (1) * (chunk)) + (sizeof(int[2]) * (1) * (1) * (chunk))) * (3);
}
class BiN_TexSynth1LCacc_Arbitrator_td
{
	std::vector<uint8_t*> bufSet;
	std::vector<uint32_t> bufSizeSet;
	std::vector<uint32_t> binSizeSet;
	std::vector<uint32_t> performancePoint;
	std::vector<uint32_t> cachePressureMod;
	InterruptArgs isr;
public:
	inline BiN_TexSynth1LCacc_Arbitrator_td(){}
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
		LCAcc_DeclareLCAccUse(threadID, 910, 1); //requests for TexSynth1
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
				TexSynth1LCacc_td_buf(bufSet[i], bufSizeSet[i], threadID);
				return;
			}
		}
		simics_assert(0);
	}
};

#endif
