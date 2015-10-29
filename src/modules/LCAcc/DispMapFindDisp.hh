#ifndef LCACC_MODE_DISPMAPFINDDISP_H
#define LCACC_MODE_DISPMAPFINDDISP_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_DispMapFindDisp : public LCAccOperatingMode
	{
		float internal_min;
		float internal_index;
	public:
		inline OperatingMode_DispMapFindDisp(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_in_IImg1 = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_in_IImg2 = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_in_IImg3 = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_in_IImg4 = argAddrVec[3];

			if(argActive[0])
			{
				outputArgs.push_back(addr_in_IImg1);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_in_IImg2);
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_in_IImg3);
			}
			if(argActive[3])
			{
				outputArgs.push_back(addr_in_IImg4);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(4 < argAddrVec.size());
			uint64_t addr_out_Disp = argAddrVec[4];

			if(argActive[4])
			{
				outputArgs.push_back(addr_out_Disp);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 5);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_IImg1 = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_IImg2 = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_IImg3 = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_IImg4 = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_out_Disp = LCACC_INTERNAL_argAddrVec[4];

			double in_IImg1;
			double in_IImg2;
			double in_IImg3;
			double in_IImg4;
			double out_Disp;
			in_IImg1 = 0;
			in_IImg2 = 0;
			in_IImg3 = 0;
			in_IImg4 = 0;
			out_Disp = 0;

			in_IImg1 = ReadSPMFlt(0, addr_in_IImg1, 0);
			in_IImg2 = ReadSPMFlt(1, addr_in_IImg2, 0);
			in_IImg3 = ReadSPMFlt(2, addr_in_IImg3, 0);
			in_IImg4 = ReadSPMFlt(3, addr_in_IImg4, 0);

			#define SPMAddressOf(x) (addr_##x)
			// Compute SAD (Sum of Absolute Differences) value
			    float SAD = in_IImg1 + in_IImg2 - in_IImg3 - in_IImg4;
			
				// Check for minimum SAD value and return its index (i.e. disparity value)
				if (internal_index == 0.f || SAD < internal_min) {
					internal_min = SAD;
					out_Disp = internal_index;
				}
				else
					out_Disp = -1;
				internal_index++;
			#undef SPMAddressOf

			WriteSPMFlt(4, addr_out_Disp, 0, out_Disp);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 31;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "DispMapFindDisp";}
		inline virtual int ArgumentCount(){return 5;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 2);
			internal_min = ConvertTypes<uint64_t, float>(regs[0]);
			internal_index = ConvertTypes<uint64_t, float>(regs[1]);
		}
		inline static int GetOpCode(){return 832;}
	};
}

#endif
