#ifndef LCACC_MODE_TEXSYNTH1_H
#define LCACC_MODE_TEXSYNTH1_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_TexSynth1 : public LCAccOperatingMode
	{
		int WIDTHin;
		int HEIGHTin;
		int WIDTHout;
		int HEIGHTout;
		intptr_t inputImage;
		uint32_t outputResult;
		uint32_t in_rand_seeds[100];
	public:
		inline OperatingMode_TexSynth1(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{

		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_out_atlas = argAddrVec[0];

			if(argActive[0])
			{
				for(size_t i = 0; i < 2 * GetArgumentWidth(0); i += GetArgumentWidth(0))
				{
					outputArgs.push_back(addr_out_atlas + i);
				}
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 1);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_out_atlas = LCACC_INTERNAL_argAddrVec[0];

			int64_t out_atlas[2];
			for(int i = 0; i < 2; i++)
			{
				out_atlas[(i) % (2)] = (int64_t)0;
			}


			#define SPMAddressOf(x) (addr_##x)
			#define RAND(seed_z_uint, seed_w_uint, ret) { \
			    seed_z_uint = 36969 * (seed_z_uint & 65535) + (seed_z_uint >> 16); \
			    seed_w_uint = 18000 * (seed_w_uint & 65535) + (seed_w_uint >> 16); \
			    ret = (seed_z_uint << 16) + seed_w_uint; \
			}
			
				int x = (iteration + maxIteration * taskID) % (WIDTHout * HEIGHTout);
				int imageIndex = (iteration + maxIteration * taskID) / (WIDTHout * HEIGHTout);
				int in_i = x / WIDTHout;
				int in_j = x % WIDTHout;
				int tmpx, tmpy;
				int rand_val1, rand_val2;
				int seed_z, seed_w;
				
				seed_z = in_rand_seeds[in_j % 100];
				seed_w = in_rand_seeds[in_i % 100];
				RAND(seed_z, seed_w, rand_val1);
				RAND(seed_z, seed_w, rand_val2);
				tmpx = 1 + rand_val1 % (WIDTHin - 2);
				tmpy = 1 + rand_val2 % (HEIGHTin - 2);
				out_atlas[0] = tmpx;
				out_atlas[1] = tmpy;
				AddRead(inputImage + //src
					imageIndex * HEIGHTin * WIDTHin * 4 + 
					(tmpx + WIDTHin * tmpy) * 4, 
					outputResult + (iteration) * 4, //dst
					4);//size
			
			#undef RAND
			#undef SPMAddressOf

			for(size_t i = 0; i < 2; i++)
			{
				WriteSPMInt(0, addr_out_atlas, i, true, out_atlas[(i) % (2)]);
			}
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 15;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "TexSynth1";}
		inline virtual int ArgumentCount(){return 1;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 106);
			WIDTHin = ConvertTypes<uint64_t, int>(regs[0]);
			HEIGHTin = ConvertTypes<uint64_t, int>(regs[1]);
			WIDTHout = ConvertTypes<uint64_t, int>(regs[2]);
			HEIGHTout = ConvertTypes<uint64_t, int>(regs[3]);
			inputImage = ConvertTypes<uint64_t, intptr_t>(regs[4]);
			outputResult = ConvertTypes<uint64_t, uint32_t>(regs[5]);
			in_rand_seeds[(0) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[6]);
			in_rand_seeds[(1) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[7]);
			in_rand_seeds[(2) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[8]);
			in_rand_seeds[(3) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[9]);
			in_rand_seeds[(4) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[10]);
			in_rand_seeds[(5) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[11]);
			in_rand_seeds[(6) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[12]);
			in_rand_seeds[(7) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[13]);
			in_rand_seeds[(8) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[14]);
			in_rand_seeds[(9) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[15]);
			in_rand_seeds[(10) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[16]);
			in_rand_seeds[(11) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[17]);
			in_rand_seeds[(12) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[18]);
			in_rand_seeds[(13) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[19]);
			in_rand_seeds[(14) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[20]);
			in_rand_seeds[(15) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[21]);
			in_rand_seeds[(16) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[22]);
			in_rand_seeds[(17) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[23]);
			in_rand_seeds[(18) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[24]);
			in_rand_seeds[(19) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[25]);
			in_rand_seeds[(20) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[26]);
			in_rand_seeds[(21) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[27]);
			in_rand_seeds[(22) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[28]);
			in_rand_seeds[(23) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[29]);
			in_rand_seeds[(24) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[30]);
			in_rand_seeds[(25) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[31]);
			in_rand_seeds[(26) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[32]);
			in_rand_seeds[(27) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[33]);
			in_rand_seeds[(28) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[34]);
			in_rand_seeds[(29) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[35]);
			in_rand_seeds[(30) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[36]);
			in_rand_seeds[(31) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[37]);
			in_rand_seeds[(32) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[38]);
			in_rand_seeds[(33) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[39]);
			in_rand_seeds[(34) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[40]);
			in_rand_seeds[(35) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[41]);
			in_rand_seeds[(36) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[42]);
			in_rand_seeds[(37) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[43]);
			in_rand_seeds[(38) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[44]);
			in_rand_seeds[(39) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[45]);
			in_rand_seeds[(40) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[46]);
			in_rand_seeds[(41) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[47]);
			in_rand_seeds[(42) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[48]);
			in_rand_seeds[(43) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[49]);
			in_rand_seeds[(44) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[50]);
			in_rand_seeds[(45) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[51]);
			in_rand_seeds[(46) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[52]);
			in_rand_seeds[(47) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[53]);
			in_rand_seeds[(48) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[54]);
			in_rand_seeds[(49) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[55]);
			in_rand_seeds[(50) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[56]);
			in_rand_seeds[(51) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[57]);
			in_rand_seeds[(52) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[58]);
			in_rand_seeds[(53) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[59]);
			in_rand_seeds[(54) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[60]);
			in_rand_seeds[(55) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[61]);
			in_rand_seeds[(56) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[62]);
			in_rand_seeds[(57) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[63]);
			in_rand_seeds[(58) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[64]);
			in_rand_seeds[(59) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[65]);
			in_rand_seeds[(60) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[66]);
			in_rand_seeds[(61) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[67]);
			in_rand_seeds[(62) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[68]);
			in_rand_seeds[(63) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[69]);
			in_rand_seeds[(64) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[70]);
			in_rand_seeds[(65) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[71]);
			in_rand_seeds[(66) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[72]);
			in_rand_seeds[(67) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[73]);
			in_rand_seeds[(68) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[74]);
			in_rand_seeds[(69) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[75]);
			in_rand_seeds[(70) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[76]);
			in_rand_seeds[(71) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[77]);
			in_rand_seeds[(72) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[78]);
			in_rand_seeds[(73) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[79]);
			in_rand_seeds[(74) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[80]);
			in_rand_seeds[(75) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[81]);
			in_rand_seeds[(76) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[82]);
			in_rand_seeds[(77) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[83]);
			in_rand_seeds[(78) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[84]);
			in_rand_seeds[(79) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[85]);
			in_rand_seeds[(80) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[86]);
			in_rand_seeds[(81) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[87]);
			in_rand_seeds[(82) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[88]);
			in_rand_seeds[(83) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[89]);
			in_rand_seeds[(84) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[90]);
			in_rand_seeds[(85) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[91]);
			in_rand_seeds[(86) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[92]);
			in_rand_seeds[(87) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[93]);
			in_rand_seeds[(88) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[94]);
			in_rand_seeds[(89) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[95]);
			in_rand_seeds[(90) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[96]);
			in_rand_seeds[(91) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[97]);
			in_rand_seeds[(92) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[98]);
			in_rand_seeds[(93) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[99]);
			in_rand_seeds[(94) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[100]);
			in_rand_seeds[(95) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[101]);
			in_rand_seeds[(96) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[102]);
			in_rand_seeds[(97) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[103]);
			in_rand_seeds[(98) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[104]);
			in_rand_seeds[(99) % (100)] = ConvertTypes<uint64_t, uint32_t>(regs[105]);
		}
		inline static int GetOpCode(){return 910;}
	};
}

#endif
