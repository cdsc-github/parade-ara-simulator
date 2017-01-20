#ifndef LCACC_MODE_DENOISE1MEGA_H
#define LCACC_MODE_DENOISE1MEGA_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_denoise1Mega : public LCAccOperatingMode
	{
		int width;
		int height;
		int depth;
	public:
		inline OperatingMode_denoise1Mega(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_a_Center = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_b_Center = argAddrVec[1];
			uint64_t addr_b_Right = addr_b_Center + 1 * GetArgumentWidth(1);
			uint64_t addr_b_Left = addr_b_Center - 1 * GetArgumentWidth(1);
			uint64_t addr_b_Down = addr_b_Center + (width + 2) * GetArgumentWidth(1);
			uint64_t addr_b_Up = addr_b_Center - (width + 2) * GetArgumentWidth(1);
			uint64_t addr_b_In = addr_b_Center + (width + 2) * (height + 2) * GetArgumentWidth(1);
			uint64_t addr_b_Out = addr_b_Center - (width + 2) * (height + 2) * GetArgumentWidth(1);

			if(argActive[0])
			{
				outputArgs.push_back(addr_a_Center);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_b_Center);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_b_Right);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_b_Left);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_b_Down);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_b_Up);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_b_In);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_b_Out);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(2 < argAddrVec.size());
			uint64_t addr_g = argAddrVec[2];

			if(argActive[2])
			{
				outputArgs.push_back(addr_g);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 3);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_a_Center = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_b_Center = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_g = LCACC_INTERNAL_argAddrVec[2];
			uint64_t addr_b_Right = addr_b_Center + 1 * GetArgumentWidth(1);
			uint64_t addr_b_Left = addr_b_Center - 1 * GetArgumentWidth(1);
			uint64_t addr_b_Down = addr_b_Center + (width + 2) * GetArgumentWidth(1);
			uint64_t addr_b_Up = addr_b_Center - (width + 2) * GetArgumentWidth(1);
			uint64_t addr_b_In = addr_b_Center + (width + 2) * (height + 2) * GetArgumentWidth(1);
			uint64_t addr_b_Out = addr_b_Center - (width + 2) * (height + 2) * GetArgumentWidth(1);

			double a_Center;
			double b_Center;
			double b_Right;
			double b_Left;
			double b_Down;
			double b_Up;
			double b_In;
			double b_Out;
			double g;
			a_Center = 0;
			b_Center = 0;
			b_Right = 0;
			b_Left = 0;
			b_Down = 0;
			b_Up = 0;
			b_In = 0;
			b_Out = 0;
			g = 0;

			a_Center = ReadSPMFlt(0, addr_a_Center, 0);
			b_Center = ReadSPMFlt(1, addr_b_Center, 0);
			b_Right = ReadSPMFlt(1, addr_b_Right, 0);
			b_Left = ReadSPMFlt(1, addr_b_Left, 0);
			b_Down = ReadSPMFlt(1, addr_b_Down, 0);
			b_Up = ReadSPMFlt(1, addr_b_Up, 0);
			b_In = ReadSPMFlt(1, addr_b_In, 0);
			b_Out = ReadSPMFlt(1, addr_b_Out, 0);

			#define SPMAddressOf(x) (addr_##x)
			float v1 = a_Center - b_Right;
			float v2 = a_Center - b_Left;
			float v3 = a_Center - b_In;
			float v4 = a_Center - b_Out;
			float v5 = a_Center - b_Up;
			float v6 = a_Center - b_Down;

			g = 1.0f / sqrt(v1 * v1 + v2 * v2 + v3 * v3 + v4 * v4 + v5 * v5 + v6 * v6);
			#undef SPMAddressOf

			WriteSPMFlt(2, addr_g, 0, g);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "denoise1Mega";}
		inline virtual int ArgumentCount(){return 3;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 3);
			width = ConvertTypes<uint64_t, int>(regs[0]);
			height = ConvertTypes<uint64_t, int>(regs[1]);
			depth = ConvertTypes<uint64_t, int>(regs[2]);
		}
		inline static int GetOpCode(){return 101;}
	};
}

#endif
