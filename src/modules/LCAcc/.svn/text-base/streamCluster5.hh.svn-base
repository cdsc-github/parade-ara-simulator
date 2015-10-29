#ifndef LCACC_MODE_STREAMCLUSTER5_H
#define LCACC_MODE_STREAMCLUSTER5_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_streamCluster5 : public LCAccOperatingMode
	{
		uint64_t tableStart;
		uint64_t spmStart1;
		uint64_t lowerStart;
		uint64_t spmStart2;
	public:
		inline OperatingMode_streamCluster5(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_assign = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_table = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_lower = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_switchMembership = argAddrVec[3];
			assert(4 < argAddrVec.size());
			uint64_t addr_inCost = argAddrVec[4];
			assert(5 < argAddrVec.size());
			uint64_t addr_weight = argAddrVec[5];
			assert(6 < argAddrVec.size());
			uint64_t addr_x = argAddrVec[6];
			assert(7 < argAddrVec.size());
			uint64_t addr_y = argAddrVec[7];

			if(argActive[0])
			{
				outputArgs.push_back(addr_assign);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_table);
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_lower);
			}
			if(argActive[3])
			{
				outputArgs.push_back(addr_switchMembership);
			}
			if(argActive[4])
			{
				outputArgs.push_back(addr_inCost);
			}
			if(argActive[5])
			{
				outputArgs.push_back(addr_weight);
			}
			if(argActive[6])
			{
				for(size_t i = 0; i < 32 * GetArgumentWidth(6); i += GetArgumentWidth(6))
				{
					outputArgs.push_back(addr_x + i);
				}
			}
			if(argActive[7])
			{
				for(size_t i = 0; i < 32 * GetArgumentWidth(7); i += GetArgumentWidth(7))
				{
					outputArgs.push_back(addr_y + i);
				}
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(8 < argAddrVec.size());
			uint64_t addr_outCost = argAddrVec[8];

			if(argActive[8])
			{
				outputArgs.push_back(addr_outCost);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 9);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_assign = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_table = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_lower = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_switchMembership = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_inCost = LCACC_INTERNAL_argAddrVec[4];
			assert(5 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_weight = LCACC_INTERNAL_argAddrVec[5];
			assert(6 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_x = LCACC_INTERNAL_argAddrVec[6];
			assert(7 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_y = LCACC_INTERNAL_argAddrVec[7];
			assert(8 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_outCost = LCACC_INTERNAL_argAddrVec[8];

			uint64_t assign;
			uint64_t table;
			double lower;
			uint64_t switchMembership;
			double inCost;
			double weight;
			double x[32];
			double y[32];
			double outCost;
			assign = 0;
			table = 0;
			lower = 0;
			switchMembership = 0;
			inCost = 0;
			weight = 0;
			for(int i = 0; i < 32; i++)
			{
				x[(i) % (32)] = (double)0;
			}
			for(int i = 0; i < 32; i++)
			{
				y[(i) % (32)] = (double)0;
			}
			outCost = 0;

			assign = ReadSPMInt(0, addr_assign, 0, true);
			table = ReadSPMInt(1, addr_table, 0, true);
			lower = ReadSPMFlt(2, addr_lower, 0);
			switchMembership = ReadSPMInt(3, addr_switchMembership, 0, true);
			inCost = ReadSPMFlt(4, addr_inCost, 0);
			weight = ReadSPMFlt(5, addr_weight, 0);
			for(size_t i = 0; i < 32; i++)
			{
				x[(i) % (32)] = ReadSPMFlt(6, addr_x, i);
			}
			for(size_t i = 0; i < 32; i++)
			{
				y[(i) % (32)] = ReadSPMFlt(7, addr_y, i);
			}

			#define SPMAddressOf(x) (addr_##x)
			AddRead(tableStart + assign * sizeof(uint32_t), spmStart1 + iteration * sizeof(uint32_t), sizeof(uint32_t));
			#undef SPMAddressOf

			WriteSPMFlt(8, addr_outCost, 0, outCost);
		}
		inline virtual void MemoryAccessComplete(int iteration, int maxIteration, int taskID, uint64_t spmAddr, uint64_t memAddr, int accessType, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 9);
			uint64_t addr_assign = LCACC_INTERNAL_argAddrVec[0];
			uint64_t addr_table = LCACC_INTERNAL_argAddrVec[1];
			uint64_t addr_lower = LCACC_INTERNAL_argAddrVec[2];
			uint64_t addr_switchMembership = LCACC_INTERNAL_argAddrVec[3];
			uint64_t addr_inCost = LCACC_INTERNAL_argAddrVec[4];
			uint64_t addr_weight = LCACC_INTERNAL_argAddrVec[5];
			uint64_t addr_x = LCACC_INTERNAL_argAddrVec[6];
			uint64_t addr_y = LCACC_INTERNAL_argAddrVec[7];
			uint64_t addr_outCost = LCACC_INTERNAL_argAddrVec[8];

			uint64_t assign;
			uint64_t table;
			double lower;
			uint64_t switchMembership;
			double inCost;
			double weight;
			double x[32];
			double y[32];
			double outCost;

			assign = ReadSPMInt(0, addr_assign, 0, true);
			table = ReadSPMInt(1, addr_table, 0, true);
			lower = ReadSPMFlt(2, addr_lower, 0);
			switchMembership = ReadSPMInt(3, addr_switchMembership, 0, true);
			inCost = ReadSPMFlt(4, addr_inCost, 0);
			weight = ReadSPMFlt(5, addr_weight, 0);
			for(size_t i = 0; i < 32; i++)
			{
				x[(i) % (32)] = ReadSPMFlt(6, addr_x, i);
			}
			for(size_t i = 0; i < 32; i++)
			{
				y[(i) % (32)] = ReadSPMFlt(7, addr_y, i);
			}

			if(spmAddr >= spmStart1 && spmAddr < spmStart1 + sizeof(uint32_t) * maxIteration)
			{
				AddRead(lowerStart + table * sizeof(uint32_t), spmStart2 + iteration * sizeof(uint32_t), sizeof(uint32_t));
			}
			else
			{
				if(switchMembership || lower > 0)
				{
					outCost = 0;
					for(int i = 0; i < 32; i++)
					{
						outCost += (x[i] - y[i]) * (x[i] - y[i]);
					}
					outCost *= weight;
				}
				else
				{
					outCost = inCost;
				}
			}

			WriteSPMFlt(8, addr_outCost, 0, outCost);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "streamCluster5";}
		inline virtual int ArgumentCount(){return 9;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 4);
			tableStart = ConvertTypes<uint64_t, uint64_t>(regs[0]);
			spmStart1 = ConvertTypes<uint64_t, uint64_t>(regs[1]);
			lowerStart = ConvertTypes<uint64_t, uint64_t>(regs[2]);
			spmStart2 = ConvertTypes<uint64_t, uint64_t>(regs[3]);
		}
		inline static int GetOpCode(){return 1205;}
	};
}

#endif
