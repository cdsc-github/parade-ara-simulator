#ifndef PACKETIZER_H
#define PACKETIZER_H

#include <stdint.h>
#include <iostream>
#include <cassert>
#include <vector>

class PacketBuilder
{
	template<class T>
	struct Converter
	{
		union
		{
			T from;
			uint8_t to[sizeof(T)];
		};
	};
	std::vector<uint8_t> buf;
	bool swapEndian;
public:
	PacketBuilder()
	{
		swapEndian = false;
	}
	template <class T> void Write(const T& val)
	{
		Converter<T> c;
		c.from = val;
		for(size_t i = 0; i < sizeof(T); i++)
		{
			if(swapEndian)
			{
				buf.push_back(c.to[(sizeof(T) - 1) - i]);
			}
			else
			{
				buf.push_back(c.to[i]);
			}
		}
	}
	void SetEndianSwap(bool swap)
	{
		swapEndian = swap;
	}
	size_t GetBufferSize() const
	{
		return buf.size();
	}
	const void* GetBuffer() const
	{
		return &(buf[0]);
	}
	const std::vector<uint8_t>& GetBufferVector() const
	{
		return buf;
	}
};
class PacketReader
{
	template<class T> struct Converter
	{
		union
		{
			uint8_t from[sizeof(T)];
			T to;
		};		
	};
	std::vector<uint8_t> buf;
	size_t readIndex;
	int compactSize;
	bool swapEndian;
public:
	PacketReader(const void* buffer, size_t size)
	{
		const uint8_t* b = (const uint8_t*)buffer;
		for(size_t i = 0; i < size; i++)
		{
			buf.push_back(b[i]);
		}
		readIndex = 0;
		swapEndian = false;
		compactSize = 0;
	}
	template<class T> PacketReader(std::vector<T> buffer)
		: PacketReader((void*)&(buffer[0]), sizeof(T) * buffer.size())
	{
		
	}
	template<class T> T Read()
	{
		Converter<T> c;
		int rSize = (compactSize) ? compactSize : (int)sizeof(T);
		assert(rSize >= (int)sizeof(T));
		for(int i = 0; i < rSize; i++)
		{
			if(!(readIndex < buf.size()))
			{
				std::cout << "Read past internal buffer, index : " << readIndex << std::endl;
				assert(readIndex < buf.size());
			}
			if(swapEndian)
			{
				c.from[(rSize - 1) - i] = buf[readIndex];
			}
			else
			{
				c.from[i] = buf[readIndex];
			}
			readIndex++;
		}
		return c.to;
	}
	size_t TotalSize() const
	{
		return buf.size();
	}
	void SetEndianSwap(bool swap)
	{
		swapEndian = swap;
	}
	void SetCompaction(int s)
	{
		assert(s > 0);
		compactSize = s;
	}
	size_t SizeRemaining() const
	{
		return buf.size() - (readIndex);
	}
};

#endif
