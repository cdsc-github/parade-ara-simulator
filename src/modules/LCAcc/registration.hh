#ifndef LCACC_MODE_REGISTRATION_H
#define LCACC_MODE_REGISTRATION_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_registration : public LCAccOperatingMode
	{
		int width;
		int height;
		int depth;
	public:
		inline OperatingMode_registration(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_v1 = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_v2 = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_v3 = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_u1 = argAddrVec[3];
			assert(4 < argAddrVec.size());
			uint64_t addr_u2 = argAddrVec[4];
			assert(5 < argAddrVec.size());
			uint64_t addr_u3 = argAddrVec[5];

			if(argActive[0])
			{
				outputArgs.push_back(addr_v1);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_v2);
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_v3);
			}
			if(argActive[3])
			{
				outputArgs.push_back(addr_u1);
			}
			if(argActive[4])
			{
				outputArgs.push_back(addr_u2);
			}
			if(argActive[5])
			{
				outputArgs.push_back(addr_u3);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(6 < argAddrVec.size());
			uint64_t addr_ret_u1 = argAddrVec[6];
			assert(7 < argAddrVec.size());
			uint64_t addr_ret_u2 = argAddrVec[7];
			assert(8 < argAddrVec.size());
			uint64_t addr_ret_u3 = argAddrVec[8];

			if(argActive[6])
			{
				outputArgs.push_back(addr_ret_u1);
			}
			if(argActive[7])
			{
				outputArgs.push_back(addr_ret_u2);
			}
			if(argActive[8])
			{
				outputArgs.push_back(addr_ret_u3);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 9);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_v1 = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_v2 = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_v3 = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_u1 = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_u2 = LCACC_INTERNAL_argAddrVec[4];
			assert(5 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_u3 = LCACC_INTERNAL_argAddrVec[5];
			assert(6 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_ret_u1 = LCACC_INTERNAL_argAddrVec[6];
			assert(7 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_ret_u2 = LCACC_INTERNAL_argAddrVec[7];
			assert(8 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_ret_u3 = LCACC_INTERNAL_argAddrVec[8];

			double v1;
			double v2;
			double v3;
			double u1;
			double u2;
			double u3;
			double ret_u1;
			double ret_u2;
			double ret_u3;
			v1 = 0;
			v2 = 0;
			v3 = 0;
			u1 = 0;
			u2 = 0;
			u3 = 0;
			ret_u1 = 0;
			ret_u2 = 0;
			ret_u3 = 0;

			v1 = ReadSPMFlt(0, addr_v1, 0);
			v2 = ReadSPMFlt(1, addr_v2, 0);
			v3 = ReadSPMFlt(2, addr_v3, 0);
			u1 = ReadSPMFlt(3, addr_u1, 0);
			u2 = ReadSPMFlt(4, addr_u2, 0);
			u3 = ReadSPMFlt(5, addr_u3, 0);

			#define SPMAddressOf(x) (addr_##x)
			#undef SPMAddressOf

			WriteSPMFlt(6, addr_ret_u1, 0, ret_u1);
			WriteSPMFlt(7, addr_ret_u2, 0, ret_u2);
			WriteSPMFlt(8, addr_ret_u3, 0, ret_u3);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "registration";}
		inline virtual int ArgumentCount(){return 9;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 3);
			width = ConvertTypes<uint32_t, int>(regs[0]);
			height = ConvertTypes<uint32_t, int>(regs[1]);
			depth = ConvertTypes<uint32_t, int>(regs[2]);
		}
		inline static int GetOpCode(){return 402;}
	};
}

#endif
