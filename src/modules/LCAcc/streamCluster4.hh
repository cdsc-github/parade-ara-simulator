#ifndef LCACC_MODE_STREAMCLUSTER4_H
#define LCACC_MODE_STREAMCLUSTER4_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_streamCluster4 : public LCAccOperatingMode
	{
		uint64_t gl_lowStart;
		uint64_t lowStart;
	public:
		inline OperatingMode_streamCluster4(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_predicate = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_table = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_low = argAddrVec[2];

			if(argActive[0])
			{
				outputArgs.push_back(addr_predicate);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_table);
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_low);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{

		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 3);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_predicate = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_table = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_low = LCACC_INTERNAL_argAddrVec[2];

			uint64_t predicate;
			uint64_t table;
			uint64_t low;
			predicate = 0;
			table = 0;
			low = 0;

			predicate = ReadSPMInt(0, addr_predicate, 0, true);
			table = ReadSPMInt(1, addr_table, 0, true);
			low = ReadSPMInt(2, addr_low, 0, true);

			#define SPMAddressOf(x) (addr_##x)
			if(predicate)
			{
				AddWrite(lowStart + iteration * sizeof(float), gl_lowStart + table * sizeof(float), sizeof(float));
			}
			#undef SPMAddressOf

		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "streamCluster4";}
		inline virtual int ArgumentCount(){return 3;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 2);
			gl_lowStart = ConvertTypes<uint64_t, uint64_t>(regs[0]);
			lowStart = ConvertTypes<uint64_t, uint64_t>(regs[1]);
		}
		inline static int GetOpCode(){return 1204;}
	};
}

#endif
