#ifndef COMPUTE_DESCRIPTION_H
#define COMPUTE_DESCRIPTION_H

#include <vector>
#include <stdint.h>
#include "Packetizer.hh"

#define INVALID_ARG_BASE_ADDR 0x0FFFFFFF


class ComputeDescription
{
public:
	uint32_t opcode;
	uint32_t spmWindowSize;
	uint32_t spmWindowCount;
	std::vector<bool> argActive;
	std::vector<uint32_t> argBaseAddr;
	std::vector<std::vector<int32_t> > argStride;
	std::vector<std::vector<uint32_t> > argSize;
	std::vector<uint32_t> argElementSize;
	std::vector<uint64_t> controlRegister;
	void WriteOut(PacketBuilder& pb) const
	{
		pb.Write((uint32_t)opcode);
		pb.Write((uint32_t)spmWindowCount);
		pb.Write((uint32_t)spmWindowSize);
		pb.Write((uint8_t)(argBaseAddr.size()));
		for(size_t i = 0; i < argBaseAddr.size(); i++)
		{
			if(argActive[i])
			{
				pb.Write((uint32_t)(argBaseAddr[i]));
			}
			else
			{
				pb.Write((uint32_t)(INVALID_ARG_BASE_ADDR));
			}
			pb.Write((uint8_t)(argSize[i].size()));
			for(size_t x = 0; x < argSize[i].size(); x++)
			{
				pb.Write((uint32_t)(argSize[i][x]));
				pb.Write((int32_t)(argStride[i][x]));
			}
			pb.Write((uint32_t)argElementSize[i]);
		}
		pb.Write((uint8_t)(controlRegister.size()));
		for(size_t i = 0; i < controlRegister.size(); i++)
		{
			pb.Write((uint64_t)(controlRegister[i]));
		}
	}
	void ReadIn(PacketReader& pr)
	{
		opcode = pr.Read<uint32_t>();
		spmWindowCount = pr.Read<uint32_t>();
		spmWindowSize = pr.Read<uint32_t>();
		int argCount = pr.Read<uint8_t>();
		//std::cout << "opcode : " << opcode << " spm window : " << spmWindowSize << "[" << spmWindowCount << "]  arg count : " << argCount << std::endl; 
		assert(spmWindowCount > 1);
		for(int i = 0; i < argCount; i++)
		{
			uint32_t baseAddr = pr.Read<uint32_t>();
			if(baseAddr == INVALID_ARG_BASE_ADDR)
			{
				argBaseAddr.push_back(0);
				argActive.push_back(false);
			}
			else
			{
				argBaseAddr.push_back(baseAddr);
				argActive.push_back(true);
			}
			int count = pr.Read<uint8_t>();
			//std::cout << "Arg " << i << " base: " << argBaseAddr[argBaseAddr.size() - 1] << " dimension count: " << count << "  ";
			std::vector<uint32_t> size;
			std::vector<int32_t> stride;
			for(int x = 0; x < count; x++)
			{
				uint32_t sizeVal = pr.Read<uint32_t>();
				int32_t strideVal = pr.Read<int32_t>();
				size.push_back(sizeVal);
				stride.push_back(strideVal);
				//std::cout << "[" << sizeVal << ", " << strideVal << "]";
			}
			//std::cout << std::endl;
			argElementSize.push_back(pr.Read<uint32_t>());
			assert(argElementSize[i] > 0);
			argSize.push_back(size);
			argStride.push_back(stride);
		}
		int regCount = pr.Read<uint8_t>();
		//std::cout << "Register count : " << regCount << " ";
		for(int i = 0; i < regCount; i++)
		{
			uint64_t val = pr.Read<uint64_t>();
			//std::cout << "[" << val << "]";
			controlRegister.push_back(val);
		}
		//std::cout << std::endl;
	}
	ComputeDescription()
	{
		opcode = 0;
	}
	ComputeDescription(PacketReader& pr)
	{
		opcode = 0;
		ReadIn(pr);
	}
};

#endif
