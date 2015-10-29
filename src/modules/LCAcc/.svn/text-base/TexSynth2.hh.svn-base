#ifndef LCACC_MODE_TEXSYNTH2_H
#define LCACC_MODE_TEXSYNTH2_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_TexSynth2 : public LCAccOperatingMode
	{
		uint32_t output0;
		uint32_t output1;
		uint32_t output2;
		uint32_t output3;
		int32_t offsetX0;
		int32_t offsetY0;
		int32_t offsetX1;
		int32_t offsetY1;
		int32_t offsetX2;
		int32_t offsetY2;
		int32_t offsetX3;
		int32_t offsetY3;
		uint32_t elementSize;
		uint32_t elementCount;
		intptr_t inputAddr;
		uint32_t width;
		uint32_t height;
		int line;
	public:
		inline OperatingMode_TexSynth2(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_dummy = argAddrVec[0];

			if(argActive[0])
			{
				outputArgs.push_back(addr_dummy);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{

		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 1);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_dummy = LCACC_INTERNAL_argAddrVec[0];

			double dummy;
			dummy = 0;

			dummy = ReadSPMFlt(0, addr_dummy, 0);

			#define SPMAddressOf(x) (addr_##x)
			int index = (iteration + maxIteration * taskID);
			int imageIndex = index / (width * height);
			int imagePitch = width * height * elementSize * elementCount * imageIndex;
			int in_j = line;
			int in_i = index % width;
			
			for(int i = 0; i < elementCount; i++)
			{
				int x = (in_i + offsetX0 + width) % width;
				int y = (in_j + offsetY0 + height) % height;
				AddRead(inputAddr + imagePitch + (x + width * y) * (elementSize * elementCount) + (i * elementSize), output0 + iteration * (elementSize * elementCount) + (i * elementSize), elementSize);
			}
			for(int i = 0; i < elementCount; i++)
			{
				int x = (in_i + offsetX1 + width) % width;
				int y = (in_j + offsetY1 + height) % height; 
				AddRead(inputAddr + imagePitch + (x + width * y) * (elementSize * elementCount) + (i * elementSize), output1 + iteration * (elementSize * elementCount) + (i * elementSize), elementSize);
			}
			for(int i = 0; i < elementCount; i++)
			{
				int x = (in_i + offsetX2 + width) % width;
				int y = (in_j + offsetY2 + height) % height; 
				AddRead(inputAddr + imagePitch + (x + width * y) * (elementSize * elementCount) + (i * elementSize), output2 + iteration * (elementSize * elementCount) + (i * elementSize), elementSize);
			}
			for(int i = 0; i < elementCount; i++)
			{
				int x = (in_i + offsetX3 + width) % width;
				int y = (in_j + offsetY3 + height) % height; 
				AddRead(inputAddr + imagePitch + (x + width * y) * (elementSize * elementCount) + (i * elementSize), output3 + iteration * (elementSize * elementCount) + (i * elementSize), elementSize);
			}
			#undef SPMAddressOf

		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 21;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "TexSynth2";}
		inline virtual int ArgumentCount(){return 1;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 18);
			output0 = ConvertTypes<uint64_t, uint32_t>(regs[0]);
			output1 = ConvertTypes<uint64_t, uint32_t>(regs[1]);
			output2 = ConvertTypes<uint64_t, uint32_t>(regs[2]);
			output3 = ConvertTypes<uint64_t, uint32_t>(regs[3]);
			offsetX0 = ConvertTypes<uint64_t, int32_t>(regs[4]);
			offsetY0 = ConvertTypes<uint64_t, int32_t>(regs[5]);
			offsetX1 = ConvertTypes<uint64_t, int32_t>(regs[6]);
			offsetY1 = ConvertTypes<uint64_t, int32_t>(regs[7]);
			offsetX2 = ConvertTypes<uint64_t, int32_t>(regs[8]);
			offsetY2 = ConvertTypes<uint64_t, int32_t>(regs[9]);
			offsetX3 = ConvertTypes<uint64_t, int32_t>(regs[10]);
			offsetY3 = ConvertTypes<uint64_t, int32_t>(regs[11]);
			elementSize = ConvertTypes<uint64_t, uint32_t>(regs[12]);
			elementCount = ConvertTypes<uint64_t, uint32_t>(regs[13]);
			inputAddr = ConvertTypes<uint64_t, intptr_t>(regs[14]);
			width = ConvertTypes<uint64_t, uint32_t>(regs[15]);
			height = ConvertTypes<uint64_t, uint32_t>(regs[16]);
			line = ConvertTypes<uint64_t, int>(regs[17]);
		}
		inline static int GetOpCode(){return 911;}
	};
}

#endif
