#ifndef LCACCOPERATINGMODE_H
#define LCACCOPERATINGMODE_H

#include "SPMInterface.hh"
#include <vector>
#include <string>
#include <queue>
#include <stdint.h>
//included to provide functionality to modules
#include <cmath>
#include <cstdlib>
#include <cstdio>

namespace LCAcc
{
	class LCAccOperatingMode
	{
	protected:
		static const int ReadAccess;
		static const int WriteAccess;
		static const int ReadLockAccess;
		static const int WriteLockAccess;
		static const int WriteUnlockAccess;
		static const int UnlockAccess;
	private:
		std::queue<uint64_t> spmAddrQueue;
		std::queue<uint64_t> memAddrQueue;
		std::queue<uint32_t> accessSizeQueue;
		std::queue<int> accessTypeQueue;
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
		std::vector<uint32_t> argumentWidth;
		SPMInterface* spm;
	protected:
		void AddRead(uint64_t srcAddr, uint64_t dstAddr, uint32_t size);
		void AddWrite(uint64_t srcAddr, uint64_t dstAddr, uint32_t size);
		template <class From, class To>
		inline To ConvertTypes(From f)
		{
			TypeConverter<From, To> x;
			x.from = f;
			return x.to;
		}
		long ReadSPMInt(uint32_t arg, uint64_t addr, uint32_t index, bool sign);
		void WriteSPMInt(uint32_t arg, uint64_t addr, uint32_t index, bool sign, long val);
		double ReadSPMFlt(uint32_t arg, uint64_t addr, uint32_t index);
		void WriteSPMFlt(uint32_t arg, uint64_t addr, uint32_t index, double val);
		uint32_t GetArgumentWidth(uint32_t index);
	public:
		virtual ~LCAccOperatingMode(){}
		virtual void BeginComputation() = 0;
		void SetSPM(SPMInterface* spm);
		virtual void EndComputation() = 0;
		virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs) = 0;
		virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs) = 0;
		virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive) = 0;
		virtual void MemoryAccessComplete(int iteration, int maxIteration, int taskID, uint64_t spmAddr, uint64_t memAddr, int accessType, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec) {};
		virtual int CycleTime() = 0;
		virtual int InitiationInterval() = 0;
		virtual int PipelineDepth() = 0;
		virtual bool CallAllAtEnd() = 0;
		void SetArgumentWidth(const std::vector<uint32_t>& widths);
		virtual void SetRegisterValues(const std::vector<uint64_t>& regs) = 0;
		
		size_t MemoryAccessCount() const;
		void GetNextMemoryAccess(uint64_t& spmAddr, uint64_t& memAddr, uint32_t& size, int& type);
	};
}

#endif
