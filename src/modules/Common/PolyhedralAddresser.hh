#ifndef POLYHEDRALADDRESSER_H
#define POLYHEDRALADDRESSER_H

#include <vector>
#include <cassert>
#include <iostream>

class PolyhedralAddresser
{
public:
	uint64_t base;
	unsigned int totalSize;
	std::vector<uint32_t> size;
	std::vector<uint32_t> subsize;
	std::vector<int32_t> stride;
private:
	void Init(uint64_t base, size_t dimSize, const uint32_t* size, const int32_t* stride)
	{
		assert(size);
		assert(stride);
		this->base = base;
		for(size_t i = 0; i < dimSize; i++)
		{
			this->size.push_back(size[i]);
			this->stride.push_back(stride[i]);
		}
		totalSize = 1;
		for(size_t i = 0; i < dimSize; i++)
		{
			totalSize *= size[i];
		}
		for(size_t i = 0; i < dimSize; i++)
		{
			subsize.push_back(0);
		}
		for(size_t i = 0; i < dimSize; i++)
		{
			uint32_t modSize = 1;
			for(size_t j = i + 1; j < dimSize; j++)
			{
				modSize *= size[j];
			}
			subsize[i] = modSize;
		}
	}
public:
	PolyhedralAddresser(uint64_t base, size_t dimSize, const uint32_t* size, const int32_t* stride)
	{
		Init(base, dimSize, size, stride);
	}
	PolyhedralAddresser(uint64_t base, const std::vector<uint32_t>& size, const std::vector<int32_t>& stride)
	{
		assert(size.size() == stride.size());
		Init(base, size.size(), &(size[0]), &(stride[0]));
	}
	uint32_t TotalSize() const
	{
		return totalSize;
	}
	uint64_t GetAddr(uint32_t index) const
	{
		index = index % totalSize;
		uint64_t ret = base;
		index = index % totalSize;
		for(size_t i = 0; i < size.size(); i++)
		{
			unsigned int localSize = index / subsize[i];
			unsigned int localStrideIndex = localSize % size[i];
			ret += (int64_t)stride[i] * localStrideIndex;
		}
		return ret;
	}	
};

#endif
