#ifndef LCACC_MODE_DENOISE1_DIMM_H
#define LCACC_MODE_DENOISE1_DIMM_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_denoise1_dimm : public LCAccOperatingMode
	{
		int width;
		int height;
		int depth;
	public:
		inline OperatingMode_denoise1_dimm(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_u = argAddrVec[0];

			if(argActive[0])
			{
				outputArgs.push_back(addr_u);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(1 < argAddrVec.size());
			uint64_t addr_g = argAddrVec[1];

			if(argActive[1])
			{
				outputArgs.push_back(addr_g);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 2);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_u = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_g = LCACC_INTERNAL_argAddrVec[1];

			double u;
			double g;
			u = 0;
			g = 0;

			u = ReadSPMFlt(0, addr_u, 0);

			#define SPMAddressOf(x) (addr_##x)
			#undef SPMAddressOf

			WriteSPMFlt(1, addr_g, 0, g);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "denoise1_dimm";}
		inline virtual int ArgumentCount(){return 2;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 3);
			width = ConvertTypes<uint32_t, int>(regs[0]);
			height = ConvertTypes<uint32_t, int>(regs[1]);
			depth = ConvertTypes<uint32_t, int>(regs[2]);
		}
		inline static int GetOpCode(){return 151;}
	};
}

#endif
