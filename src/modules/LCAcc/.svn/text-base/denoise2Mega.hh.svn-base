#ifndef LCACC_MODE_DENOISE2MEGA_H
#define LCACC_MODE_DENOISE2MEGA_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_denoise2Mega : public LCAccOperatingMode
	{
		int width;
		int height;
		int depth;
		float dt;
		float gamma;
		float sigma;
	public:
		inline OperatingMode_denoise2Mega(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_u_Center = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_g_Center = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_f_Center = argAddrVec[2];
			uint64_t addr_u_Right = addr_u_Center + 1 * GetArgumentWidth(0);
			uint64_t addr_u_Left = addr_u_Center - 1 * GetArgumentWidth(0);
			uint64_t addr_u_Down = addr_u_Center + (width + 2) * GetArgumentWidth(0);
			uint64_t addr_u_Up = addr_u_Center - (width + 2) * GetArgumentWidth(0);
			uint64_t addr_u_In = addr_u_Center + (width + 2) * (height + 2) * GetArgumentWidth(0);
			uint64_t addr_u_Out = addr_u_Center - (width + 2) * (height + 2) * GetArgumentWidth(0);
			uint64_t addr_g_Right = addr_g_Center + 1 * GetArgumentWidth(1);
			uint64_t addr_g_Left = addr_g_Center - 1 * GetArgumentWidth(1);
			uint64_t addr_g_Down = addr_g_Center + (width + 2) * GetArgumentWidth(1);
			uint64_t addr_g_Up = addr_g_Center - (width + 2) * GetArgumentWidth(1);
			uint64_t addr_g_In = addr_g_Center + (width + 2) * (height + 2) * GetArgumentWidth(1);
			uint64_t addr_g_Out = addr_g_Center - (width + 2) * (height + 2) * GetArgumentWidth(1);

			if(argActive[0])
			{
				outputArgs.push_back(addr_u_Center);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_g_Center);
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_f_Center);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_u_Right);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_u_Left);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_u_Down);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_u_Up);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_u_In);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_u_Out);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_g_Right);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_g_Left);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_g_Down);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_g_Up);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_g_In);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_g_Out);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(3 < argAddrVec.size());
			uint64_t addr_u_Result = argAddrVec[3];

			if(argActive[3])
			{
				outputArgs.push_back(addr_u_Result);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 4);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_u_Center = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_g_Center = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_f_Center = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_u_Result = LCACC_INTERNAL_argAddrVec[3];
			uint64_t addr_u_Right = addr_u_Center + 1 * GetArgumentWidth(0);
			uint64_t addr_u_Left = addr_u_Center - 1 * GetArgumentWidth(0);
			uint64_t addr_u_Down = addr_u_Center + (width + 2) * GetArgumentWidth(0);
			uint64_t addr_u_Up = addr_u_Center - (width + 2) * GetArgumentWidth(0);
			uint64_t addr_u_In = addr_u_Center + (width + 2) * (height + 2) * GetArgumentWidth(0);
			uint64_t addr_u_Out = addr_u_Center - (width + 2) * (height + 2) * GetArgumentWidth(0);
			uint64_t addr_g_Right = addr_g_Center + 1 * GetArgumentWidth(1);
			uint64_t addr_g_Left = addr_g_Center - 1 * GetArgumentWidth(1);
			uint64_t addr_g_Down = addr_g_Center + (width + 2) * GetArgumentWidth(1);
			uint64_t addr_g_Up = addr_g_Center - (width + 2) * GetArgumentWidth(1);
			uint64_t addr_g_In = addr_g_Center + (width + 2) * (height + 2) * GetArgumentWidth(1);
			uint64_t addr_g_Out = addr_g_Center - (width + 2) * (height + 2) * GetArgumentWidth(1);

			double u_Center;
			double g_Center;
			double f_Center;
			double u_Right;
			double u_Left;
			double u_Down;
			double u_Up;
			double u_In;
			double u_Out;
			double g_Right;
			double g_Left;
			double g_Down;
			double g_Up;
			double g_In;
			double g_Out;
			double u_Result;
			u_Center = 0;
			g_Center = 0;
			f_Center = 0;
			u_Right = 0;
			u_Left = 0;
			u_Down = 0;
			u_Up = 0;
			u_In = 0;
			u_Out = 0;
			g_Right = 0;
			g_Left = 0;
			g_Down = 0;
			g_Up = 0;
			g_In = 0;
			g_Out = 0;
			u_Result = 0;

			u_Center = ReadSPMFlt(0, addr_u_Center, 0);
			g_Center = ReadSPMFlt(1, addr_g_Center, 0);
			f_Center = ReadSPMFlt(2, addr_f_Center, 0);
			u_Right = ReadSPMFlt(0, addr_u_Right, 0);
			u_Left = ReadSPMFlt(0, addr_u_Left, 0);
			u_Down = ReadSPMFlt(0, addr_u_Down, 0);
			u_Up = ReadSPMFlt(0, addr_u_Up, 0);
			u_In = ReadSPMFlt(0, addr_u_In, 0);
			u_Out = ReadSPMFlt(0, addr_u_Out, 0);
			g_Right = ReadSPMFlt(1, addr_g_Right, 0);
			g_Left = ReadSPMFlt(1, addr_g_Left, 0);
			g_Down = ReadSPMFlt(1, addr_g_Down, 0);
			g_Up = ReadSPMFlt(1, addr_g_Up, 0);
			g_In = ReadSPMFlt(1, addr_g_In, 0);
			g_Out = ReadSPMFlt(1, addr_g_Out, 0);

			#define SPMAddressOf(x) (addr_##x)
			float r = u_Center * f_Center / sigma;
			
			r = (r * (2.38944f + r * (0.950037f + r))) / (4.65314f + r * (2.57541f + r * (1.48937f + r)));
			
			u_Result = (u_Center + dt * (u_Right * g_Right + u_Left * g_Left + u_Down * g_Down + u_Up * g_Up + u_Out * g_Out + u_In * g_In + gamma * f_Center * r) ) / (1.0f + dt * (g_Right + g_Left + g_Down + g_Up + g_Out + g_In + gamma));
			#undef SPMAddressOf

			WriteSPMFlt(3, addr_u_Result, 0, u_Result);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "denoise2Mega";}
		inline virtual int ArgumentCount(){return 4;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 6);
			width = ConvertTypes<uint64_t, int>(regs[0]);
			height = ConvertTypes<uint64_t, int>(regs[1]);
			depth = ConvertTypes<uint64_t, int>(regs[2]);
			dt = ConvertTypes<uint64_t, float>(regs[3]);
			gamma = ConvertTypes<uint64_t, float>(regs[4]);
			sigma = ConvertTypes<uint64_t, float>(regs[5]);
		}
		inline static int GetOpCode(){return 104;}
	};
}

#endif
