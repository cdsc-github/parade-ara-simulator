#ifndef LCACC_MODE_SWAPTIONS3_H
#define LCACC_MODE_SWAPTIONS3_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_swaptions3 : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_swaptions3(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_ppdFactors = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_pdZ = argAddrVec[1];

			if(argActive[0])
			{
				for(size_t i = 0; i < 33 * GetArgumentWidth(0); i += GetArgumentWidth(0))
				{
					outputArgs.push_back(addr_ppdFactors + i);
				}
			}
			if(argActive[1])
			{
				for(size_t i = 0; i < 33 * GetArgumentWidth(1); i += GetArgumentWidth(1))
				{
					outputArgs.push_back(addr_pdZ + i);
				}
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(2 < argAddrVec.size());
			uint64_t addr_shockVals = argAddrVec[2];

			if(argActive[2])
			{
				for(size_t i = 0; i < 55 * GetArgumentWidth(2); i += GetArgumentWidth(2))
				{
					outputArgs.push_back(addr_shockVals + i);
				}
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 3);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_ppdFactors = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_pdZ = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_shockVals = LCACC_INTERNAL_argAddrVec[2];

			double ppdFactors[3][11];
			double pdZ[3][11];
			double shockVals[55];
			for(int i = 0; i < 33; i++)
			{
				ppdFactors[(i) % (3)][((i) / ((1 * 3 * 11))) % (11)] = (double)0;
			}
			for(int i = 0; i < 33; i++)
			{
				pdZ[(i) % (3)][((i) / ((1 * 3 * 11))) % (11)] = (double)0;
			}
			for(int i = 0; i < 55; i++)
			{
				shockVals[(i) % (55)] = (double)0;
			}

			for(size_t i = 0; i < 33; i++)
			{
				ppdFactors[(i) % (3)][((i) / ((1 * 3 * 11))) % (11)] = ReadSPMFlt(0, addr_ppdFactors, i);
			}
			for(size_t i = 0; i < 33; i++)
			{
				pdZ[(i) % (3)][((i) / ((1 * 3 * 11))) % (11)] = ReadSPMFlt(1, addr_pdZ, i);
			}

			#define SPMAddressOf(x) (addr_##x)
			#define TotalShock(l, j) (ppdFactors[0][l] * pdZ[0][j] + ppdFactors[1][l] * pdZ[1][j] + ppdFactors[2][l] * pdZ[2][j])

			                shockVals[0] = TotalShock(0, 1);
			                shockVals[1] = TotalShock(1, 1);
			                shockVals[2] = TotalShock(2, 1);
			                shockVals[3] = TotalShock(3, 1);
			                shockVals[4] = TotalShock(4, 1);
			                shockVals[5] = TotalShock(5, 1);
			                shockVals[6] = TotalShock(6, 1);
			                shockVals[7] = TotalShock(7, 1);
			                shockVals[8] = TotalShock(8, 1);
			                shockVals[9] = TotalShock(9, 1);

			                shockVals[10] = TotalShock(0, 2);
			                shockVals[11] = TotalShock(1, 2);
			                shockVals[12] = TotalShock(2, 2);
			                shockVals[13] = TotalShock(3, 2);
			                shockVals[14] = TotalShock(4, 2);
			                shockVals[15] = TotalShock(5, 2);
			                shockVals[16] = TotalShock(6, 2);
			                shockVals[17] = TotalShock(7, 2);
			                shockVals[18] = TotalShock(8, 2);

			                shockVals[19] = TotalShock(0, 3);
			                shockVals[20] = TotalShock(1, 3);
			                shockVals[21] = TotalShock(2, 3);
			                shockVals[22] = TotalShock(3, 3);
			                shockVals[23] = TotalShock(4, 3);
			                shockVals[24] = TotalShock(5, 3);
			                shockVals[25] = TotalShock(6, 3);
			                shockVals[26] = TotalShock(7, 3);

			                shockVals[27] = TotalShock(0, 4);
			                shockVals[28] = TotalShock(1, 4);
			                shockVals[29] = TotalShock(2, 4);
			                shockVals[30] = TotalShock(3, 4);
			                shockVals[31] = TotalShock(4, 4);
			                shockVals[32] = TotalShock(5, 4);
			                shockVals[33] = TotalShock(6, 4);

			                shockVals[34] = TotalShock(0, 5);
			                shockVals[35] = TotalShock(1, 5);
			                shockVals[36] = TotalShock(2, 5);
			                shockVals[37] = TotalShock(3, 5);
			                shockVals[38] = TotalShock(4, 5);
			                shockVals[39] = TotalShock(5, 5);

			                shockVals[40] = TotalShock(0, 6);
			                shockVals[41] = TotalShock(1, 6);
			                shockVals[42] = TotalShock(2, 6);
			                shockVals[43] = TotalShock(3, 6);
			                shockVals[44] = TotalShock(4, 6);

			                shockVals[45] = TotalShock(0, 7);
			                shockVals[46] = TotalShock(1, 7);
			                shockVals[47] = TotalShock(2, 7);
			                shockVals[48] = TotalShock(3, 7);

			                shockVals[49] = TotalShock(0, 8);
			                shockVals[50] = TotalShock(1, 8);
			                shockVals[51] = TotalShock(2, 8);

			                shockVals[52] = TotalShock(0, 9);
			                shockVals[53] = TotalShock(1, 9);

			                shockVals[54] = TotalShock(0, 10);

			#undef pdZ
			#undef TotalShock
			#undef SPMAddressOf

			for(size_t i = 0; i < 55; i++)
			{
				WriteSPMFlt(2, addr_shockVals, i, shockVals[(i) % (55)]);
			}
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 2;}
		inline virtual int PipelineDepth(){return 60;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "swaptions3";}
		inline virtual int ArgumentCount(){return 3;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 1303;}
	};
}

#endif
