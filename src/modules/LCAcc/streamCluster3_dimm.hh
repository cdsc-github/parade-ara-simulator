#ifndef LCACC_MODE_STREAMCLUSTER3_DIMM_H
#define LCACC_MODE_STREAMCLUSTER3_DIMM_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_streamCluster3_dimm : public LCAccOperatingMode
	{
		int counter;
	public:
		inline OperatingMode_streamCluster3_dimm(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_centerTable = argAddrVec[0];

			if(argActive[0])
			{
				outputArgs.push_back(addr_centerTable);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(1 < argAddrVec.size());
			uint64_t addr_centerTableOut = argAddrVec[1];

			if(argActive[1])
			{
				outputArgs.push_back(addr_centerTableOut);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 2);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_centerTable = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_centerTableOut = LCACC_INTERNAL_argAddrVec[1];

			long centerTable;
			long centerTableOut;
			centerTable = 0;
			centerTableOut = 0;

			centerTable = ReadSPMInt(0, addr_centerTable, 0, true);

			#define SPMAddressOf(x) (addr_##x)
			//for ( int i = k1; i < k2; i++ ) {
				//	if( is_center[i] ) {
				//		double low = z;
				//		//aggregate from all threads
				//		for( int p = 0; p < nproc; p++ ) {
				//			low += work_mem[center_table[i]+p*stride];
				//		}
				//		gl_lower[center_table[i]] = low;
				//		if ( low > 0 ) {
				//			// i is a median, and
				//			// if we were to open x (which we still may not) we'd close i

				//			// note, we'll ignore the following quantity unless we do open x
				//			++number_of_centers_to_close;
				//			cost_of_opening_x -= low;
				//		}
				//	}
				//}
				////use the rest of working memory to store the following
				//work_mem[pid*stride + K] = number_of_centers_to_close;
				//work_mem[pid*stride + K+1] = cost_of_opening_x;

				//if( pid==0 ) {
				//	gl_cost_of_opening_x = z;
				//	//aggregate
				//	for( int p = 0; p < nproc; p++ ) {
				//		gl_number_of_centers_to_close += (int)work_mem[p*stride + K];
				//		gl_cost_of_opening_x += work_mem[p*stride+K+1];
				//	}
				//}


			centerTableOut = centerTable;//just a place holder
			#undef SPMAddressOf

			WriteSPMInt(1, addr_centerTableOut, 0, true, centerTableOut);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "streamCluster3_dimm";}
		inline virtual int ArgumentCount(){return 2;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 1);
			counter = ConvertTypes<uint32_t, int>(regs[0]);
		}
		inline static int GetOpCode(){return 1053;}
	};
}

#endif
