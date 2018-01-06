#ifndef LCACC_MODE_SEGMENTATION_H
#define LCACC_MODE_SEGMENTATION_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_segmentation : public LCAccOperatingMode
	{
		int width;
		int height;
		int depth;
	public:
		inline OperatingMode_segmentation(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_phi_Center = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_u0 = argAddrVec[1];

			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_Center);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_u0);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(2 < argAddrVec.size());
			uint64_t addr_result = argAddrVec[2];

			if(argActive[2])
			{
				outputArgs.push_back(addr_result);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 3);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_phi_Center = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_u0 = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_result = LCACC_INTERNAL_argAddrVec[2];

			double phi_Center;
			double u0;
			double result;
			phi_Center = 0;
			u0 = 0;
			result = 0;

			phi_Center = ReadSPMFlt(0, addr_phi_Center, 0);
			u0 = ReadSPMFlt(1, addr_u0, 0);

			#define SPMAddressOf(x) (addr_##x)
			#undef SPMAddressOf

			WriteSPMFlt(2, addr_result, 0, result);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "segmentation";}
		inline virtual int ArgumentCount(){return 3;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 3);
			width = ConvertTypes<uint32_t, int>(regs[0]);
			height = ConvertTypes<uint32_t, int>(regs[1]);
			depth = ConvertTypes<uint32_t, int>(regs[2]);
		}
		inline static int GetOpCode(){return 302;}
	};
}

#endif
