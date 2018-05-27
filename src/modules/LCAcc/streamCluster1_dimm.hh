#ifndef LCACC_MODE_STREAMCLUSTER1_DIMM_H
#define LCACC_MODE_STREAMCLUSTER1_DIMM_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_streamCluster1_dimm : public LCAccOperatingMode
	{
		int counter;
	public:
		inline OperatingMode_streamCluster1_dimm(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_isCenter = argAddrVec[0];

			if(argActive[0])
			{
				outputArgs.push_back(addr_isCenter);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(1 < argAddrVec.size());
			uint64_t addr_centerTable = argAddrVec[1];

			if(argActive[1])
			{
				outputArgs.push_back(addr_centerTable);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 2);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_isCenter = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_centerTable = LCACC_INTERNAL_argAddrVec[1];

			long isCenter;
			long centerTable;
			isCenter = 0;
			centerTable = 0;

			isCenter = ReadSPMInt(0, addr_isCenter, 0, true);

			#define SPMAddressOf(x) (addr_##x)
			if(isCenter != 0)
			{
				centerTable = counter;
				counter++;
			}
			#undef SPMAddressOf

			WriteSPMInt(1, addr_centerTable, 0, true, centerTable);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "streamCluster1_dimm";}
		inline virtual int ArgumentCount(){return 2;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 1);
			counter = ConvertTypes<uint32_t, int>(regs[0]);
		}
		inline static int GetOpCode(){return 1051;}
	};
}

#endif
