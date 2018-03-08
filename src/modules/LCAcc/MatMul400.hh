#ifndef LCACC_MODE_MATMUL400_H
#define LCACC_MODE_MATMUL400_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_MatMul400 : public LCAccOperatingMode
	{
		int vectorLength;
		int currentIndex;
		float currentAccumulation;
	public:
		inline OperatingMode_MatMul400(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_input1 = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_input2 = argAddrVec[1];

			if(argActive[0])
			{
				outputArgs.push_back(addr_input1);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_input2);
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
			uint64_t addr_input1 = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_input2 = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output = LCACC_INTERNAL_argAddrVec[2];

			double input1;
			double input2;
			double output;
			input1 = 0;
			input2 = 0;
			output = 0;

			input1 = ReadSPMFlt(0, addr_input1, 0);
			input2 = ReadSPMFlt(1, addr_input2, 0);

			#define SPMAddressOf(x) (addr_##x)
			currentAccumulation += input1 * input2;
			currentIndex++;
			
			if(currentIndex == vectorLength)
			{
				currentAccumulation = 0.0f;
				currentIndex = 0;
			}
			#undef SPMAddressOf

			WriteSPMFlt(2, addr_output, 0, output);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 5;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 512;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "MatMul400";}
		inline virtual int ArgumentCount(){return 3;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 3);
			vectorLength = ConvertTypes<uint64_t, int>(regs[0]);
			currentIndex = ConvertTypes<uint64_t, int>(regs[1]);
			currentAccumulation = ConvertTypes<uint64_t, float>(regs[2]);
		}
		inline static int GetOpCode(){return 108;}
	};
}

#endif
