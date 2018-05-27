#ifndef LCACC_MODE_FFT_H
#define LCACC_MODE_FFT_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_fft : public LCAccOperatingMode
	{
		int elemCount;
	public:
		inline OperatingMode_fft(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_input = argAddrVec[0];

			if(argActive[0])
			{
				for(size_t i = 0; i < ((2)) * GetArgumentWidth(0); i += GetArgumentWidth(0))
				{
					outputArgs.push_back(addr_input + i);
				}
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(1 < argAddrVec.size());
			uint64_t addr_output = argAddrVec[1];

			if(argActive[1])
			{
				for(size_t i = 0; i < ((2)) * GetArgumentWidth(1); i += GetArgumentWidth(1))
				{
					outputArgs.push_back(addr_output + i);
				}
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 2);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_input = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output = LCACC_INTERNAL_argAddrVec[1];

			double input[2];
			double output[2];
			for(int i = 0; i < ((2)); i++)
			{
				input[(i) % (2)] = (double)0;
			}
			for(int i = 0; i < ((2)); i++)
			{
				output[(i) % (2)] = (double)0;
			}

			for(size_t i = 0; i < ((2)); i++)
			{
				input[(i) % (2)] = ReadSPMFlt(0, addr_input, i);
			}

			#define SPMAddressOf(x) (addr_##x)
			#undef SPMAddressOf

			for(size_t i = 0; i < ((2)); i++)
			{
				WriteSPMFlt(1, addr_output, i, output[(i) % (2)]);
			}
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 16;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "fft";}
		inline virtual int ArgumentCount(){return 2;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 1);
			elemCount = ConvertTypes<uint64_t, int>(regs[0]);
		}
		inline static int GetOpCode(){return 701;}
	};
}

#endif
