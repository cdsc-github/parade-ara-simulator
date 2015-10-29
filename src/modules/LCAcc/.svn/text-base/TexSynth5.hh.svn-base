#ifndef LCACC_MODE_TEXSYNTH5_H
#define LCACC_MODE_TEXSYNTH5_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_TexSynth5 : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_TexSynth5(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_in_diff0 = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_in_diff1 = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_in_diff2 = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_in_diff3 = argAddrVec[3];
			assert(4 < argAddrVec.size());
			uint64_t addr_in_pixel0 = argAddrVec[4];
			assert(5 < argAddrVec.size());
			uint64_t addr_in_pixel1 = argAddrVec[5];
			assert(6 < argAddrVec.size());
			uint64_t addr_in_pixel2 = argAddrVec[6];
			assert(7 < argAddrVec.size());
			uint64_t addr_in_pixel3 = argAddrVec[7];

			if(argActive[0])
			{
				outputArgs.push_back(addr_in_diff0);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_in_diff1);
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_in_diff2);
			}
			if(argActive[3])
			{
				outputArgs.push_back(addr_in_diff3);
			}
			if(argActive[4])
			{
				outputArgs.push_back(addr_in_pixel0);
			}
			if(argActive[5])
			{
				outputArgs.push_back(addr_in_pixel1);
			}
			if(argActive[6])
			{
				outputArgs.push_back(addr_in_pixel2);
			}
			if(argActive[7])
			{
				outputArgs.push_back(addr_in_pixel3);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(8 < argAddrVec.size());
			uint64_t addr_out_pixel = argAddrVec[8];

			if(argActive[8])
			{
				outputArgs.push_back(addr_out_pixel);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 9);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_diff0 = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_diff1 = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_diff2 = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_diff3 = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_pixel0 = LCACC_INTERNAL_argAddrVec[4];
			assert(5 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_pixel1 = LCACC_INTERNAL_argAddrVec[5];
			assert(6 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_pixel2 = LCACC_INTERNAL_argAddrVec[6];
			assert(7 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_pixel3 = LCACC_INTERNAL_argAddrVec[7];
			assert(8 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_out_pixel = LCACC_INTERNAL_argAddrVec[8];

			double in_diff0;
			double in_diff1;
			double in_diff2;
			double in_diff3;
			double in_pixel0;
			double in_pixel1;
			double in_pixel2;
			double in_pixel3;
			double out_pixel;
			in_diff0 = 0;
			in_diff1 = 0;
			in_diff2 = 0;
			in_diff3 = 0;
			in_pixel0 = 0;
			in_pixel1 = 0;
			in_pixel2 = 0;
			in_pixel3 = 0;
			out_pixel = 0;

			in_diff0 = ReadSPMFlt(0, addr_in_diff0, 0);
			in_diff1 = ReadSPMFlt(1, addr_in_diff1, 0);
			in_diff2 = ReadSPMFlt(2, addr_in_diff2, 0);
			in_diff3 = ReadSPMFlt(3, addr_in_diff3, 0);
			in_pixel0 = ReadSPMFlt(4, addr_in_pixel0, 0);
			in_pixel1 = ReadSPMFlt(5, addr_in_pixel1, 0);
			in_pixel2 = ReadSPMFlt(6, addr_in_pixel2, 0);
			in_pixel3 = ReadSPMFlt(7, addr_in_pixel3, 0);

			#define SPMAddressOf(x) (addr_##x)
			if(in_diff0 > in_diff1)
			{
				if(in_diff0 > in_diff2)
				{
					if(in_diff0 > in_diff3)
					{//0 is greatest
						out_pixel = in_pixel0;
					}
					else
					{//3 is greatest
						out_pixel = in_pixel3;
					}
				}
				else
				{
					if(in_diff2 > in_diff3)
					{//2 is greatest
						out_pixel = in_pixel2;
					}
					else
					{//3 is greatest
						out_pixel = in_pixel3;
					}
				}
			}
			else
			{
				if(in_diff1 > in_diff2)
				{
					if(in_diff1 > in_diff3)
					{//1 is greatest
						out_pixel = in_pixel1;
					}
					else
					{//3 is greatest
						out_pixel = in_pixel3;
					}
				}
				else
				{
					if(in_diff2 > in_diff3)
					{//2 is greatest
						out_pixel = in_pixel2;
					}
					else
					{//3 is greatest
						out_pixel = in_pixel3;
					}
				}
			}
			#undef SPMAddressOf

			WriteSPMFlt(8, addr_out_pixel, 0, out_pixel);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "TexSynth5";}
		inline virtual int ArgumentCount(){return 9;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 914;}
	};
}

#endif
