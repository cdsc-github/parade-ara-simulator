#ifndef LCACC_MODE_SWAPTIONS1_H
#define LCACC_MODE_SWAPTIONS1_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_swaptions1 : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_swaptions1(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_randSeed = argAddrVec[0];

			if(argActive[0])
			{
				outputArgs.push_back(addr_randSeed);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(1 < argAddrVec.size());
			uint64_t addr_randResult = argAddrVec[1];

			if(argActive[1])
			{
				for(size_t i = 0; i < 33 * GetArgumentWidth(1); i += GetArgumentWidth(1))
				{
					outputArgs.push_back(addr_randResult + i);
				}
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 2);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_randSeed = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_randResult = LCACC_INTERNAL_argAddrVec[1];

			double randSeed;
			double randResult[3][11];
			randSeed = 0;
			for(int i = 0; i < 33; i++)
			{
				randResult[(i) % (3)][((i) / ((1 * 3 * 11))) % (11)] = (double)0;
			}

			randSeed = ReadSPMFlt(0, addr_randSeed, 0);

			#define SPMAddressOf(x) (addr_##x)
			#define RanUnif(x, result) \
			{ \
			        float ix, k1; \
			        float dRes; \
			        ix = x; \
			        k1 = ix * 7.8263796e-6f; \
			        ix = 16807.0f * (ix - k1 * 127773.0f) - k1 * 2836.0f; \
			        x = ix; \
			        dRes = ix * 4.656612875e-10f; \
			        result = dRes; \
			}
			
			#define randVal(factor, iN) randResult[factor][iN]
			
			#define FillRandVal_Fac(x) \
			        RanUnif(randSeed, randVal(x, 0)); \
			        RanUnif(randSeed, randVal(x, 1)); \
			        RanUnif(randSeed, randVal(x, 2)); \
			        RanUnif(randSeed, randVal(x, 3)); \
			        RanUnif(randSeed, randVal(x, 4)); \
			        RanUnif(randSeed, randVal(x, 5)); \
			        RanUnif(randSeed, randVal(x, 6)); \
			        RanUnif(randSeed, randVal(x, 7)); \
			        RanUnif(randSeed, randVal(x, 8)); \
			        RanUnif(randSeed, randVal(x, 9)); \
			        RanUnif(randSeed, randVal(x, 10));
			
			
			                FillRandVal_Fac(0);
			                FillRandVal_Fac(1);
			                FillRandVal_Fac(2);
			
			#undef randVal
			#undef RanUnif
			#undef FillRandVal
			#undef SPMAddressOf

			for(size_t i = 0; i < 33; i++)
			{
				WriteSPMFlt(1, addr_randResult, i, randResult[(i) % (3)][((i) / ((1 * 3 * 11))) % (11)]);
			}
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 2;}
		inline virtual int PipelineDepth(){return 60;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "swaptions1";}
		inline virtual int ArgumentCount(){return 2;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 1301;}
	};
}

#endif
