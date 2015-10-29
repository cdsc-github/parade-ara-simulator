#ifndef LCACC_MODE_VECTORADDSAMPLE_H
#define LCACC_MODE_VECTORADDSAMPLE_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_VectorAddSample : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_VectorAddSample(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_input_a = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_input_b = argAddrVec[1];

			if(argActive[0])
			{
				outputArgs.push_back(addr_input_a);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_input_b);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(2 < argAddrVec.size());
			uint64_t addr_output = argAddrVec[2];

			if(argActive[2])
			{
				outputArgs.push_back(addr_output);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 3);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_input_a = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_input_b = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output = LCACC_INTERNAL_argAddrVec[2];

			double input_a;
			double input_b;
			double output;
			input_a = 0;
			input_b = 0;
			output = 0;

			input_a = ReadSPMFlt(0, addr_input_a, 0);
			input_b = ReadSPMFlt(1, addr_input_b, 0);

			#define SPMAddressOf(x) (addr_##x)
			output = input_a + input_b;
			#undef SPMAddressOf

			WriteSPMFlt(2, addr_output, 0, output);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "VectorAddSample";}
		inline virtual int ArgumentCount(){return 3;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 14001;}
	};
}

#endif
