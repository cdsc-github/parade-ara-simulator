#include "LCAccOperatingMode.hh"
#include "DMAController.hh"

namespace LCAcc
{
	const int LCAccOperatingMode::ReadAccess = DMAController::AccessType::Read;
	const int LCAccOperatingMode::WriteAccess = DMAController::AccessType::Write;
	const int LCAccOperatingMode::ReadLockAccess = DMAController::AccessType::ReadLock;
	const int LCAccOperatingMode::WriteLockAccess = DMAController::AccessType::WriteLock;
	const int LCAccOperatingMode::WriteUnlockAccess = DMAController::AccessType::WriteUnlock;
	const int LCAccOperatingMode::UnlockAccess = DMAController::AccessType::Unlock;
	void LCAccOperatingMode::AddRead(uint64_t srcAddr, uint64_t dstAddr, uint32_t size)
	{
		accessTypeQueue.push(ReadAccess);
		spmAddrQueue.push(dstAddr);
		memAddrQueue.push(srcAddr);
		accessSizeQueue.push(size);
	}
	void LCAccOperatingMode::AddWrite(uint64_t srcAddr, uint64_t dstAddr, uint32_t size)
	{
		accessTypeQueue.push(WriteAccess);
		spmAddrQueue.push(srcAddr);
		memAddrQueue.push(dstAddr);
		accessSizeQueue.push(size);
	}
	long LCAccOperatingMode::ReadSPMInt(uint32_t arg, uint64_t addr, uint32_t index, bool sign)
	{
		assert(argumentWidth.size() > arg);
		uint32_t width = argumentWidth[arg];
		assert(width == 1 || width == 2 || width == 4 || width == 8);
		assert(spm);
		addr += width * index;
		if(sign)
		{
			if(width == 1)
			{
				int8_t x;
				spm->Read(addr, width, &x);
				return (int64_t)x;
			}
			else if(width == 2)
			{
				int16_t x;
				spm->Read(addr, width, &x);
				return (int64_t)x;
			}
			else if(width == 4)
			{
				int32_t x;
				spm->Read(addr, width, &x);
				return (int64_t)x;
			}
			else if(width == 8)
			{
				int64_t x;
				spm->Read(addr, width, &x);
				return (int64_t)x;
			}
		}
		else
		{
			if(width == 1)
			{
				uint8_t x;
				spm->Read(addr, width, &x);
				return (int64_t)x;
			}
			else if(width == 2)
                        {
                                uint16_t x;
                                spm->Read(addr, width, &x);
                                return (int64_t)x;
                        }
                        else if(width == 4)
                        {
                                uint32_t x;
                                spm->Read(addr, width, &x);
                                return (int64_t)x;
                        }
                        else if(width == 8)
                        {
                                uint64_t x;
                                spm->Read(addr, width, &x);
                                return (int64_t)x;
                        }
		}
		return 0;
	}
	void LCAccOperatingMode::WriteSPMInt(uint32_t arg, uint64_t addr, uint32_t index, bool sign, long val)
	{
		assert(argumentWidth.size() > arg);
		uint32_t width = argumentWidth[arg];
		assert(width == 1 || width == 2 || width == 4 || width == 8);
		assert(spm);
		addr += width * index;
		if(sign)
		{
			if(width == 1)
			{
				int8_t x = (int8_t)val;
				spm->Write(addr, width, &x);
			}
			else if(width == 2)
			{
				int16_t x = (int16_t)val;
				spm->Write(addr, width, &x);
			}
			else if(width == 4)
			{
				int32_t x = (int32_t)val;
				spm->Write(addr, width, &x);
			}
			else if(width == 8)
			{
				int64_t x = (int64_t)val;
				spm->Write(addr, width, &x);
			}
		}
		else
		{
			if(width == 1)
			{
				uint8_t x = (uint8_t)val;
				spm->Write(addr, width, &x);
			}
			else if(width == 2)
			{
				uint16_t x = (uint16_t)val;
				spm->Write(addr, width, &x);
			}
			else if(width == 4)
			{
				uint32_t x = (uint32_t)val;
				spm->Write(addr, width, &x);
			}
			else if(width == 8)
			{
				uint64_t x = (uint64_t)val;
				spm->Write(addr, width, &x);
			}
		}
	}
	double LCAccOperatingMode::ReadSPMFlt(uint32_t arg, uint64_t addr, uint32_t index)
	{
		assert(argumentWidth.size() > arg);
		uint32_t width = argumentWidth[arg];
		assert(width == 4 || width == 8);
		assert(spm);
		addr += width * index;
		if(width == 4)
		{
			float x;
			spm->Read(addr, width, &x);
			return (double)x;
		}
		else if(width == 8)
		{
			double x;
			spm->Read(addr, width, &x);
			return x;
		}
		return 0.0;
	}
	void LCAccOperatingMode::WriteSPMFlt(uint32_t arg, uint64_t addr, uint32_t index, double val)
	{
		assert(argumentWidth.size() > arg);
		uint32_t width = argumentWidth[arg];
		assert(width == 4 || width == 8);
		assert(spm);
		addr += width * index;
		if(width == 4)
		{
			float x = (float)val;
			spm->Write(addr, width, &x);
		}
		else if(width == 8)
		{
			spm->Write(addr, width, &val);
		}
	}
	uint32_t LCAccOperatingMode::GetArgumentWidth(uint32_t index)
	{
		assert(argumentWidth.size() > index);
		return argumentWidth[index];
	}
	void LCAccOperatingMode::SetSPM(SPMInterface* spm)
	{
		assert(spm);
		this->spm = spm;
	}
	void LCAccOperatingMode::SetArgumentWidth(const std::vector<uint32_t>& widths)
	{
		argumentWidth = widths;
	}
	size_t LCAccOperatingMode::MemoryAccessCount() const
	{
		assert(spmAddrQueue.size() == memAddrQueue.size());
		assert(spmAddrQueue.size() == accessTypeQueue.size());
		assert(spmAddrQueue.size() == accessSizeQueue.size());
		return spmAddrQueue.size();
	}
	void LCAccOperatingMode::GetNextMemoryAccess(uint64_t& spmAddr, uint64_t& memAddr, uint32_t& size, int& type)
	{
		assert(MemoryAccessCount() > 0);
		spmAddr = spmAddrQueue.front();
		memAddr = memAddrQueue.front();
		size = accessSizeQueue.front();
		type = accessTypeQueue.front();
		spmAddrQueue.pop();
		memAddrQueue.pop();
		accessSizeQueue.pop();
		accessTypeQueue.pop();
	}
}
