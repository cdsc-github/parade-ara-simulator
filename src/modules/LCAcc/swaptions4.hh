#ifndef LCACC_MODE_SWAPTIONS4_H
#define LCACC_MODE_SWAPTIONS4_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_swaptions4 : public LCAccOperatingMode
	{
		float ddelt;
		float sqrt_ddelt;
	public:
		inline OperatingMode_swaptions4(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_pdTotalDrift = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_shock_arr = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_in_ppdHJMPath = argAddrVec[2];

			if(argActive[0])
			{
				for(size_t i = 0; i < 11 * GetArgumentWidth(0); i += GetArgumentWidth(0))
				{
					outputArgs.push_back(addr_pdTotalDrift + i);
				}
			}
			if(argActive[1])
			{
				for(size_t i = 0; i < 55 * GetArgumentWidth(1); i += GetArgumentWidth(1))
				{
					outputArgs.push_back(addr_shock_arr + i);
				}
			}
			if(argActive[2])
			{
				for(size_t i = 0; i < 121 * GetArgumentWidth(2); i += GetArgumentWidth(2))
				{
					outputArgs.push_back(addr_in_ppdHJMPath + i);
				}
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(3 < argAddrVec.size());
			uint64_t addr_out_ppdHJMPath = argAddrVec[3];

			if(argActive[3])
			{
				for(size_t i = 0; i < 121 * GetArgumentWidth(3); i += GetArgumentWidth(3))
				{
					outputArgs.push_back(addr_out_ppdHJMPath + i);
				}
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 4);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_pdTotalDrift = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_shock_arr = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_in_ppdHJMPath = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_out_ppdHJMPath = LCACC_INTERNAL_argAddrVec[3];

			double pdTotalDrift[11];
			double shock_arr[55];
			double in_ppdHJMPath[11][11];
			double out_ppdHJMPath[11][11];
			for(int i = 0; i < 11; i++)
			{
				pdTotalDrift[(i) % (11)] = (double)0;
			}
			for(int i = 0; i < 55; i++)
			{
				shock_arr[(i) % (55)] = (double)0;
			}
			for(int i = 0; i < 121; i++)
			{
				in_ppdHJMPath[(i) % (11)][((i) / ((1 * 11 * 11))) % (11)] = (double)0;
			}
			for(int i = 0; i < 121; i++)
			{
				out_ppdHJMPath[(i) % (11)][((i) / ((1 * 11 * 11))) % (11)] = (double)0;
			}

			for(size_t i = 0; i < 11; i++)
			{
				pdTotalDrift[(i) % (11)] = ReadSPMFlt(0, addr_pdTotalDrift, i);
			}
			for(size_t i = 0; i < 55; i++)
			{
				shock_arr[(i) % (55)] = ReadSPMFlt(1, addr_shock_arr, i);
			}
			for(size_t i = 0; i < 121; i++)
			{
				in_ppdHJMPath[(i) % (11)][((i) / ((1 * 11 * 11))) % (11)] = ReadSPMFlt(2, addr_in_ppdHJMPath, i);
			}

			#define SPMAddressOf(x) (addr_##x)
			float ppd_path_val_1_0 = out_ppdHJMPath[1][0] = in_ppdHJMPath[0][1] + pdTotalDrift[0] * ddelt + sqrt_ddelt * shock_arr[0];
			        float ppd_path_val_1_1 = out_ppdHJMPath[1][1] = in_ppdHJMPath[0][2] + pdTotalDrift[1] * ddelt + sqrt_ddelt * shock_arr[1];
			        float ppd_path_val_1_2 = out_ppdHJMPath[1][2] = in_ppdHJMPath[0][3] + pdTotalDrift[2] * ddelt + sqrt_ddelt * shock_arr[2];
			        float ppd_path_val_1_3 = out_ppdHJMPath[1][3] = in_ppdHJMPath[0][4] + pdTotalDrift[3] * ddelt + sqrt_ddelt * shock_arr[3];
			        float ppd_path_val_1_4 = out_ppdHJMPath[1][4] = in_ppdHJMPath[0][5] + pdTotalDrift[4] * ddelt + sqrt_ddelt * shock_arr[4];
			        float ppd_path_val_1_5 = out_ppdHJMPath[1][5] = in_ppdHJMPath[0][6] + pdTotalDrift[5] * ddelt + sqrt_ddelt * shock_arr[5];
			        float ppd_path_val_1_6 = out_ppdHJMPath[1][6] = in_ppdHJMPath[0][7] + pdTotalDrift[6] * ddelt + sqrt_ddelt * shock_arr[6];
			        float ppd_path_val_1_7 = out_ppdHJMPath[1][7] = in_ppdHJMPath[0][8] + pdTotalDrift[7] * ddelt + sqrt_ddelt * shock_arr[7];
			        float ppd_path_val_1_8 = out_ppdHJMPath[1][8] = in_ppdHJMPath[0][9] + pdTotalDrift[8] * ddelt + sqrt_ddelt * shock_arr[8];
			        float ppd_path_val_1_9 = out_ppdHJMPath[1][9] = in_ppdHJMPath[0][10] + pdTotalDrift[9] * ddelt + sqrt_ddelt * shock_arr[9];

			        float ppd_path_val_2_0 = out_ppdHJMPath[2][0] = ppd_path_val_1_1 + pdTotalDrift[0] * ddelt + sqrt_ddelt * shock_arr[10];
			        float ppd_path_val_2_1 = out_ppdHJMPath[2][1] = ppd_path_val_1_2 + pdTotalDrift[1] * ddelt + sqrt_ddelt * shock_arr[11];
			        float ppd_path_val_2_2 = out_ppdHJMPath[2][2] = ppd_path_val_1_3 + pdTotalDrift[2] * ddelt + sqrt_ddelt * shock_arr[12];
			        float ppd_path_val_2_3 = out_ppdHJMPath[2][3] = ppd_path_val_1_4 + pdTotalDrift[3] * ddelt + sqrt_ddelt * shock_arr[13];
			        float ppd_path_val_2_4 = out_ppdHJMPath[2][4] = ppd_path_val_1_5 + pdTotalDrift[4] * ddelt + sqrt_ddelt * shock_arr[14];
			        float ppd_path_val_2_5 = out_ppdHJMPath[2][5] = ppd_path_val_1_6 + pdTotalDrift[5] * ddelt + sqrt_ddelt * shock_arr[15];
			        float ppd_path_val_2_6 = out_ppdHJMPath[2][6] = ppd_path_val_1_7 + pdTotalDrift[6] * ddelt + sqrt_ddelt * shock_arr[16];
			        float ppd_path_val_2_7 = out_ppdHJMPath[2][7] = ppd_path_val_1_8 + pdTotalDrift[7] * ddelt + sqrt_ddelt * shock_arr[17];
			        float ppd_path_val_2_8 = out_ppdHJMPath[2][8] = ppd_path_val_1_9 + pdTotalDrift[8] * ddelt + sqrt_ddelt * shock_arr[18];

			        float ppd_path_val_3_0 = out_ppdHJMPath[3][0] = ppd_path_val_2_1 + pdTotalDrift[0] * ddelt + sqrt_ddelt * shock_arr[19];
			        float ppd_path_val_3_1 = out_ppdHJMPath[3][1] = ppd_path_val_2_2 + pdTotalDrift[1] * ddelt + sqrt_ddelt * shock_arr[20];
			        float ppd_path_val_3_2 = out_ppdHJMPath[3][2] = ppd_path_val_2_3 + pdTotalDrift[2] * ddelt + sqrt_ddelt * shock_arr[21];
			        float ppd_path_val_3_3 = out_ppdHJMPath[3][3] = ppd_path_val_2_4 + pdTotalDrift[3] * ddelt + sqrt_ddelt * shock_arr[22];
			        float ppd_path_val_3_4 = out_ppdHJMPath[3][4] = ppd_path_val_2_5 + pdTotalDrift[4] * ddelt + sqrt_ddelt * shock_arr[23];
			        float ppd_path_val_3_5 = out_ppdHJMPath[3][5] = ppd_path_val_2_6 + pdTotalDrift[5] * ddelt + sqrt_ddelt * shock_arr[24];
			        float ppd_path_val_3_6 = out_ppdHJMPath[3][6] = ppd_path_val_2_7 + pdTotalDrift[6] * ddelt + sqrt_ddelt * shock_arr[25];
			        float ppd_path_val_3_7 = out_ppdHJMPath[3][7] = ppd_path_val_2_8 + pdTotalDrift[7] * ddelt + sqrt_ddelt * shock_arr[26];

			        float ppd_path_val_4_0 = out_ppdHJMPath[4][0] = ppd_path_val_3_1 + pdTotalDrift[0] * ddelt + sqrt_ddelt * shock_arr[27];
			        float ppd_path_val_4_1 = out_ppdHJMPath[4][1] = ppd_path_val_3_2 + pdTotalDrift[1] * ddelt + sqrt_ddelt * shock_arr[28];
			        float ppd_path_val_4_2 = out_ppdHJMPath[4][2] = ppd_path_val_3_3 + pdTotalDrift[2] * ddelt + sqrt_ddelt * shock_arr[29];
			        float ppd_path_val_4_3 = out_ppdHJMPath[4][3] = ppd_path_val_3_4 + pdTotalDrift[3] * ddelt + sqrt_ddelt * shock_arr[30];
			        float ppd_path_val_4_4 = out_ppdHJMPath[4][4] = ppd_path_val_3_5 + pdTotalDrift[4] * ddelt + sqrt_ddelt * shock_arr[31];
			        float ppd_path_val_4_5 = out_ppdHJMPath[4][5] = ppd_path_val_3_6 + pdTotalDrift[5] * ddelt + sqrt_ddelt * shock_arr[32];
			        float ppd_path_val_4_6 = out_ppdHJMPath[4][6] = ppd_path_val_3_7 + pdTotalDrift[6] * ddelt + sqrt_ddelt * shock_arr[33];

			        float ppd_path_val_5_0 = out_ppdHJMPath[5][0] = ppd_path_val_4_1 + pdTotalDrift[0] * ddelt + sqrt_ddelt * shock_arr[34];
			        float ppd_path_val_5_1 = out_ppdHJMPath[5][1] = ppd_path_val_4_2 + pdTotalDrift[1] * ddelt + sqrt_ddelt * shock_arr[35];
			        float ppd_path_val_5_2 = out_ppdHJMPath[5][2] = ppd_path_val_4_3 + pdTotalDrift[2] * ddelt + sqrt_ddelt * shock_arr[36];
			        float ppd_path_val_5_3 = out_ppdHJMPath[5][3] = ppd_path_val_4_4 + pdTotalDrift[3] * ddelt + sqrt_ddelt * shock_arr[37];
			        float ppd_path_val_5_4 = out_ppdHJMPath[5][4] = ppd_path_val_4_5 + pdTotalDrift[4] * ddelt + sqrt_ddelt * shock_arr[38];
			        float ppd_path_val_5_5 = out_ppdHJMPath[5][5] = ppd_path_val_4_6 + pdTotalDrift[5] * ddelt + sqrt_ddelt * shock_arr[39];

			        float ppd_path_val_6_0 = out_ppdHJMPath[6][0] = ppd_path_val_5_1 + pdTotalDrift[0] * ddelt + sqrt_ddelt * shock_arr[40];
			        float ppd_path_val_6_1 = out_ppdHJMPath[6][1] = ppd_path_val_5_2 + pdTotalDrift[1] * ddelt + sqrt_ddelt * shock_arr[41];
			        float ppd_path_val_6_2 = out_ppdHJMPath[6][2] = ppd_path_val_5_3 + pdTotalDrift[2] * ddelt + sqrt_ddelt * shock_arr[42];
			        float ppd_path_val_6_3 = out_ppdHJMPath[6][3] = ppd_path_val_5_4 + pdTotalDrift[3] * ddelt + sqrt_ddelt * shock_arr[43];
			        float ppd_path_val_6_4 = out_ppdHJMPath[6][4] = ppd_path_val_5_5 + pdTotalDrift[4] * ddelt + sqrt_ddelt * shock_arr[44];

			        float ppd_path_val_7_0 = out_ppdHJMPath[7][0] = ppd_path_val_6_1 + pdTotalDrift[0] * ddelt + sqrt_ddelt * shock_arr[45];
			        float ppd_path_val_7_1 = out_ppdHJMPath[7][1] = ppd_path_val_6_2 + pdTotalDrift[1] * ddelt + sqrt_ddelt * shock_arr[46];
			        float ppd_path_val_7_2 = out_ppdHJMPath[7][2] = ppd_path_val_6_3 + pdTotalDrift[2] * ddelt + sqrt_ddelt * shock_arr[47];
			        float ppd_path_val_7_3 = out_ppdHJMPath[7][3] = ppd_path_val_6_4 + pdTotalDrift[3] * ddelt + sqrt_ddelt * shock_arr[48];

			        float ppd_path_val_8_0 = out_ppdHJMPath[8][0] = ppd_path_val_7_1 + pdTotalDrift[0] * ddelt + sqrt_ddelt * shock_arr[49];
			        float ppd_path_val_8_1 = out_ppdHJMPath[8][1] = ppd_path_val_7_2 + pdTotalDrift[1] * ddelt + sqrt_ddelt * shock_arr[50];
			        float ppd_path_val_8_2 = out_ppdHJMPath[8][2] = ppd_path_val_7_3 + pdTotalDrift[2] * ddelt + sqrt_ddelt * shock_arr[51];

			        float ppd_path_val_9_0 = out_ppdHJMPath[9][0] = ppd_path_val_8_1 + pdTotalDrift[0] * ddelt + sqrt_ddelt * shock_arr[52];
			        float ppd_path_val_9_1 = out_ppdHJMPath[9][1] = ppd_path_val_8_2 + pdTotalDrift[1] * ddelt + sqrt_ddelt * shock_arr[53];

			        float ppd_path_val_10_0 = out_ppdHJMPath[10][0] = ppd_path_val_9_1 + pdTotalDrift[0] * ddelt + sqrt_ddelt * shock_arr[54];

        			out_ppdHJMPath[1][0] = ppd_path_val_1_0;
        			out_ppdHJMPath[2][0] = ppd_path_val_2_0;
        			out_ppdHJMPath[3][0] = ppd_path_val_3_0;
        			out_ppdHJMPath[4][0] = ppd_path_val_4_0;
        			out_ppdHJMPath[5][0] = ppd_path_val_5_0;
        			out_ppdHJMPath[6][0] = ppd_path_val_6_0;
        			out_ppdHJMPath[7][0] = ppd_path_val_7_0;
        			out_ppdHJMPath[8][0] = ppd_path_val_8_0;
        			out_ppdHJMPath[9][0] = ppd_path_val_9_0;
        			out_ppdHJMPath[10][0] = ppd_path_val_10_0;
			#undef SPMAddressOf

			for(size_t i = 0; i < 121; i++)
			{
				WriteSPMFlt(3, addr_out_ppdHJMPath, i, out_ppdHJMPath[(i) % (11)][((i) / ((1 * 11 * 11))) % (11)]);
			}
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 2;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "swaptions4";}
		inline virtual int ArgumentCount(){return 4;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 2);
			ddelt = ConvertTypes<uint64_t, float>(regs[0]);
			sqrt_ddelt = ConvertTypes<uint64_t, float>(regs[1]);
		}
		inline static int GetOpCode(){return 1304;}
	};
}

#endif
