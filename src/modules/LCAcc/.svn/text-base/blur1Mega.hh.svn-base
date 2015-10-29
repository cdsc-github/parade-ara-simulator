#ifndef LCACC_MODE_BLUR1MEGA_H
#define LCACC_MODE_BLUR1MEGA_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_blur1Mega : public LCAccOperatingMode
	{
		float carry;
		float nu;
		int resetLength;
	public:
		inline OperatingMode_blur1Mega(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_val = argAddrVec[0];
			uint64_t addr_val2 = addr_val + 1 * GetArgumentWidth(0);

			if(argActive[0])
			{
				outputArgs.push_back(addr_val);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_val2);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(1 < argAddrVec.size());
			uint64_t addr_out = argAddrVec[1];

			if(argActive[1])
			{
				outputArgs.push_back(addr_out);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 2);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_val = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_out = LCACC_INTERNAL_argAddrVec[1];
			uint64_t addr_val2 = addr_val + 1 * GetArgumentWidth(0);

			double val;
			double val2;
			double out;
			val = 0;
			val2 = 0;
			out = 0;

			val = ReadSPMFlt(0, addr_val, 0);
			val2 = ReadSPMFlt(0, addr_val2, 0);

			#define SPMAddressOf(x) (addr_##x)
			if((iteration + maxIteration * taskID) % resetLength == 0)
			{
				carry = val * nu;
				out = val * nu + val2;
			}
			else
			{
				carry = out = carry * nu + val2;
			}
			#undef SPMAddressOf

			WriteSPMFlt(1, addr_out, 0, out);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 17;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "blur1Mega";}
		inline virtual int ArgumentCount(){return 2;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 3);
			carry = ConvertTypes<uint64_t, float>(regs[0]);
			nu = ConvertTypes<uint64_t, float>(regs[1]);
			resetLength = ConvertTypes<uint64_t, int>(regs[2]);
		}
		inline static int GetOpCode(){return 105;}
	};
}

#endif
