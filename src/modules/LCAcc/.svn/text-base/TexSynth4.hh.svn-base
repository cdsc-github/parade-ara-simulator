#ifndef LCACC_MODE_TEXSYNTH4_H
#define LCACC_MODE_TEXSYNTH4_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_TexSynth4 : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_TexSynth4(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_in_image0 = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_in_image1 = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_in_image2 = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_in_image3 = argAddrVec[3];
			assert(4 < argAddrVec.size());
			uint64_t addr_in_image4 = argAddrVec[4];
			assert(5 < argAddrVec.size());
			uint64_t addr_in_image5 = argAddrVec[5];
			assert(6 < argAddrVec.size());
			uint64_t addr_in_image6 = argAddrVec[6];
			assert(7 < argAddrVec.size());
			uint64_t addr_in_image7 = argAddrVec[7];
			assert(8 < argAddrVec.size());
			uint64_t addr_in_target0 = argAddrVec[8];
			assert(9 < argAddrVec.size());
			uint64_t addr_in_target1 = argAddrVec[9];
			assert(10 < argAddrVec.size());
			uint64_t addr_in_target2 = argAddrVec[10];
			assert(11 < argAddrVec.size());
			uint64_t addr_in_target3 = argAddrVec[11];
			assert(12 < argAddrVec.size());
			uint64_t addr_in_result0 = argAddrVec[12];
			assert(13 < argAddrVec.size());
			uint64_t addr_in_result1 = argAddrVec[13];
			assert(14 < argAddrVec.size());
			uint64_t addr_in_result2 = argAddrVec[14];
			assert(15 < argAddrVec.size());
			uint64_t addr_in_result3 = argAddrVec[15];

			if(argActive[0])
			{
				outputArgs.push_back(addr_in_image0);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_in_image1);
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_in_image2);
			}
			if(argActive[3])
			{
				outputArgs.push_back(addr_in_image3);
			}
			if(argActive[4])
			{
				outputArgs.push_back(addr_in_image4);
			}
			if(argActive[5])
			{
				outputArgs.push_back(addr_in_image5);
			}
			if(argActive[6])
			{
				outputArgs.push_back(addr_in_image6);
			}
			if(argActive[7])
			{
				outputArgs.push_back(addr_in_image7);
			}
			if(argActive[8])
			{
				outputArgs.push_back(addr_in_target0);
			}
			if(argActive[9])
			{
				outputArgs.push_back(addr_in_target1);
			}
			if(argActive[10])
			{
				outputArgs.push_back(addr_in_target2);
			}
			if(argActive[11])
			{
				outputArgs.push_back(addr_in_target3);
			}
			if(argActive[12])
			{
				outputArgs.push_back(addr_in_result0);
			}
			if(argActive[13])
			{
				outputArgs.push_back(addr_in_result1);
			}
			if(argActive[14])
			{
				outputArgs.push_back(addr_in_result2);
			}
			if(argActive[15])
			{
				outputArgs.push_back(addr_in_result3);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(16 < argAddrVec.size());
			uint64_t addr_out_diff = argAddrVec[16];

			if(argActive[16])
			{
				outputArgs.push_back(addr_out_diff);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 17);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_image0 = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_image1 = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_image2 = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_image3 = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_image4 = LCACC_INTERNAL_argAddrVec[4];
			assert(5 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_image5 = LCACC_INTERNAL_argAddrVec[5];
			assert(6 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_image6 = LCACC_INTERNAL_argAddrVec[6];
			assert(7 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_image7 = LCACC_INTERNAL_argAddrVec[7];
			assert(8 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_target0 = LCACC_INTERNAL_argAddrVec[8];
			assert(9 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_target1 = LCACC_INTERNAL_argAddrVec[9];
			assert(10 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_target2 = LCACC_INTERNAL_argAddrVec[10];
			assert(11 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_target3 = LCACC_INTERNAL_argAddrVec[11];
			assert(12 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_result0 = LCACC_INTERNAL_argAddrVec[12];
			assert(13 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_result1 = LCACC_INTERNAL_argAddrVec[13];
			assert(14 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_result2 = LCACC_INTERNAL_argAddrVec[14];
			assert(15 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_result3 = LCACC_INTERNAL_argAddrVec[15];
			assert(16 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_out_diff = LCACC_INTERNAL_argAddrVec[16];

			double in_image0;
			double in_image1;
			double in_image2;
			double in_image3;
			double in_image4;
			double in_image5;
			double in_image6;
			double in_image7;
			double in_target0;
			double in_target1;
			double in_target2;
			double in_target3;
			double in_result0;
			double in_result1;
			double in_result2;
			double in_result3;
			double out_diff;
			in_image0 = 0;
			in_image1 = 0;
			in_image2 = 0;
			in_image3 = 0;
			in_image4 = 0;
			in_image5 = 0;
			in_image6 = 0;
			in_image7 = 0;
			in_target0 = 0;
			in_target1 = 0;
			in_target2 = 0;
			in_target3 = 0;
			in_result0 = 0;
			in_result1 = 0;
			in_result2 = 0;
			in_result3 = 0;
			out_diff = 0;

			in_image0 = ReadSPMFlt(0, addr_in_image0, 0);
			in_image1 = ReadSPMFlt(1, addr_in_image1, 0);
			in_image2 = ReadSPMFlt(2, addr_in_image2, 0);
			in_image3 = ReadSPMFlt(3, addr_in_image3, 0);
			in_image4 = ReadSPMFlt(4, addr_in_image4, 0);
			in_image5 = ReadSPMFlt(5, addr_in_image5, 0);
			in_image6 = ReadSPMFlt(6, addr_in_image6, 0);
			in_image7 = ReadSPMFlt(7, addr_in_image7, 0);
			in_target0 = ReadSPMFlt(8, addr_in_target0, 0);
			in_target1 = ReadSPMFlt(9, addr_in_target1, 0);
			in_target2 = ReadSPMFlt(10, addr_in_target2, 0);
			in_target3 = ReadSPMFlt(11, addr_in_target3, 0);
			in_result0 = ReadSPMFlt(12, addr_in_result0, 0);
			in_result1 = ReadSPMFlt(13, addr_in_result1, 0);
			in_result2 = ReadSPMFlt(14, addr_in_result2, 0);
			in_result3 = ReadSPMFlt(15, addr_in_result3, 0);

			#define SPMAddressOf(x) (addr_##x)
			#define sqr(x) ((x) * (x))
			#define upperPixel(x) diff += sqr(in_image##x - in_result##x)
			#define lowerPixel(initial, target) if(in_target##target < 1.0f) diff += sqr(in_image##initial - in_target##target)
			
			float diff = 0.0f;
			
			upperPixel(0);
			upperPixel(1);
			upperPixel(2);
			upperPixel(3);
			lowerPixel(4, 0);
			lowerPixel(5, 1);
			lowerPixel(6, 2);
			lowerPixel(7, 3);
			
			out_diff = diff;
			
			#undef sqr
			#undef upperPixel
			#undef lowerPixel
			#undef SPMAddressOf

			WriteSPMFlt(16, addr_out_diff, 0, out_diff);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 2;}
		inline virtual int PipelineDepth(){return 89;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "TexSynth4";}
		inline virtual int ArgumentCount(){return 17;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 913;}
	};
}

#endif
