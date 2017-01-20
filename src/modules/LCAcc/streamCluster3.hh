#ifndef LCACC_MODE_STREAMCLUSTER3_H
#define LCACC_MODE_STREAMCLUSTER3_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_streamCluster3 : public LCAccOperatingMode
	{
		uint64_t tableAddr;
		uint32_t tableStride;
		uint64_t spmAddr;
		uint64_t lowerAddr;
		uint32_t lowerStride;
		uint64_t modificationAddr;
	public:
		inline OperatingMode_streamCluster3(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_src = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_dst = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_weight = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_cost = argAddrVec[3];
			assert(4 < argAddrVec.size());
			uint64_t addr_readIndex = argAddrVec[4];
			assert(5 < argAddrVec.size());
			uint64_t addr_inSwitchMembership = argAddrVec[5];

			if(argActive[0])
			{
				for(size_t i = 0; i < 32 * GetArgumentWidth(0); i += GetArgumentWidth(0))
				{
					outputArgs.push_back(addr_src + i);
				}
			}
			if(argActive[1])
			{
				for(size_t i = 0; i < 32 * GetArgumentWidth(1); i += GetArgumentWidth(1))
				{
					outputArgs.push_back(addr_dst + i);
				}
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_weight);
			}
			if(argActive[3])
			{
				outputArgs.push_back(addr_cost);
			}
			if(argActive[4])
			{
				outputArgs.push_back(addr_readIndex);
			}
			if(argActive[5])
			{
				outputArgs.push_back(addr_inSwitchMembership);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(6 < argAddrVec.size());
			uint64_t addr_predicateResolution = argAddrVec[6];
			assert(7 < argAddrVec.size());
			uint64_t addr_outSwitchMembership = argAddrVec[7];
			assert(8 < argAddrVec.size());
			uint64_t addr_modification = argAddrVec[8];

			if(argActive[6])
			{
				outputArgs.push_back(addr_predicateResolution);
			}
			if(argActive[7])
			{
				outputArgs.push_back(addr_outSwitchMembership);
			}
			if(argActive[8])
			{
				outputArgs.push_back(addr_modification);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 9);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_src = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_dst = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_weight = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_cost = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_readIndex = LCACC_INTERNAL_argAddrVec[4];
			assert(5 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_inSwitchMembership = LCACC_INTERNAL_argAddrVec[5];
			assert(6 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_predicateResolution = LCACC_INTERNAL_argAddrVec[6];
			assert(7 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_outSwitchMembership = LCACC_INTERNAL_argAddrVec[7];
			assert(8 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_modification = LCACC_INTERNAL_argAddrVec[8];

			double src[32];
			double dst[32];
			double weight;
			double cost;
			uint64_t readIndex;
			uint64_t inSwitchMembership;
			uint64_t predicateResolution;
			uint64_t outSwitchMembership;
			double modification;
			for(int i = 0; i < 32; i++)
			{
				src[(i) % (32)] = (double)0;
			}
			for(int i = 0; i < 32; i++)
			{
				dst[(i) % (32)] = (double)0;
			}
			weight = 0;
			cost = 0;
			readIndex = 0;
			inSwitchMembership = 0;
			predicateResolution = 0;
			outSwitchMembership = 0;
			modification = 0;

			for(size_t i = 0; i < 32; i++)
			{
				src[(i) % (32)] = ReadSPMFlt(0, addr_src, i);
			}
			for(size_t i = 0; i < 32; i++)
			{
				dst[(i) % (32)] = ReadSPMFlt(1, addr_dst, i);
			}
			weight = ReadSPMFlt(2, addr_weight, 0);
			cost = ReadSPMFlt(3, addr_cost, 0);
			readIndex = ReadSPMInt(4, addr_readIndex, 0, true);
			inSwitchMembership = ReadSPMInt(5, addr_inSwitchMembership, 0, true);

			#define SPMAddressOf(x) (addr_##x)
			float x_cost= 0;
			for(int i = 0; i < 32; i++)
			{
				x_cost += (src[i] - dst[i]) * (src[i] - dst[i]) * weight;
			}
			float current_cost = cost;

			predicateResolution = (x_cost < current_cost) ? 1 : 0;

			if(predicateResolution)
			{
				outSwitchMembership = 1;
			}
			else
			{
				outSwitchMembership = inSwitchMembership;
				AddRead(tableAddr + tableStride * readIndex, spmAddr + iteration * sizeof(float), sizeof(float));
			}
			#undef SPMAddressOf

			WriteSPMInt(6, addr_predicateResolution, 0, true, predicateResolution);
			WriteSPMInt(7, addr_outSwitchMembership, 0, true, outSwitchMembership);
			WriteSPMFlt(8, addr_modification, 0, modification);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "streamCluster3";}
		inline virtual int ArgumentCount(){return 9;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 6);
			tableAddr = ConvertTypes<uint64_t, uint64_t>(regs[0]);
			tableStride = ConvertTypes<uint64_t, uint32_t>(regs[1]);
			spmAddr = ConvertTypes<uint64_t, uint64_t>(regs[2]);
			lowerAddr = ConvertTypes<uint64_t, uint64_t>(regs[3]);
			lowerStride = ConvertTypes<uint64_t, uint32_t>(regs[4]);
			modificationAddr = ConvertTypes<uint64_t, uint64_t>(regs[5]);
		}
		inline static int GetOpCode(){return 1203;}
	};
}

#endif
