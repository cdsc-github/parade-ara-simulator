#ifndef LCACC_MODE_BLUR_DIMM_H
#define LCACC_MODE_BLUR_DIMM_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_blur_dimm : public LCAccOperatingMode
	{
		int length;
	public:
		inline OperatingMode_blur_dimm(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_in = argAddrVec[0];

			if(argActive[0])
			{
				outputArgs.push_back(addr_in);
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
			uint64_t addr_in = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_out = LCACC_INTERNAL_argAddrVec[1];

			double in;
			double out;
			in = 0;
			out = 0;

			in = ReadSPMFlt(0, addr_in, 0);

			#define SPMAddressOf(x) (addr_##x)
			#undef SPMAddressOf

			WriteSPMFlt(1, addr_out, 0, out);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "blur_dimm";}
		inline virtual int ArgumentCount(){return 2;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 1);
			length = ConvertTypes<uint32_t, int>(regs[0]);
		}
		inline static int GetOpCode(){return 352;}
	};
}

#endif
