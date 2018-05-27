#ifndef LCACC_MODE_POOL_H
#define LCACC_MODE_POOL_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_Pool : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_Pool(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_in1 = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_in2 = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_in3 = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_in4 = argAddrVec[3];

			if(argActive[0])
			{
				outputArgs.push_back(addr_in1);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_in2);
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_in3);
			}
			if(argActive[3])
			{
				outputArgs.push_back(addr_in4);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(4 < argAddrVec.size());
			uint64_t addr_Pout = argAddrVec[4];

			if(argActive[4])
			{
				outputArgs.push_back(addr_Pout);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 5);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in1 = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in2 = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in3 = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in4 = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_Pout = LCACC_INTERNAL_argAddrVec[4];

			double in1;
			double in2;
			double in3;
			double in4;
			double Pout;
			in1 = 0;
			in2 = 0;
			in3 = 0;
			in4 = 0;
			Pout = 0;

			in1 = ReadSPMFlt(0, addr_in1, 0);
			in2 = ReadSPMFlt(1, addr_in2, 0);
			in3 = ReadSPMFlt(2, addr_in3, 0);
			in4 = ReadSPMFlt(3, addr_in4, 0);

			#define SPMAddressOf(x) (addr_##x)
			if (in1 > Pout)
			  Pout = in1;
			if (in2 > Pout)
			  Pout = in2;
			if (in3 > Pout)
			  Pout = in3;
			if (in4 > Pout)
			  Pout = in4;
			#undef SPMAddressOf

			WriteSPMFlt(4, addr_Pout, 0, Pout);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 5;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 4;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "Pool";}
		inline virtual int ArgumentCount(){return 5;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 250;}
	};
}

#endif
