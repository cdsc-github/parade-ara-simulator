#ifndef LCACC_MODE_DEBLUR1MEGA_H
#define LCACC_MODE_DEBLUR1MEGA_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_deblur1Mega : public LCAccOperatingMode
	{
		float sigma;
	public:
		inline OperatingMode_deblur1Mega(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_conv_in = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_f = argAddrVec[1];

			if(argActive[0])
			{
				outputArgs.push_back(addr_conv_in);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_f);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(2 < argAddrVec.size());
			uint64_t addr_conv_out = argAddrVec[2];

			if(argActive[2])
			{
				outputArgs.push_back(addr_conv_out);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 3);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_conv_in = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_f = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_conv_out = LCACC_INTERNAL_argAddrVec[2];

			double conv_in;
			double f;
			double conv_out;
			conv_in = 0;
			f = 0;
			conv_out = 0;

			conv_in = ReadSPMFlt(0, addr_conv_in, 0);
			f = ReadSPMFlt(1, addr_f, 0);

			#define SPMAddressOf(x) (addr_##x)
			float r = conv_in * f / sigma;
			r = (r * (2.38944f + r * (0.950037f + r))) / (4.65314f + r * (2.57541f + r * (1.48937f + r)));
			conv_out = conv_in - f * r;
			#undef SPMAddressOf

			WriteSPMFlt(2, addr_conv_out, 0, conv_out);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "deblur1Mega";}
		inline virtual int ArgumentCount(){return 3;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 1);
			sigma = ConvertTypes<uint64_t, float>(regs[0]);
		}
		inline static int GetOpCode(){return 201;}
	};
}

#endif
