#ifndef LCACC_MODE_RELU_H
#define LCACC_MODE_RELU_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_Relu : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_Relu(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_Pin = argAddrVec[0];

			if(argActive[0])
			{
				outputArgs.push_back(addr_Pin);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(1 < argAddrVec.size());
			uint64_t addr_Pout = argAddrVec[1];

			if(argActive[1])
			{
				outputArgs.push_back(addr_Pout);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 2);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_Pin = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_Pout = LCACC_INTERNAL_argAddrVec[1];

			double Pin;
			double Pout;
			Pin = 0;
			Pout = 0;

			Pin = ReadSPMFlt(0, addr_Pin, 0);

			#define SPMAddressOf(x) (addr_##x)
			if (Pin < 0.0)
			  Pout = 0.0;
			#undef SPMAddressOf

			WriteSPMFlt(1, addr_Pout, 0, Pout);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 5;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "Relu";}
		inline virtual int ArgumentCount(){return 2;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 246;}
	};
}

#endif
