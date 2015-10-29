#ifndef LCACC_MODE_SEGMENTATION1MEGA_H
#define LCACC_MODE_SEGMENTATION1MEGA_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_segmentation1Mega : public LCAccOperatingMode
	{
		int width;
		int height;
		int depth;
		float dx;
		float dy;
		float dz;
		float lambda1;
		float lambda2;
		float mu;
		float c1;
		float c2;
	public:
		inline OperatingMode_segmentation1Mega(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_phi_Center = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_u0 = argAddrVec[1];
			uint64_t addr_phi_Left = addr_phi_Center - 1 * GetArgumentWidth(0);
			uint64_t addr_phi_Right = addr_phi_Center + 1 * GetArgumentWidth(0);
			uint64_t addr_phi_Up = addr_phi_Center - width * GetArgumentWidth(0);
			uint64_t addr_phi_Down = addr_phi_Center + width * GetArgumentWidth(0);
			uint64_t addr_phi_In = addr_phi_Center + width * height * GetArgumentWidth(0);
			uint64_t addr_phi_Out = addr_phi_Center - width * height * GetArgumentWidth(0);
			uint64_t addr_phi_LeftUp = addr_phi_Center - 1 * GetArgumentWidth(0) - width * GetArgumentWidth(0);
			uint64_t addr_phi_LeftDown = addr_phi_Center - 1 * GetArgumentWidth(0) + width * GetArgumentWidth(0);
			uint64_t addr_phi_LeftIn = addr_phi_Center - 1 * GetArgumentWidth(0) + width * height * GetArgumentWidth(0);
			uint64_t addr_phi_LeftOut = addr_phi_Center - 1 * GetArgumentWidth(0) - width * height * GetArgumentWidth(0);
			uint64_t addr_phi_RightUp = addr_phi_Center + 1 * GetArgumentWidth(0) - width * GetArgumentWidth(0);
			uint64_t addr_phi_RightDown = addr_phi_Center + 1 * GetArgumentWidth(0) + width * GetArgumentWidth(0);
			uint64_t addr_phi_RightIn = addr_phi_Center + 1 * GetArgumentWidth(0) + width * height * GetArgumentWidth(0);
			uint64_t addr_phi_RightOut = addr_phi_Center + 1 * GetArgumentWidth(0) - width * height * GetArgumentWidth(0);
			uint64_t addr_phi_UpIn = addr_phi_Center - width * GetArgumentWidth(0) + width * height * GetArgumentWidth(0);
			uint64_t addr_phi_UpOut = addr_phi_Center - width * GetArgumentWidth(0) - width * height * GetArgumentWidth(0);
			uint64_t addr_phi_DownIn = addr_phi_Center + width * GetArgumentWidth(0) + width * height * GetArgumentWidth(0);
			uint64_t addr_phi_DownOut = addr_phi_Center + width * GetArgumentWidth(0) - width * height * GetArgumentWidth(0);

			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_Center);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_Left);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_Right);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_Up);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_Down);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_In);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_Out);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_LeftUp);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_LeftDown);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_LeftIn);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_LeftOut);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_RightUp);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_RightDown);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_RightIn);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_RightOut);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_UpIn);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_UpOut);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_DownIn);
			}
			if(argActive[0])
			{
				outputArgs.push_back(addr_phi_DownOut);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_u0);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(2 < argAddrVec.size());
			uint64_t addr_result = argAddrVec[2];

			if(argActive[2])
			{
				outputArgs.push_back(addr_result);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 3);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_phi_Center = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_u0 = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_result = LCACC_INTERNAL_argAddrVec[2];
			uint64_t addr_phi_Left = addr_phi_Center - 1 * GetArgumentWidth(0);
			uint64_t addr_phi_Right = addr_phi_Center + 1 * GetArgumentWidth(0);
			uint64_t addr_phi_Up = addr_phi_Center - width * GetArgumentWidth(0);
			uint64_t addr_phi_Down = addr_phi_Center + width * GetArgumentWidth(0);
			uint64_t addr_phi_In = addr_phi_Center + width * height * GetArgumentWidth(0);
			uint64_t addr_phi_Out = addr_phi_Center - width * height * GetArgumentWidth(0);
			uint64_t addr_phi_LeftUp = addr_phi_Center - 1 * GetArgumentWidth(0) - width * GetArgumentWidth(0);
			uint64_t addr_phi_LeftDown = addr_phi_Center - 1 * GetArgumentWidth(0) + width * GetArgumentWidth(0);
			uint64_t addr_phi_LeftIn = addr_phi_Center - 1 * GetArgumentWidth(0) + width * height * GetArgumentWidth(0);
			uint64_t addr_phi_LeftOut = addr_phi_Center - 1 * GetArgumentWidth(0) - width * height * GetArgumentWidth(0);
			uint64_t addr_phi_RightUp = addr_phi_Center + 1 * GetArgumentWidth(0) - width * GetArgumentWidth(0);
			uint64_t addr_phi_RightDown = addr_phi_Center + 1 * GetArgumentWidth(0) + width * GetArgumentWidth(0);
			uint64_t addr_phi_RightIn = addr_phi_Center + 1 * GetArgumentWidth(0) + width * height * GetArgumentWidth(0);
			uint64_t addr_phi_RightOut = addr_phi_Center + 1 * GetArgumentWidth(0) - width * height * GetArgumentWidth(0);
			uint64_t addr_phi_UpIn = addr_phi_Center - width * GetArgumentWidth(0) + width * height * GetArgumentWidth(0);
			uint64_t addr_phi_UpOut = addr_phi_Center - width * GetArgumentWidth(0) - width * height * GetArgumentWidth(0);
			uint64_t addr_phi_DownIn = addr_phi_Center + width * GetArgumentWidth(0) + width * height * GetArgumentWidth(0);
			uint64_t addr_phi_DownOut = addr_phi_Center + width * GetArgumentWidth(0) - width * height * GetArgumentWidth(0);

			double phi_Center;
			double phi_Left;
			double phi_Right;
			double phi_Up;
			double phi_Down;
			double phi_In;
			double phi_Out;
			double phi_LeftUp;
			double phi_LeftDown;
			double phi_LeftIn;
			double phi_LeftOut;
			double phi_RightUp;
			double phi_RightDown;
			double phi_RightIn;
			double phi_RightOut;
			double phi_UpIn;
			double phi_UpOut;
			double phi_DownIn;
			double phi_DownOut;
			double u0;
			double result;
			phi_Center = 0;
			phi_Left = 0;
			phi_Right = 0;
			phi_Up = 0;
			phi_Down = 0;
			phi_In = 0;
			phi_Out = 0;
			phi_LeftUp = 0;
			phi_LeftDown = 0;
			phi_LeftIn = 0;
			phi_LeftOut = 0;
			phi_RightUp = 0;
			phi_RightDown = 0;
			phi_RightIn = 0;
			phi_RightOut = 0;
			phi_UpIn = 0;
			phi_UpOut = 0;
			phi_DownIn = 0;
			phi_DownOut = 0;
			u0 = 0;
			result = 0;

			phi_Center = ReadSPMFlt(0, addr_phi_Center, 0);
			phi_Left = ReadSPMFlt(0, addr_phi_Left, 0);
			phi_Right = ReadSPMFlt(0, addr_phi_Right, 0);
			phi_Up = ReadSPMFlt(0, addr_phi_Up, 0);
			phi_Down = ReadSPMFlt(0, addr_phi_Down, 0);
			phi_In = ReadSPMFlt(0, addr_phi_In, 0);
			phi_Out = ReadSPMFlt(0, addr_phi_Out, 0);
			phi_LeftUp = ReadSPMFlt(0, addr_phi_LeftUp, 0);
			phi_LeftDown = ReadSPMFlt(0, addr_phi_LeftDown, 0);
			phi_LeftIn = ReadSPMFlt(0, addr_phi_LeftIn, 0);
			phi_LeftOut = ReadSPMFlt(0, addr_phi_LeftOut, 0);
			phi_RightUp = ReadSPMFlt(0, addr_phi_RightUp, 0);
			phi_RightDown = ReadSPMFlt(0, addr_phi_RightDown, 0);
			phi_RightIn = ReadSPMFlt(0, addr_phi_RightIn, 0);
			phi_RightOut = ReadSPMFlt(0, addr_phi_RightOut, 0);
			phi_UpIn = ReadSPMFlt(0, addr_phi_UpIn, 0);
			phi_UpOut = ReadSPMFlt(0, addr_phi_UpOut, 0);
			phi_DownIn = ReadSPMFlt(0, addr_phi_DownIn, 0);
			phi_DownOut = ReadSPMFlt(0, addr_phi_DownOut, 0);
			u0 = ReadSPMFlt(1, addr_u0, 0);

			#define SPMAddressOf(x) (addr_##x)
			float Dx_0 = (phi_Right - phi_Left)/(dx * 2.0f);
			float Dy_0 = (phi_Down - phi_Up)/(dy * 2.0f);
			float Dz_0 = (phi_In - phi_Out)/(dz * 2.0f);
			float Dxx = (phi_Right + phi_Left - 2.0f * phi_Center) / (dx * dx);
			float Dyy = (phi_Up + phi_Down - 2.0f * phi_Center) / (dy * dy);
			float Dzz = (phi_In + phi_Out + 2.0f * phi_Center) / (dz * dz);
			float Dxy = (phi_RightDown - phi_RightUp - phi_LeftDown + phi_LeftUp) / (4*dx*dy);
			float Dxz = (phi_RightIn - phi_RightOut - phi_LeftIn + phi_LeftOut) / (4*dx*dz);
			float Dyz = (phi_DownIn - phi_DownOut - phi_UpIn + phi_UpOut) / (4*dy*dz);
			float K  = (  Dx_0*Dx_0*Dyy - 2.0f*Dx_0*Dy_0*Dxy + Dy_0*Dy_0*Dxx + Dx_0*Dx_0*Dzz - 2.0f*Dx_0*Dz_0*Dxz + Dz_0*Dz_0*Dxx + Dy_0*Dy_0*Dzz - 2.0f*Dy_0*Dz_0*Dyz + Dz_0*Dz_0*Dyy ) / ( pow(Dx_0*Dx_0 + Dy_0*Dy_0 + Dz_0*Dz_0, 1.5f) + 0.00005f );
			float Grad = sqrt(Dx_0*Dx_0 + Dy_0*Dy_0 + Dz_0*Dz_0);
			result = Grad * (mu * K + lambda1 * (u0 - c1) * (u0 - c1) - lambda2 * (u0 - c2) * (u0 - c2));
			#undef SPMAddressOf

			WriteSPMFlt(2, addr_result, 0, result);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "segmentation1Mega";}
		inline virtual int ArgumentCount(){return 3;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 11);
			width = ConvertTypes<uint64_t, int>(regs[0]);
			height = ConvertTypes<uint64_t, int>(regs[1]);
			depth = ConvertTypes<uint64_t, int>(regs[2]);
			dx = ConvertTypes<uint64_t, float>(regs[3]);
			dy = ConvertTypes<uint64_t, float>(regs[4]);
			dz = ConvertTypes<uint64_t, float>(regs[5]);
			lambda1 = ConvertTypes<uint64_t, float>(regs[6]);
			lambda2 = ConvertTypes<uint64_t, float>(regs[7]);
			mu = ConvertTypes<uint64_t, float>(regs[8]);
			c1 = ConvertTypes<uint64_t, float>(regs[9]);
			c2 = ConvertTypes<uint64_t, float>(regs[10]);
		}
		inline static int GetOpCode(){return 301;}
	};
}

#endif
