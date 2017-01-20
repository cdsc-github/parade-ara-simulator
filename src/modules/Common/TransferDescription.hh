#ifndef TRANSFER_DESCRIPTION_H
#define TRANSFER_DESCRIPTION_H

#include <vector>
#include <stdint.h>
#include "Packetizer.hh"

class TransferDescription
{
public:
	static const int MemoryDevice = -1;
	typedef uint64_t PtrType;
	int srcDevice;
	int dstDevice;
	uint64_t srcBaseAddress;
	uint64_t dstBaseAddress;
	std::vector<int32_t> srcStride;
	std::vector<uint32_t> srcSize;
	std::vector<int32_t> dstStride;
	std::vector<uint32_t> dstSize;
	unsigned int srcSplit;
	unsigned int dstSplit;
	uint32_t elementSize;
	inline bool operator ==(const TransferDescription& td) const
	{
		if(srcDevice != td.srcDevice ||
			dstDevice != td.dstDevice ||
			srcBaseAddress != td.srcBaseAddress ||
			dstBaseAddress != td.dstBaseAddress ||
			srcSplit != td.srcSplit ||
			dstSplit != td.dstSplit ||
			srcStride.size() != td.srcStride.size() ||
			dstStride.size() != td.dstStride.size() ||
			srcSize.size() != td.srcSize.size() ||
			dstSize.size() != td.dstSize.size())
		{
			return false;
		}
		assert(srcStride.size() == srcSize.size());
		assert(td.srcStride.size() == td.srcSize.size());
		for(size_t i = 0; i < srcStride.size(); i++)
		{
			if(srcStride.at(i) != td.srcStride.at(i) || srcSize.at(i) != td.srcSize.at(i))
			{
				return false;
			}
		}
		assert(dstStride.size() == dstSize.size());
		assert(td.dstStride.size() == td.dstSize.size());
		for(size_t i = 0; i < dstStride.size(); i++)
		{
			if(dstStride.at(i) != td.dstStride.at(i) || dstSize.at(i) != td.dstSize.at(i))
			{
				return false;
			}
		}
		return true;
	}
	void WriteOut(PacketBuilder& pb) const
	{
		pb.Write((int32_t)srcDevice);
		pb.Write((PtrType)srcBaseAddress);
		pb.Write((uint8_t)srcSplit);
		pb.Write((uint8_t)(srcSize.size() - srcSplit));
		pb.Write((int32_t)dstDevice);
		pb.Write((PtrType)dstBaseAddress);
		pb.Write((uint8_t)dstSplit);
		pb.Write((uint8_t)(dstSize.size() - dstSplit));
		for(unsigned int x = 0; x < srcSize.size(); x++)
		{
			pb.Write((int32_t)srcSize[x]);
			pb.Write((uint32_t)srcStride[x]);
		}
		for(unsigned int x = 0; x < dstSize.size(); x++)
		{
			pb.Write((int32_t)dstSize[x]);
			pb.Write((uint32_t)dstStride[x]);
		}
		pb.Write((uint8_t)elementSize);
	}
	void ReadIn(PacketReader& pr)
	{
		srcDevice = pr.Read<int32_t>();
		srcBaseAddress = pr.Read<PtrType>();
		unsigned int srcDimensionalityLower = pr.Read<uint8_t>();
		unsigned int srcDimensionalityUpper = pr.Read<uint8_t>();
		dstDevice = pr.Read<int32_t>();
		dstBaseAddress = pr.Read<PtrType>();
		unsigned int dstDimensionalityLower = pr.Read<uint8_t>();
		unsigned int dstDimensionalityUpper = pr.Read<uint8_t>();
		srcSplit = srcDimensionalityLower;
		dstSplit = dstDimensionalityLower;
// std::cout << "Transfer details : " << std::endl;
// std::cout << "src device : " << srcDevice << " src addr : " << srcBaseAddress << " dim : " << srcDimensionalityLower << ":" << srcDimensionalityUpper << "   ";
		for(unsigned int x = 0; x < srcDimensionalityLower + srcDimensionalityUpper; x++)
		{
			uint32_t sizeVal = pr.Read<uint32_t>();
			uint32_t strideVal = pr.Read<int32_t>();
			srcSize.push_back(sizeVal);
			srcStride.push_back(strideVal);
// std::cout << "[" << sizeVal << ", " << strideVal << "]";
		}
// std::cout << std::endl;
// std::cout << "dst device : " << dstDevice << " dst addr : " << dstBaseAddress << " dim : " << dstDimensionalityLower << ":" << dstDimensionalityUpper << "   ";
		for(unsigned int x = 0; x < dstDimensionalityLower + dstDimensionalityUpper; x++)
		{
			uint32_t sizeVal = pr.Read<uint32_t>();
			uint32_t strideVal = pr.Read<int32_t>();
			dstSize.push_back(sizeVal);
			dstStride.push_back(strideVal);
// std::cout << "[" << sizeVal << ", " << strideVal << "]";
		}
// std::cout << std::endl;
		elementSize = pr.Read<uint8_t>();
		assert(elementSize > 0);
// std::cout << "Element size: " << elementSize << std::endl;
	}
	TransferDescription()
	{
		srcDevice = MemoryDevice;
		dstDevice = MemoryDevice;
		srcBaseAddress = dstBaseAddress = 0;
		elementSize = 0;
	}
	TransferDescription(PacketReader& pr)
	{
		srcDevice = MemoryDevice;
		dstDevice = MemoryDevice;
		srcBaseAddress = dstBaseAddress = 0;
		elementSize = 0;
		ReadIn(pr);
	}
};

#endif
