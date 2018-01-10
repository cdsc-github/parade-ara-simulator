#ifndef LCACC_MODE_MERGESORT_H
#define LCACC_MODE_MERGESORT_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_MergeSort : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_MergeSort(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_dist = argAddrVec[0];

			if(argActive[0])
			{
				for(size_t i = 0; i < 128 * GetArgumentWidth(0); i += GetArgumentWidth(0))
				{
					outputArgs.push_back(addr_dist + i);
				}
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{

		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 1);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_dist = LCACC_INTERNAL_argAddrVec[0];

			double dist[128];
			for(int i = 0; i < 128; i++)
			{
				dist[(i) % (128)] = (double)0;
			}

			for(size_t i = 0; i < 128; i++)
			{
				dist[(i) % (128)] = ReadSPMFlt(0, addr_dist, i);
			}

			#define SPMAddressOf(x) (addr_##x)
			#ifndef merge
			#define merge(a, start, m, stop) { \
			  float temp[128]; \
			  int i, j, k; \
			  for (i = start; i <= m; i++) { \
			    temp[i] = a[i]; \
			  } \
			  for (j = m + 1; j <= stop; j++) { \
			    temp[m + 1 + stop - j] = a[j]; \
			  } \
			  i = start; \
			  j = stop; \
			  for (k = start; k <= stop; k++) { \
			    float tmp_j = temp[j]; \
			    float tmp_i = temp[i]; \
			    if (tmp_j < tmp_i) { \
			      a[k] = tmp_j; \
			      j--; \
			    } else { \
			      a[k] = tmp_i; \
			      i++; \
			    } \
			  } \
			}
			#endif
			
			int start, stop;
			int i, m, from, mid, to;
			
			start = 0;
			stop = 128;
			
			for (m = 1; m < stop - start; m += m) {
			  for (i = start; i < stop; i += m + m) {
			    from = i;
			    mid = i + m - 1;
			    to = i + m + m - 1;
			    if (to < stop) {
			      merge(dist, from, mid, to);
			    } else {
			      merge(dist, from, mid, stop);
			    }
			  }
			}
			
			#undef merge
			#undef SPMAddressOf

		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 10;}
		inline virtual int InitiationInterval(){return 4;}
		inline virtual int PipelineDepth(){return 513;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "MergeSort";}
		inline virtual int ArgumentCount(){return 1;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 248;}
	};
}

#endif
