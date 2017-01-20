#ifndef LCACC_MODE_ROBLOC_H
#define LCACC_MODE_ROBLOC_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_RobLoc : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_RobLoc(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_in_ran1 = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_in_ran2 = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_in_quat = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_in_accl = argAddrVec[3];
			assert(4 < argAddrVec.size());
			uint64_t addr_in_pos = argAddrVec[4];
			assert(5 < argAddrVec.size());
			uint64_t addr_in_vel = argAddrVec[5];

			if(argActive[0])
			{
				for(size_t i = 0; i < 3 * GetArgumentWidth(0); i += GetArgumentWidth(0))
				{
					outputArgs.push_back(addr_in_ran1 + i);
				}
			}
			if(argActive[1])
			{
				for(size_t i = 0; i < 3 * GetArgumentWidth(1); i += GetArgumentWidth(1))
				{
					outputArgs.push_back(addr_in_ran2 + i);
				}
			}
			if(argActive[2])
			{
				for(size_t i = 0; i < 4 * GetArgumentWidth(2); i += GetArgumentWidth(2))
				{
					outputArgs.push_back(addr_in_quat + i);
				}
			}
			if(argActive[3])
			{
				for(size_t i = 0; i < 3 * GetArgumentWidth(3); i += GetArgumentWidth(3))
				{
					outputArgs.push_back(addr_in_accl + i);
				}
			}
			if(argActive[4])
			{
				for(size_t i = 0; i < 3 * GetArgumentWidth(4); i += GetArgumentWidth(4))
				{
					outputArgs.push_back(addr_in_pos + i);
				}
			}
			if(argActive[5])
			{
				for(size_t i = 0; i < 3 * GetArgumentWidth(5); i += GetArgumentWidth(5))
				{
					outputArgs.push_back(addr_in_vel + i);
				}
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(6 < argAddrVec.size());
			uint64_t addr_out_pos = argAddrVec[6];
			assert(7 < argAddrVec.size());
			uint64_t addr_out_vel = argAddrVec[7];

			if(argActive[6])
			{
				for(size_t i = 0; i < 3 * GetArgumentWidth(6); i += GetArgumentWidth(6))
				{
					outputArgs.push_back(addr_out_pos + i);
				}
			}
			if(argActive[7])
			{
				for(size_t i = 0; i < 3 * GetArgumentWidth(7); i += GetArgumentWidth(7))
				{
					outputArgs.push_back(addr_out_vel + i);
				}
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 8);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_ran1 = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_ran2 = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_quat = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_accl = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_pos = LCACC_INTERNAL_argAddrVec[4];
			assert(5 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_vel = LCACC_INTERNAL_argAddrVec[5];
			assert(6 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_out_pos = LCACC_INTERNAL_argAddrVec[6];
			assert(7 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_out_vel = LCACC_INTERNAL_argAddrVec[7];

			double in_ran1[3];
			double in_ran2[3];
			double in_quat[4];
			double in_accl[3];
			double in_pos[3];
			double in_vel[3];
			double out_pos[3];
			double out_vel[3];
			for(int i = 0; i < 3; i++)
			{
				in_ran1[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < 3; i++)
			{
				in_ran2[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < 4; i++)
			{
				in_quat[(i) % (4)] = (double)0;
			}
			for(int i = 0; i < 3; i++)
			{
				in_accl[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < 3; i++)
			{
				in_pos[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < 3; i++)
			{
				in_vel[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < 3; i++)
			{
				out_pos[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < 3; i++)
			{
				out_vel[(i) % (3)] = (double)0;
			}

			for(size_t i = 0; i < 3; i++)
			{
				in_ran1[(i) % (3)] = ReadSPMFlt(0, addr_in_ran1, i);
			}
			for(size_t i = 0; i < 3; i++)
			{
				in_ran2[(i) % (3)] = ReadSPMFlt(1, addr_in_ran2, i);
			}
			for(size_t i = 0; i < 4; i++)
			{
				in_quat[(i) % (4)] = ReadSPMFlt(2, addr_in_quat, i);
			}
			for(size_t i = 0; i < 3; i++)
			{
				in_accl[(i) % (3)] = ReadSPMFlt(3, addr_in_accl, i);
			}
			for(size_t i = 0; i < 3; i++)
			{
				in_pos[(i) % (3)] = ReadSPMFlt(4, addr_in_pos, i);
			}
			for(size_t i = 0; i < 3; i++)
			{
				in_vel[(i) % (3)] = ReadSPMFlt(5, addr_in_vel, i);
			}

			#define SPMAddressOf(x) (addr_##x)
			// Observation

				float q0 = in_quat[0];
				float q1 = in_quat[1];
				float q2 = in_quat[2];
				float q3 = in_quat[3];

				float g0 = q2 * 9.8f;
				float g1 = q3 * 9.8f;
				float g2 = q0 * (-9.8f);
				float g3 = q1 * (-9.8f);

				float gravity0 = g1*q0 - g0*q1 - g2*q3 + g3*q2;
				float gravity1 = g1*q3 - g0*q2 + g2*q0 - g3*q1;
				float gravity2 = g2*q1 - g0*q3 - g1*q2 + g3*q0;

				// Motion Model

				float accl0 = in_accl[0] - gravity0;
				float accl1 = in_accl[1] - gravity1;
				float accl2 = in_accl[2] - gravity2;

				float a0 = q0*in_vel[0] + q1*in_vel[1] + q2*in_vel[2];
				float a1 = q0*in_vel[1] - q1*in_vel[0] + q3*in_vel[2];
				float a2 = q0*in_vel[2] - q2*in_vel[0] - q3*in_vel[1];
				float a3 = q2*in_vel[1] - q1*in_vel[2] - q3*in_vel[0];

				float c0 = q0*accl0 + q1*accl1 + q2*accl2;
				float c1 = q0*accl1 - q1*accl0 + q3*accl2;
				float c2 = q0*accl2 - q2*accl0 - q3*accl1;
				float c3 = q2*accl1 - q1*accl2 - q3*accl0;

				out_pos[0] = in_pos[0] + 0.01f * (a0*q1 + a1*q0 + a2*q3 - a3*q2) + in_ran1[0] + 0.00005f * (c0*q1 + c1*q0 + c2*q3 - c3*q2);
				out_pos[1] = in_pos[1] + 0.01f * (a0*q2 - a1*q3 + a2*q0 + a3*q1) + in_ran1[1] + 0.00005f * (c0*q2 - c1*q3 + c2*q0 + c3*q1);
				out_pos[2] = in_pos[2] + 0.01f * (a0*q3 + a1*q2 - a2*q1 + a3*q0) + in_ran1[2] + 0.00005f * (c0*q3 + c1*q2 - c2*q1 + c3*q0);

				out_vel[0] = in_vel[0] + accl0 * 0.01f + in_ran2[0];
				out_vel[1] = in_vel[1] + accl1 * 0.01f + in_ran2[1];
				out_vel[2] = in_vel[2] + accl2 * 0.01f + in_ran2[2];
			#undef SPMAddressOf

			for(size_t i = 0; i < 3; i++)
			{
				WriteSPMFlt(6, addr_out_pos, i, out_pos[(i) % (3)]);
			}
			for(size_t i = 0; i < 3; i++)
			{
				WriteSPMFlt(7, addr_out_vel, i, out_vel[(i) % (3)]);
			}
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 129;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "RobLoc";}
		inline virtual int ArgumentCount(){return 8;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 820;}
	};
}

#endif
