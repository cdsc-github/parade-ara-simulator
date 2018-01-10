#ifndef LCACC_MODE_MANHATTANDIST_H
#define LCACC_MODE_MANHATTANDIST_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_ManhattanDist : public LCAccOperatingMode
	{
		float query[96];
	public:
		inline OperatingMode_ManhattanDist(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_reference = argAddrVec[0];

			if(argActive[0])
			{
				for(size_t i = 0; i < 96 * GetArgumentWidth(0); i += GetArgumentWidth(0))
				{
					outputArgs.push_back(addr_reference + i);
				}
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(1 < argAddrVec.size());
			uint64_t addr_dist = argAddrVec[1];

			if(argActive[1])
			{
				outputArgs.push_back(addr_dist);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 2);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_reference = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_dist = LCACC_INTERNAL_argAddrVec[1];

			double reference[96];
			double dist;
			for(int i = 0; i < 96; i++)
			{
				reference[(i) % (96)] = (double)0;
			}
			dist = 0;

			for(size_t i = 0; i < 96; i++)
			{
				reference[(i) % (96)] = ReadSPMFlt(0, addr_reference, i);
			}

			#define SPMAddressOf(x) (addr_##x)
			int i;
			float q;
			
			dist = 0.0;
			
			for (i = 0; i < 96; i++) {
			  q = reference[i] - query[i];
			  dist += (q * q);
			}
			#undef SPMAddressOf

			WriteSPMFlt(1, addr_dist, 0, dist);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 10;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 9;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "ManhattanDist";}
		inline virtual int ArgumentCount(){return 2;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 96);
			query[(0) % (96)] = ConvertTypes<uint64_t, float>(regs[0]);
			query[(1) % (96)] = ConvertTypes<uint64_t, float>(regs[1]);
			query[(2) % (96)] = ConvertTypes<uint64_t, float>(regs[2]);
			query[(3) % (96)] = ConvertTypes<uint64_t, float>(regs[3]);
			query[(4) % (96)] = ConvertTypes<uint64_t, float>(regs[4]);
			query[(5) % (96)] = ConvertTypes<uint64_t, float>(regs[5]);
			query[(6) % (96)] = ConvertTypes<uint64_t, float>(regs[6]);
			query[(7) % (96)] = ConvertTypes<uint64_t, float>(regs[7]);
			query[(8) % (96)] = ConvertTypes<uint64_t, float>(regs[8]);
			query[(9) % (96)] = ConvertTypes<uint64_t, float>(regs[9]);
			query[(10) % (96)] = ConvertTypes<uint64_t, float>(regs[10]);
			query[(11) % (96)] = ConvertTypes<uint64_t, float>(regs[11]);
			query[(12) % (96)] = ConvertTypes<uint64_t, float>(regs[12]);
			query[(13) % (96)] = ConvertTypes<uint64_t, float>(regs[13]);
			query[(14) % (96)] = ConvertTypes<uint64_t, float>(regs[14]);
			query[(15) % (96)] = ConvertTypes<uint64_t, float>(regs[15]);
			query[(16) % (96)] = ConvertTypes<uint64_t, float>(regs[16]);
			query[(17) % (96)] = ConvertTypes<uint64_t, float>(regs[17]);
			query[(18) % (96)] = ConvertTypes<uint64_t, float>(regs[18]);
			query[(19) % (96)] = ConvertTypes<uint64_t, float>(regs[19]);
			query[(20) % (96)] = ConvertTypes<uint64_t, float>(regs[20]);
			query[(21) % (96)] = ConvertTypes<uint64_t, float>(regs[21]);
			query[(22) % (96)] = ConvertTypes<uint64_t, float>(regs[22]);
			query[(23) % (96)] = ConvertTypes<uint64_t, float>(regs[23]);
			query[(24) % (96)] = ConvertTypes<uint64_t, float>(regs[24]);
			query[(25) % (96)] = ConvertTypes<uint64_t, float>(regs[25]);
			query[(26) % (96)] = ConvertTypes<uint64_t, float>(regs[26]);
			query[(27) % (96)] = ConvertTypes<uint64_t, float>(regs[27]);
			query[(28) % (96)] = ConvertTypes<uint64_t, float>(regs[28]);
			query[(29) % (96)] = ConvertTypes<uint64_t, float>(regs[29]);
			query[(30) % (96)] = ConvertTypes<uint64_t, float>(regs[30]);
			query[(31) % (96)] = ConvertTypes<uint64_t, float>(regs[31]);
			query[(32) % (96)] = ConvertTypes<uint64_t, float>(regs[32]);
			query[(33) % (96)] = ConvertTypes<uint64_t, float>(regs[33]);
			query[(34) % (96)] = ConvertTypes<uint64_t, float>(regs[34]);
			query[(35) % (96)] = ConvertTypes<uint64_t, float>(regs[35]);
			query[(36) % (96)] = ConvertTypes<uint64_t, float>(regs[36]);
			query[(37) % (96)] = ConvertTypes<uint64_t, float>(regs[37]);
			query[(38) % (96)] = ConvertTypes<uint64_t, float>(regs[38]);
			query[(39) % (96)] = ConvertTypes<uint64_t, float>(regs[39]);
			query[(40) % (96)] = ConvertTypes<uint64_t, float>(regs[40]);
			query[(41) % (96)] = ConvertTypes<uint64_t, float>(regs[41]);
			query[(42) % (96)] = ConvertTypes<uint64_t, float>(regs[42]);
			query[(43) % (96)] = ConvertTypes<uint64_t, float>(regs[43]);
			query[(44) % (96)] = ConvertTypes<uint64_t, float>(regs[44]);
			query[(45) % (96)] = ConvertTypes<uint64_t, float>(regs[45]);
			query[(46) % (96)] = ConvertTypes<uint64_t, float>(regs[46]);
			query[(47) % (96)] = ConvertTypes<uint64_t, float>(regs[47]);
			query[(48) % (96)] = ConvertTypes<uint64_t, float>(regs[48]);
			query[(49) % (96)] = ConvertTypes<uint64_t, float>(regs[49]);
			query[(50) % (96)] = ConvertTypes<uint64_t, float>(regs[50]);
			query[(51) % (96)] = ConvertTypes<uint64_t, float>(regs[51]);
			query[(52) % (96)] = ConvertTypes<uint64_t, float>(regs[52]);
			query[(53) % (96)] = ConvertTypes<uint64_t, float>(regs[53]);
			query[(54) % (96)] = ConvertTypes<uint64_t, float>(regs[54]);
			query[(55) % (96)] = ConvertTypes<uint64_t, float>(regs[55]);
			query[(56) % (96)] = ConvertTypes<uint64_t, float>(regs[56]);
			query[(57) % (96)] = ConvertTypes<uint64_t, float>(regs[57]);
			query[(58) % (96)] = ConvertTypes<uint64_t, float>(regs[58]);
			query[(59) % (96)] = ConvertTypes<uint64_t, float>(regs[59]);
			query[(60) % (96)] = ConvertTypes<uint64_t, float>(regs[60]);
			query[(61) % (96)] = ConvertTypes<uint64_t, float>(regs[61]);
			query[(62) % (96)] = ConvertTypes<uint64_t, float>(regs[62]);
			query[(63) % (96)] = ConvertTypes<uint64_t, float>(regs[63]);
			query[(64) % (96)] = ConvertTypes<uint64_t, float>(regs[64]);
			query[(65) % (96)] = ConvertTypes<uint64_t, float>(regs[65]);
			query[(66) % (96)] = ConvertTypes<uint64_t, float>(regs[66]);
			query[(67) % (96)] = ConvertTypes<uint64_t, float>(regs[67]);
			query[(68) % (96)] = ConvertTypes<uint64_t, float>(regs[68]);
			query[(69) % (96)] = ConvertTypes<uint64_t, float>(regs[69]);
			query[(70) % (96)] = ConvertTypes<uint64_t, float>(regs[70]);
			query[(71) % (96)] = ConvertTypes<uint64_t, float>(regs[71]);
			query[(72) % (96)] = ConvertTypes<uint64_t, float>(regs[72]);
			query[(73) % (96)] = ConvertTypes<uint64_t, float>(regs[73]);
			query[(74) % (96)] = ConvertTypes<uint64_t, float>(regs[74]);
			query[(75) % (96)] = ConvertTypes<uint64_t, float>(regs[75]);
			query[(76) % (96)] = ConvertTypes<uint64_t, float>(regs[76]);
			query[(77) % (96)] = ConvertTypes<uint64_t, float>(regs[77]);
			query[(78) % (96)] = ConvertTypes<uint64_t, float>(regs[78]);
			query[(79) % (96)] = ConvertTypes<uint64_t, float>(regs[79]);
			query[(80) % (96)] = ConvertTypes<uint64_t, float>(regs[80]);
			query[(81) % (96)] = ConvertTypes<uint64_t, float>(regs[81]);
			query[(82) % (96)] = ConvertTypes<uint64_t, float>(regs[82]);
			query[(83) % (96)] = ConvertTypes<uint64_t, float>(regs[83]);
			query[(84) % (96)] = ConvertTypes<uint64_t, float>(regs[84]);
			query[(85) % (96)] = ConvertTypes<uint64_t, float>(regs[85]);
			query[(86) % (96)] = ConvertTypes<uint64_t, float>(regs[86]);
			query[(87) % (96)] = ConvertTypes<uint64_t, float>(regs[87]);
			query[(88) % (96)] = ConvertTypes<uint64_t, float>(regs[88]);
			query[(89) % (96)] = ConvertTypes<uint64_t, float>(regs[89]);
			query[(90) % (96)] = ConvertTypes<uint64_t, float>(regs[90]);
			query[(91) % (96)] = ConvertTypes<uint64_t, float>(regs[91]);
			query[(92) % (96)] = ConvertTypes<uint64_t, float>(regs[92]);
			query[(93) % (96)] = ConvertTypes<uint64_t, float>(regs[93]);
			query[(94) % (96)] = ConvertTypes<uint64_t, float>(regs[94]);
			query[(95) % (96)] = ConvertTypes<uint64_t, float>(regs[95]);
		}
		inline static int GetOpCode(){return 247;}
	};
}

#endif
