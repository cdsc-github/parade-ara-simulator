#ifndef LCACC_MODE_SWAPTIONS2_H
#define LCACC_MODE_SWAPTIONS2_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_swaptions2 : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_swaptions2(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_input = argAddrVec[0];

			if(argActive[0])
			{
				outputArgs.push_back(addr_input);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(1 < argAddrVec.size());
			uint64_t addr_output = argAddrVec[1];

			if(argActive[1])
			{
				outputArgs.push_back(addr_output);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 2);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_input = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output = LCACC_INTERNAL_argAddrVec[1];

			double input;
			double output;
			input = 0;
			output = 0;

			input = ReadSPMFlt(0, addr_input, 0);

			#define SPMAddressOf(x) (addr_##x)
			#define selectff(a, b, c, d) (((a) > (b)) ? (c) : (d))
			#define CumNormalInv(u, result) \
			{ \
			        float x = u - 0.5f; \
			        float r1a = x * x; \
			        float r1 = x * (((-25.44106049637f * r1a + 41.39119773534f) * r1a + -18.61500062529f) * r1a + 2.50662823884f) / ((((3.13082909833f * r1a + -21.06224101826f) * r1a + 23.08336743743f) * r1a + -8.47351093090f) * r1a + 1.0f); \
			        float r2a = logf(-1.0f * logf(u)); \
			        float r2 = 0.3374754822726147f + r2a * (0.9761690190917186f + r2a * (0.1607979714918209f + r2a * (0.0276438810333863f + r2a * (0.0038405729373609f + r2a * (0.0003951896511919f + r2a * (0.0000321767881768f + r2a * (0.0000002888167364f + r2a * 0.0000003960315187f))))))); \
			        result = selectff(x, 0.42f, selectff(-0.42f, x, r1, r2), r2); \
			}

			CumNormalInv(input, output);

			#undef CumNormalInv
			#undef selectff
			#undef SPMAddressOf

			WriteSPMFlt(1, addr_output, 0, output);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 2;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "swaptions2";}
		inline virtual int ArgumentCount(){return 2;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 1302;}
	};
}

#endif
