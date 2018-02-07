#ifndef LCACC_MODE_PARTIALSORT_H
#define LCACC_MODE_PARTIALSORT_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_PartialSort : public LCAccOperatingMode
	{
		float topK[100];
	public:
		inline OperatingMode_PartialSort(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_dists = argAddrVec[0];

			if(argActive[0])
			{
				outputArgs.push_back(addr_dists);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{

		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 1);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_dists = LCACC_INTERNAL_argAddrVec[0];

			double dists;
			dists = 0;

			dists = ReadSPMFlt(0, addr_dists, 0);

			#define SPMAddressOf(x) (addr_##x)
			#define K 100
			
			int i;
			float carry[K];
			float temp;
			
			for (i = 0; i < K; i++) {
			  if (carry[i] < topK[i]) {
			    temp = topK[i];
			    topK[i] = carry[i];
			    carry[i] = temp;
			  }
			}
			
			for (i = 1; i < K; i++) {
			  carry[i] = carry[i - 1];
			}
			
			carry[0] = dists;
			#undef SPMAddressOf

		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 10;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 100;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "PartialSort";}
		inline virtual int ArgumentCount(){return 1;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 100);
			topK[(0) % (100)] = ConvertTypes<uint64_t, float>(regs[0]);
			topK[(1) % (100)] = ConvertTypes<uint64_t, float>(regs[1]);
			topK[(2) % (100)] = ConvertTypes<uint64_t, float>(regs[2]);
			topK[(3) % (100)] = ConvertTypes<uint64_t, float>(regs[3]);
			topK[(4) % (100)] = ConvertTypes<uint64_t, float>(regs[4]);
			topK[(5) % (100)] = ConvertTypes<uint64_t, float>(regs[5]);
			topK[(6) % (100)] = ConvertTypes<uint64_t, float>(regs[6]);
			topK[(7) % (100)] = ConvertTypes<uint64_t, float>(regs[7]);
			topK[(8) % (100)] = ConvertTypes<uint64_t, float>(regs[8]);
			topK[(9) % (100)] = ConvertTypes<uint64_t, float>(regs[9]);
			topK[(10) % (100)] = ConvertTypes<uint64_t, float>(regs[10]);
			topK[(11) % (100)] = ConvertTypes<uint64_t, float>(regs[11]);
			topK[(12) % (100)] = ConvertTypes<uint64_t, float>(regs[12]);
			topK[(13) % (100)] = ConvertTypes<uint64_t, float>(regs[13]);
			topK[(14) % (100)] = ConvertTypes<uint64_t, float>(regs[14]);
			topK[(15) % (100)] = ConvertTypes<uint64_t, float>(regs[15]);
			topK[(16) % (100)] = ConvertTypes<uint64_t, float>(regs[16]);
			topK[(17) % (100)] = ConvertTypes<uint64_t, float>(regs[17]);
			topK[(18) % (100)] = ConvertTypes<uint64_t, float>(regs[18]);
			topK[(19) % (100)] = ConvertTypes<uint64_t, float>(regs[19]);
			topK[(20) % (100)] = ConvertTypes<uint64_t, float>(regs[20]);
			topK[(21) % (100)] = ConvertTypes<uint64_t, float>(regs[21]);
			topK[(22) % (100)] = ConvertTypes<uint64_t, float>(regs[22]);
			topK[(23) % (100)] = ConvertTypes<uint64_t, float>(regs[23]);
			topK[(24) % (100)] = ConvertTypes<uint64_t, float>(regs[24]);
			topK[(25) % (100)] = ConvertTypes<uint64_t, float>(regs[25]);
			topK[(26) % (100)] = ConvertTypes<uint64_t, float>(regs[26]);
			topK[(27) % (100)] = ConvertTypes<uint64_t, float>(regs[27]);
			topK[(28) % (100)] = ConvertTypes<uint64_t, float>(regs[28]);
			topK[(29) % (100)] = ConvertTypes<uint64_t, float>(regs[29]);
			topK[(30) % (100)] = ConvertTypes<uint64_t, float>(regs[30]);
			topK[(31) % (100)] = ConvertTypes<uint64_t, float>(regs[31]);
			topK[(32) % (100)] = ConvertTypes<uint64_t, float>(regs[32]);
			topK[(33) % (100)] = ConvertTypes<uint64_t, float>(regs[33]);
			topK[(34) % (100)] = ConvertTypes<uint64_t, float>(regs[34]);
			topK[(35) % (100)] = ConvertTypes<uint64_t, float>(regs[35]);
			topK[(36) % (100)] = ConvertTypes<uint64_t, float>(regs[36]);
			topK[(37) % (100)] = ConvertTypes<uint64_t, float>(regs[37]);
			topK[(38) % (100)] = ConvertTypes<uint64_t, float>(regs[38]);
			topK[(39) % (100)] = ConvertTypes<uint64_t, float>(regs[39]);
			topK[(40) % (100)] = ConvertTypes<uint64_t, float>(regs[40]);
			topK[(41) % (100)] = ConvertTypes<uint64_t, float>(regs[41]);
			topK[(42) % (100)] = ConvertTypes<uint64_t, float>(regs[42]);
			topK[(43) % (100)] = ConvertTypes<uint64_t, float>(regs[43]);
			topK[(44) % (100)] = ConvertTypes<uint64_t, float>(regs[44]);
			topK[(45) % (100)] = ConvertTypes<uint64_t, float>(regs[45]);
			topK[(46) % (100)] = ConvertTypes<uint64_t, float>(regs[46]);
			topK[(47) % (100)] = ConvertTypes<uint64_t, float>(regs[47]);
			topK[(48) % (100)] = ConvertTypes<uint64_t, float>(regs[48]);
			topK[(49) % (100)] = ConvertTypes<uint64_t, float>(regs[49]);
			topK[(50) % (100)] = ConvertTypes<uint64_t, float>(regs[50]);
			topK[(51) % (100)] = ConvertTypes<uint64_t, float>(regs[51]);
			topK[(52) % (100)] = ConvertTypes<uint64_t, float>(regs[52]);
			topK[(53) % (100)] = ConvertTypes<uint64_t, float>(regs[53]);
			topK[(54) % (100)] = ConvertTypes<uint64_t, float>(regs[54]);
			topK[(55) % (100)] = ConvertTypes<uint64_t, float>(regs[55]);
			topK[(56) % (100)] = ConvertTypes<uint64_t, float>(regs[56]);
			topK[(57) % (100)] = ConvertTypes<uint64_t, float>(regs[57]);
			topK[(58) % (100)] = ConvertTypes<uint64_t, float>(regs[58]);
			topK[(59) % (100)] = ConvertTypes<uint64_t, float>(regs[59]);
			topK[(60) % (100)] = ConvertTypes<uint64_t, float>(regs[60]);
			topK[(61) % (100)] = ConvertTypes<uint64_t, float>(regs[61]);
			topK[(62) % (100)] = ConvertTypes<uint64_t, float>(regs[62]);
			topK[(63) % (100)] = ConvertTypes<uint64_t, float>(regs[63]);
			topK[(64) % (100)] = ConvertTypes<uint64_t, float>(regs[64]);
			topK[(65) % (100)] = ConvertTypes<uint64_t, float>(regs[65]);
			topK[(66) % (100)] = ConvertTypes<uint64_t, float>(regs[66]);
			topK[(67) % (100)] = ConvertTypes<uint64_t, float>(regs[67]);
			topK[(68) % (100)] = ConvertTypes<uint64_t, float>(regs[68]);
			topK[(69) % (100)] = ConvertTypes<uint64_t, float>(regs[69]);
			topK[(70) % (100)] = ConvertTypes<uint64_t, float>(regs[70]);
			topK[(71) % (100)] = ConvertTypes<uint64_t, float>(regs[71]);
			topK[(72) % (100)] = ConvertTypes<uint64_t, float>(regs[72]);
			topK[(73) % (100)] = ConvertTypes<uint64_t, float>(regs[73]);
			topK[(74) % (100)] = ConvertTypes<uint64_t, float>(regs[74]);
			topK[(75) % (100)] = ConvertTypes<uint64_t, float>(regs[75]);
			topK[(76) % (100)] = ConvertTypes<uint64_t, float>(regs[76]);
			topK[(77) % (100)] = ConvertTypes<uint64_t, float>(regs[77]);
			topK[(78) % (100)] = ConvertTypes<uint64_t, float>(regs[78]);
			topK[(79) % (100)] = ConvertTypes<uint64_t, float>(regs[79]);
			topK[(80) % (100)] = ConvertTypes<uint64_t, float>(regs[80]);
			topK[(81) % (100)] = ConvertTypes<uint64_t, float>(regs[81]);
			topK[(82) % (100)] = ConvertTypes<uint64_t, float>(regs[82]);
			topK[(83) % (100)] = ConvertTypes<uint64_t, float>(regs[83]);
			topK[(84) % (100)] = ConvertTypes<uint64_t, float>(regs[84]);
			topK[(85) % (100)] = ConvertTypes<uint64_t, float>(regs[85]);
			topK[(86) % (100)] = ConvertTypes<uint64_t, float>(regs[86]);
			topK[(87) % (100)] = ConvertTypes<uint64_t, float>(regs[87]);
			topK[(88) % (100)] = ConvertTypes<uint64_t, float>(regs[88]);
			topK[(89) % (100)] = ConvertTypes<uint64_t, float>(regs[89]);
			topK[(90) % (100)] = ConvertTypes<uint64_t, float>(regs[90]);
			topK[(91) % (100)] = ConvertTypes<uint64_t, float>(regs[91]);
			topK[(92) % (100)] = ConvertTypes<uint64_t, float>(regs[92]);
			topK[(93) % (100)] = ConvertTypes<uint64_t, float>(regs[93]);
			topK[(94) % (100)] = ConvertTypes<uint64_t, float>(regs[94]);
			topK[(95) % (100)] = ConvertTypes<uint64_t, float>(regs[95]);
			topK[(96) % (100)] = ConvertTypes<uint64_t, float>(regs[96]);
			topK[(97) % (100)] = ConvertTypes<uint64_t, float>(regs[97]);
			topK[(98) % (100)] = ConvertTypes<uint64_t, float>(regs[98]);
			topK[(99) % (100)] = ConvertTypes<uint64_t, float>(regs[99]);
		}
		inline static int GetOpCode(){return 249;}
	};
}

#endif
