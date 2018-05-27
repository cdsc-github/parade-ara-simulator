#ifndef LCACC_MODE_STREAMCLUSTER2_DIMM_H
#define LCACC_MODE_STREAMCLUSTER2_DIMM_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_streamCluster2_dimm : public LCAccOperatingMode
	{
		float px0;
		float px1;
		float px2;
		float px3;
		float px4;
		float px5;
		float px6;
		float px7;
		float px8;
		float px9;
		float px10;
		float px11;
		float px12;
		float px13;
		float px14;
		float px15;
		float px16;
		float px17;
		float px18;
		float px19;
		float px20;
		float px21;
		float px22;
		float px23;
		float px24;
		float px25;
		float px26;
		float px27;
		float px28;
		float px29;
		float px30;
		float px31;
		float px32;
		float px33;
		float px34;
		float px35;
		float px36;
		float px37;
		float px38;
		float px39;
		float px40;
		float px41;
		float px42;
		float px43;
		float px44;
		float px45;
		float px46;
		float px47;
		float px48;
		float px49;
		float px50;
		float px51;
		float px52;
		float px53;
		float px54;
		float px55;
		float px56;
		float px57;
		float px58;
		float px59;
		float px60;
		float px61;
		float px62;
		float px63;
	public:
		inline OperatingMode_streamCluster2_dimm(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_weight = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_p0 = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_p1 = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_p2 = argAddrVec[3];
			assert(4 < argAddrVec.size());
			uint64_t addr_p3 = argAddrVec[4];
			assert(5 < argAddrVec.size());
			uint64_t addr_p4 = argAddrVec[5];
			assert(6 < argAddrVec.size());
			uint64_t addr_p5 = argAddrVec[6];
			assert(7 < argAddrVec.size());
			uint64_t addr_p6 = argAddrVec[7];
			assert(8 < argAddrVec.size());
			uint64_t addr_p7 = argAddrVec[8];
			assert(9 < argAddrVec.size());
			uint64_t addr_p8 = argAddrVec[9];
			assert(10 < argAddrVec.size());
			uint64_t addr_p9 = argAddrVec[10];
			assert(11 < argAddrVec.size());
			uint64_t addr_p10 = argAddrVec[11];
			assert(12 < argAddrVec.size());
			uint64_t addr_p11 = argAddrVec[12];
			assert(13 < argAddrVec.size());
			uint64_t addr_p12 = argAddrVec[13];
			assert(14 < argAddrVec.size());
			uint64_t addr_p13 = argAddrVec[14];
			assert(15 < argAddrVec.size());
			uint64_t addr_p14 = argAddrVec[15];
			assert(16 < argAddrVec.size());
			uint64_t addr_p15 = argAddrVec[16];
			assert(17 < argAddrVec.size());
			uint64_t addr_p16 = argAddrVec[17];
			assert(18 < argAddrVec.size());
			uint64_t addr_p17 = argAddrVec[18];
			assert(19 < argAddrVec.size());
			uint64_t addr_p18 = argAddrVec[19];
			assert(20 < argAddrVec.size());
			uint64_t addr_p19 = argAddrVec[20];
			assert(21 < argAddrVec.size());
			uint64_t addr_p20 = argAddrVec[21];
			assert(22 < argAddrVec.size());
			uint64_t addr_p21 = argAddrVec[22];
			assert(23 < argAddrVec.size());
			uint64_t addr_p22 = argAddrVec[23];
			assert(24 < argAddrVec.size());
			uint64_t addr_p23 = argAddrVec[24];
			assert(25 < argAddrVec.size());
			uint64_t addr_p24 = argAddrVec[25];
			assert(26 < argAddrVec.size());
			uint64_t addr_p25 = argAddrVec[26];
			assert(27 < argAddrVec.size());
			uint64_t addr_p26 = argAddrVec[27];
			assert(28 < argAddrVec.size());
			uint64_t addr_p27 = argAddrVec[28];
			assert(29 < argAddrVec.size());
			uint64_t addr_p28 = argAddrVec[29];
			assert(30 < argAddrVec.size());
			uint64_t addr_p29 = argAddrVec[30];
			assert(31 < argAddrVec.size());
			uint64_t addr_p30 = argAddrVec[31];
			assert(32 < argAddrVec.size());
			uint64_t addr_p31 = argAddrVec[32];
			assert(33 < argAddrVec.size());
			uint64_t addr_p32 = argAddrVec[33];
			assert(34 < argAddrVec.size());
			uint64_t addr_p33 = argAddrVec[34];
			assert(35 < argAddrVec.size());
			uint64_t addr_p34 = argAddrVec[35];
			assert(36 < argAddrVec.size());
			uint64_t addr_p35 = argAddrVec[36];
			assert(37 < argAddrVec.size());
			uint64_t addr_p36 = argAddrVec[37];
			assert(38 < argAddrVec.size());
			uint64_t addr_p37 = argAddrVec[38];
			assert(39 < argAddrVec.size());
			uint64_t addr_p38 = argAddrVec[39];
			assert(40 < argAddrVec.size());
			uint64_t addr_p39 = argAddrVec[40];
			assert(41 < argAddrVec.size());
			uint64_t addr_p40 = argAddrVec[41];
			assert(42 < argAddrVec.size());
			uint64_t addr_p41 = argAddrVec[42];
			assert(43 < argAddrVec.size());
			uint64_t addr_p42 = argAddrVec[43];
			assert(44 < argAddrVec.size());
			uint64_t addr_p43 = argAddrVec[44];
			assert(45 < argAddrVec.size());
			uint64_t addr_p44 = argAddrVec[45];
			assert(46 < argAddrVec.size());
			uint64_t addr_p45 = argAddrVec[46];
			assert(47 < argAddrVec.size());
			uint64_t addr_p46 = argAddrVec[47];
			assert(48 < argAddrVec.size());
			uint64_t addr_p47 = argAddrVec[48];
			assert(49 < argAddrVec.size());
			uint64_t addr_p48 = argAddrVec[49];
			assert(50 < argAddrVec.size());
			uint64_t addr_p49 = argAddrVec[50];
			assert(51 < argAddrVec.size());
			uint64_t addr_p50 = argAddrVec[51];
			assert(52 < argAddrVec.size());
			uint64_t addr_p51 = argAddrVec[52];
			assert(53 < argAddrVec.size());
			uint64_t addr_p52 = argAddrVec[53];
			assert(54 < argAddrVec.size());
			uint64_t addr_p53 = argAddrVec[54];
			assert(55 < argAddrVec.size());
			uint64_t addr_p54 = argAddrVec[55];
			assert(56 < argAddrVec.size());
			uint64_t addr_p55 = argAddrVec[56];
			assert(57 < argAddrVec.size());
			uint64_t addr_p56 = argAddrVec[57];
			assert(58 < argAddrVec.size());
			uint64_t addr_p57 = argAddrVec[58];
			assert(59 < argAddrVec.size());
			uint64_t addr_p58 = argAddrVec[59];
			assert(60 < argAddrVec.size());
			uint64_t addr_p59 = argAddrVec[60];
			assert(61 < argAddrVec.size());
			uint64_t addr_p60 = argAddrVec[61];
			assert(62 < argAddrVec.size());
			uint64_t addr_p61 = argAddrVec[62];
			assert(63 < argAddrVec.size());
			uint64_t addr_p62 = argAddrVec[63];
			assert(64 < argAddrVec.size());
			uint64_t addr_p63 = argAddrVec[64];

			if(argActive[0])
			{
				outputArgs.push_back(addr_weight);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_p0);
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_p1);
			}
			if(argActive[3])
			{
				outputArgs.push_back(addr_p2);
			}
			if(argActive[4])
			{
				outputArgs.push_back(addr_p3);
			}
			if(argActive[5])
			{
				outputArgs.push_back(addr_p4);
			}
			if(argActive[6])
			{
				outputArgs.push_back(addr_p5);
			}
			if(argActive[7])
			{
				outputArgs.push_back(addr_p6);
			}
			if(argActive[8])
			{
				outputArgs.push_back(addr_p7);
			}
			if(argActive[9])
			{
				outputArgs.push_back(addr_p8);
			}
			if(argActive[10])
			{
				outputArgs.push_back(addr_p9);
			}
			if(argActive[11])
			{
				outputArgs.push_back(addr_p10);
			}
			if(argActive[12])
			{
				outputArgs.push_back(addr_p11);
			}
			if(argActive[13])
			{
				outputArgs.push_back(addr_p12);
			}
			if(argActive[14])
			{
				outputArgs.push_back(addr_p13);
			}
			if(argActive[15])
			{
				outputArgs.push_back(addr_p14);
			}
			if(argActive[16])
			{
				outputArgs.push_back(addr_p15);
			}
			if(argActive[17])
			{
				outputArgs.push_back(addr_p16);
			}
			if(argActive[18])
			{
				outputArgs.push_back(addr_p17);
			}
			if(argActive[19])
			{
				outputArgs.push_back(addr_p18);
			}
			if(argActive[20])
			{
				outputArgs.push_back(addr_p19);
			}
			if(argActive[21])
			{
				outputArgs.push_back(addr_p20);
			}
			if(argActive[22])
			{
				outputArgs.push_back(addr_p21);
			}
			if(argActive[23])
			{
				outputArgs.push_back(addr_p22);
			}
			if(argActive[24])
			{
				outputArgs.push_back(addr_p23);
			}
			if(argActive[25])
			{
				outputArgs.push_back(addr_p24);
			}
			if(argActive[26])
			{
				outputArgs.push_back(addr_p25);
			}
			if(argActive[27])
			{
				outputArgs.push_back(addr_p26);
			}
			if(argActive[28])
			{
				outputArgs.push_back(addr_p27);
			}
			if(argActive[29])
			{
				outputArgs.push_back(addr_p28);
			}
			if(argActive[30])
			{
				outputArgs.push_back(addr_p29);
			}
			if(argActive[31])
			{
				outputArgs.push_back(addr_p30);
			}
			if(argActive[32])
			{
				outputArgs.push_back(addr_p31);
			}
			if(argActive[33])
			{
				outputArgs.push_back(addr_p32);
			}
			if(argActive[34])
			{
				outputArgs.push_back(addr_p33);
			}
			if(argActive[35])
			{
				outputArgs.push_back(addr_p34);
			}
			if(argActive[36])
			{
				outputArgs.push_back(addr_p35);
			}
			if(argActive[37])
			{
				outputArgs.push_back(addr_p36);
			}
			if(argActive[38])
			{
				outputArgs.push_back(addr_p37);
			}
			if(argActive[39])
			{
				outputArgs.push_back(addr_p38);
			}
			if(argActive[40])
			{
				outputArgs.push_back(addr_p39);
			}
			if(argActive[41])
			{
				outputArgs.push_back(addr_p40);
			}
			if(argActive[42])
			{
				outputArgs.push_back(addr_p41);
			}
			if(argActive[43])
			{
				outputArgs.push_back(addr_p42);
			}
			if(argActive[44])
			{
				outputArgs.push_back(addr_p43);
			}
			if(argActive[45])
			{
				outputArgs.push_back(addr_p44);
			}
			if(argActive[46])
			{
				outputArgs.push_back(addr_p45);
			}
			if(argActive[47])
			{
				outputArgs.push_back(addr_p46);
			}
			if(argActive[48])
			{
				outputArgs.push_back(addr_p47);
			}
			if(argActive[49])
			{
				outputArgs.push_back(addr_p48);
			}
			if(argActive[50])
			{
				outputArgs.push_back(addr_p49);
			}
			if(argActive[51])
			{
				outputArgs.push_back(addr_p50);
			}
			if(argActive[52])
			{
				outputArgs.push_back(addr_p51);
			}
			if(argActive[53])
			{
				outputArgs.push_back(addr_p52);
			}
			if(argActive[54])
			{
				outputArgs.push_back(addr_p53);
			}
			if(argActive[55])
			{
				outputArgs.push_back(addr_p54);
			}
			if(argActive[56])
			{
				outputArgs.push_back(addr_p55);
			}
			if(argActive[57])
			{
				outputArgs.push_back(addr_p56);
			}
			if(argActive[58])
			{
				outputArgs.push_back(addr_p57);
			}
			if(argActive[59])
			{
				outputArgs.push_back(addr_p58);
			}
			if(argActive[60])
			{
				outputArgs.push_back(addr_p59);
			}
			if(argActive[61])
			{
				outputArgs.push_back(addr_p60);
			}
			if(argActive[62])
			{
				outputArgs.push_back(addr_p61);
			}
			if(argActive[63])
			{
				outputArgs.push_back(addr_p62);
			}
			if(argActive[64])
			{
				outputArgs.push_back(addr_p63);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(65 < argAddrVec.size());
			uint64_t addr_dist = argAddrVec[65];

			if(argActive[65])
			{
				outputArgs.push_back(addr_dist);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 66);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_weight = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p0 = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p1 = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p2 = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p3 = LCACC_INTERNAL_argAddrVec[4];
			assert(5 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p4 = LCACC_INTERNAL_argAddrVec[5];
			assert(6 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p5 = LCACC_INTERNAL_argAddrVec[6];
			assert(7 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p6 = LCACC_INTERNAL_argAddrVec[7];
			assert(8 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p7 = LCACC_INTERNAL_argAddrVec[8];
			assert(9 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p8 = LCACC_INTERNAL_argAddrVec[9];
			assert(10 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p9 = LCACC_INTERNAL_argAddrVec[10];
			assert(11 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p10 = LCACC_INTERNAL_argAddrVec[11];
			assert(12 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p11 = LCACC_INTERNAL_argAddrVec[12];
			assert(13 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p12 = LCACC_INTERNAL_argAddrVec[13];
			assert(14 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p13 = LCACC_INTERNAL_argAddrVec[14];
			assert(15 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p14 = LCACC_INTERNAL_argAddrVec[15];
			assert(16 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p15 = LCACC_INTERNAL_argAddrVec[16];
			assert(17 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p16 = LCACC_INTERNAL_argAddrVec[17];
			assert(18 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p17 = LCACC_INTERNAL_argAddrVec[18];
			assert(19 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p18 = LCACC_INTERNAL_argAddrVec[19];
			assert(20 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p19 = LCACC_INTERNAL_argAddrVec[20];
			assert(21 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p20 = LCACC_INTERNAL_argAddrVec[21];
			assert(22 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p21 = LCACC_INTERNAL_argAddrVec[22];
			assert(23 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p22 = LCACC_INTERNAL_argAddrVec[23];
			assert(24 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p23 = LCACC_INTERNAL_argAddrVec[24];
			assert(25 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p24 = LCACC_INTERNAL_argAddrVec[25];
			assert(26 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p25 = LCACC_INTERNAL_argAddrVec[26];
			assert(27 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p26 = LCACC_INTERNAL_argAddrVec[27];
			assert(28 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p27 = LCACC_INTERNAL_argAddrVec[28];
			assert(29 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p28 = LCACC_INTERNAL_argAddrVec[29];
			assert(30 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p29 = LCACC_INTERNAL_argAddrVec[30];
			assert(31 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p30 = LCACC_INTERNAL_argAddrVec[31];
			assert(32 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p31 = LCACC_INTERNAL_argAddrVec[32];
			assert(33 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p32 = LCACC_INTERNAL_argAddrVec[33];
			assert(34 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p33 = LCACC_INTERNAL_argAddrVec[34];
			assert(35 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p34 = LCACC_INTERNAL_argAddrVec[35];
			assert(36 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p35 = LCACC_INTERNAL_argAddrVec[36];
			assert(37 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p36 = LCACC_INTERNAL_argAddrVec[37];
			assert(38 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p37 = LCACC_INTERNAL_argAddrVec[38];
			assert(39 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p38 = LCACC_INTERNAL_argAddrVec[39];
			assert(40 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p39 = LCACC_INTERNAL_argAddrVec[40];
			assert(41 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p40 = LCACC_INTERNAL_argAddrVec[41];
			assert(42 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p41 = LCACC_INTERNAL_argAddrVec[42];
			assert(43 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p42 = LCACC_INTERNAL_argAddrVec[43];
			assert(44 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p43 = LCACC_INTERNAL_argAddrVec[44];
			assert(45 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p44 = LCACC_INTERNAL_argAddrVec[45];
			assert(46 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p45 = LCACC_INTERNAL_argAddrVec[46];
			assert(47 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p46 = LCACC_INTERNAL_argAddrVec[47];
			assert(48 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p47 = LCACC_INTERNAL_argAddrVec[48];
			assert(49 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p48 = LCACC_INTERNAL_argAddrVec[49];
			assert(50 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p49 = LCACC_INTERNAL_argAddrVec[50];
			assert(51 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p50 = LCACC_INTERNAL_argAddrVec[51];
			assert(52 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p51 = LCACC_INTERNAL_argAddrVec[52];
			assert(53 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p52 = LCACC_INTERNAL_argAddrVec[53];
			assert(54 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p53 = LCACC_INTERNAL_argAddrVec[54];
			assert(55 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p54 = LCACC_INTERNAL_argAddrVec[55];
			assert(56 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p55 = LCACC_INTERNAL_argAddrVec[56];
			assert(57 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p56 = LCACC_INTERNAL_argAddrVec[57];
			assert(58 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p57 = LCACC_INTERNAL_argAddrVec[58];
			assert(59 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p58 = LCACC_INTERNAL_argAddrVec[59];
			assert(60 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p59 = LCACC_INTERNAL_argAddrVec[60];
			assert(61 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p60 = LCACC_INTERNAL_argAddrVec[61];
			assert(62 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p61 = LCACC_INTERNAL_argAddrVec[62];
			assert(63 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p62 = LCACC_INTERNAL_argAddrVec[63];
			assert(64 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_p63 = LCACC_INTERNAL_argAddrVec[64];
			assert(65 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_dist = LCACC_INTERNAL_argAddrVec[65];

			double weight;
			double p0;
			double p1;
			double p2;
			double p3;
			double p4;
			double p5;
			double p6;
			double p7;
			double p8;
			double p9;
			double p10;
			double p11;
			double p12;
			double p13;
			double p14;
			double p15;
			double p16;
			double p17;
			double p18;
			double p19;
			double p20;
			double p21;
			double p22;
			double p23;
			double p24;
			double p25;
			double p26;
			double p27;
			double p28;
			double p29;
			double p30;
			double p31;
			double p32;
			double p33;
			double p34;
			double p35;
			double p36;
			double p37;
			double p38;
			double p39;
			double p40;
			double p41;
			double p42;
			double p43;
			double p44;
			double p45;
			double p46;
			double p47;
			double p48;
			double p49;
			double p50;
			double p51;
			double p52;
			double p53;
			double p54;
			double p55;
			double p56;
			double p57;
			double p58;
			double p59;
			double p60;
			double p61;
			double p62;
			double p63;
			double dist;
			weight = 0;
			p0 = 0;
			p1 = 0;
			p2 = 0;
			p3 = 0;
			p4 = 0;
			p5 = 0;
			p6 = 0;
			p7 = 0;
			p8 = 0;
			p9 = 0;
			p10 = 0;
			p11 = 0;
			p12 = 0;
			p13 = 0;
			p14 = 0;
			p15 = 0;
			p16 = 0;
			p17 = 0;
			p18 = 0;
			p19 = 0;
			p20 = 0;
			p21 = 0;
			p22 = 0;
			p23 = 0;
			p24 = 0;
			p25 = 0;
			p26 = 0;
			p27 = 0;
			p28 = 0;
			p29 = 0;
			p30 = 0;
			p31 = 0;
			p32 = 0;
			p33 = 0;
			p34 = 0;
			p35 = 0;
			p36 = 0;
			p37 = 0;
			p38 = 0;
			p39 = 0;
			p40 = 0;
			p41 = 0;
			p42 = 0;
			p43 = 0;
			p44 = 0;
			p45 = 0;
			p46 = 0;
			p47 = 0;
			p48 = 0;
			p49 = 0;
			p50 = 0;
			p51 = 0;
			p52 = 0;
			p53 = 0;
			p54 = 0;
			p55 = 0;
			p56 = 0;
			p57 = 0;
			p58 = 0;
			p59 = 0;
			p60 = 0;
			p61 = 0;
			p62 = 0;
			p63 = 0;
			dist = 0;

			weight = ReadSPMFlt(0, addr_weight, 0);
			p0 = ReadSPMFlt(1, addr_p0, 0);
			p1 = ReadSPMFlt(2, addr_p1, 0);
			p2 = ReadSPMFlt(3, addr_p2, 0);
			p3 = ReadSPMFlt(4, addr_p3, 0);
			p4 = ReadSPMFlt(5, addr_p4, 0);
			p5 = ReadSPMFlt(6, addr_p5, 0);
			p6 = ReadSPMFlt(7, addr_p6, 0);
			p7 = ReadSPMFlt(8, addr_p7, 0);
			p8 = ReadSPMFlt(9, addr_p8, 0);
			p9 = ReadSPMFlt(10, addr_p9, 0);
			p10 = ReadSPMFlt(11, addr_p10, 0);
			p11 = ReadSPMFlt(12, addr_p11, 0);
			p12 = ReadSPMFlt(13, addr_p12, 0);
			p13 = ReadSPMFlt(14, addr_p13, 0);
			p14 = ReadSPMFlt(15, addr_p14, 0);
			p15 = ReadSPMFlt(16, addr_p15, 0);
			p16 = ReadSPMFlt(17, addr_p16, 0);
			p17 = ReadSPMFlt(18, addr_p17, 0);
			p18 = ReadSPMFlt(19, addr_p18, 0);
			p19 = ReadSPMFlt(20, addr_p19, 0);
			p20 = ReadSPMFlt(21, addr_p20, 0);
			p21 = ReadSPMFlt(22, addr_p21, 0);
			p22 = ReadSPMFlt(23, addr_p22, 0);
			p23 = ReadSPMFlt(24, addr_p23, 0);
			p24 = ReadSPMFlt(25, addr_p24, 0);
			p25 = ReadSPMFlt(26, addr_p25, 0);
			p26 = ReadSPMFlt(27, addr_p26, 0);
			p27 = ReadSPMFlt(28, addr_p27, 0);
			p28 = ReadSPMFlt(29, addr_p28, 0);
			p29 = ReadSPMFlt(30, addr_p29, 0);
			p30 = ReadSPMFlt(31, addr_p30, 0);
			p31 = ReadSPMFlt(32, addr_p31, 0);
			p32 = ReadSPMFlt(33, addr_p32, 0);
			p33 = ReadSPMFlt(34, addr_p33, 0);
			p34 = ReadSPMFlt(35, addr_p34, 0);
			p35 = ReadSPMFlt(36, addr_p35, 0);
			p36 = ReadSPMFlt(37, addr_p36, 0);
			p37 = ReadSPMFlt(38, addr_p37, 0);
			p38 = ReadSPMFlt(39, addr_p38, 0);
			p39 = ReadSPMFlt(40, addr_p39, 0);
			p40 = ReadSPMFlt(41, addr_p40, 0);
			p41 = ReadSPMFlt(42, addr_p41, 0);
			p42 = ReadSPMFlt(43, addr_p42, 0);
			p43 = ReadSPMFlt(44, addr_p43, 0);
			p44 = ReadSPMFlt(45, addr_p44, 0);
			p45 = ReadSPMFlt(46, addr_p45, 0);
			p46 = ReadSPMFlt(47, addr_p46, 0);
			p47 = ReadSPMFlt(48, addr_p47, 0);
			p48 = ReadSPMFlt(49, addr_p48, 0);
			p49 = ReadSPMFlt(50, addr_p49, 0);
			p50 = ReadSPMFlt(51, addr_p50, 0);
			p51 = ReadSPMFlt(52, addr_p51, 0);
			p52 = ReadSPMFlt(53, addr_p52, 0);
			p53 = ReadSPMFlt(54, addr_p53, 0);
			p54 = ReadSPMFlt(55, addr_p54, 0);
			p55 = ReadSPMFlt(56, addr_p55, 0);
			p56 = ReadSPMFlt(57, addr_p56, 0);
			p57 = ReadSPMFlt(58, addr_p57, 0);
			p58 = ReadSPMFlt(59, addr_p58, 0);
			p59 = ReadSPMFlt(60, addr_p59, 0);
			p60 = ReadSPMFlt(61, addr_p60, 0);
			p61 = ReadSPMFlt(62, addr_p61, 0);
			p62 = ReadSPMFlt(63, addr_p62, 0);
			p63 = ReadSPMFlt(64, addr_p63, 0);

			#define SPMAddressOf(x) (addr_##x)
			#define DIFF_X(y) (p##y - px##y) * (p##y - px##y)

			float x_cost = DIFF_X(0) + DIFF_X(1) + DIFF_X(2) + DIFF_X(3) + DIFF_X(4) + DIFF_X(5) + DIFF_X(6) + DIFF_X(7) + DIFF_X(8) + DIFF_X(10) + DIFF_X(11) + DIFF_X(12) + DIFF_X(13) + DIFF_X(14) + DIFF_X(15) + DIFF_X(16) + DIFF_X(17) + DIFF_X(18) + DIFF_X(19) + DIFF_X(20) + DIFF_X(21) + DIFF_X(22) + DIFF_X(23) + DIFF_X(24) + DIFF_X(25) + DIFF_X(26) + DIFF_X(27) + DIFF_X(28) + DIFF_X(29) + DIFF_X(30) + DIFF_X(31) + DIFF_X(32) + DIFF_X(33) + DIFF_X(34) + DIFF_X(35) + DIFF_X(36) + DIFF_X(37) + DIFF_X(38) + DIFF_X(39) + DIFF_X(40) + DIFF_X(41) + DIFF_X(42) + DIFF_X(43) + DIFF_X(44) + DIFF_X(45) + DIFF_X(46) + DIFF_X(47) + DIFF_X(48) + DIFF_X(49) + DIFF_X(50) + DIFF_X(51) + DIFF_X(52) + DIFF_X(53) + DIFF_X(54) + DIFF_X(55) + DIFF_X(56) + DIFF_X(57) + DIFF_X(58) + DIFF_X(59) + DIFF_X(60) + DIFF_X(61) + DIFF_X(62) + DIFF_X(63);

			x_cost *= weight;

			#undef DIFF_X
			#undef SPMAddressOf

			WriteSPMFlt(65, addr_dist, 0, dist);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 32;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "streamCluster2_dimm";}
		inline virtual int ArgumentCount(){return 66;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 64);
			px0 = ConvertTypes<uint32_t, float>(regs[0]);
			px1 = ConvertTypes<uint32_t, float>(regs[1]);
			px2 = ConvertTypes<uint32_t, float>(regs[2]);
			px3 = ConvertTypes<uint32_t, float>(regs[3]);
			px4 = ConvertTypes<uint32_t, float>(regs[4]);
			px5 = ConvertTypes<uint32_t, float>(regs[5]);
			px6 = ConvertTypes<uint32_t, float>(regs[6]);
			px7 = ConvertTypes<uint32_t, float>(regs[7]);
			px8 = ConvertTypes<uint32_t, float>(regs[8]);
			px9 = ConvertTypes<uint32_t, float>(regs[9]);
			px10 = ConvertTypes<uint32_t, float>(regs[10]);
			px11 = ConvertTypes<uint32_t, float>(regs[11]);
			px12 = ConvertTypes<uint32_t, float>(regs[12]);
			px13 = ConvertTypes<uint32_t, float>(regs[13]);
			px14 = ConvertTypes<uint32_t, float>(regs[14]);
			px15 = ConvertTypes<uint32_t, float>(regs[15]);
			px16 = ConvertTypes<uint32_t, float>(regs[16]);
			px17 = ConvertTypes<uint32_t, float>(regs[17]);
			px18 = ConvertTypes<uint32_t, float>(regs[18]);
			px19 = ConvertTypes<uint32_t, float>(regs[19]);
			px20 = ConvertTypes<uint32_t, float>(regs[20]);
			px21 = ConvertTypes<uint32_t, float>(regs[21]);
			px22 = ConvertTypes<uint32_t, float>(regs[22]);
			px23 = ConvertTypes<uint32_t, float>(regs[23]);
			px24 = ConvertTypes<uint32_t, float>(regs[24]);
			px25 = ConvertTypes<uint32_t, float>(regs[25]);
			px26 = ConvertTypes<uint32_t, float>(regs[26]);
			px27 = ConvertTypes<uint32_t, float>(regs[27]);
			px28 = ConvertTypes<uint32_t, float>(regs[28]);
			px29 = ConvertTypes<uint32_t, float>(regs[29]);
			px30 = ConvertTypes<uint32_t, float>(regs[30]);
			px31 = ConvertTypes<uint32_t, float>(regs[31]);
			px32 = ConvertTypes<uint32_t, float>(regs[32]);
			px33 = ConvertTypes<uint32_t, float>(regs[33]);
			px34 = ConvertTypes<uint32_t, float>(regs[34]);
			px35 = ConvertTypes<uint32_t, float>(regs[35]);
			px36 = ConvertTypes<uint32_t, float>(regs[36]);
			px37 = ConvertTypes<uint32_t, float>(regs[37]);
			px38 = ConvertTypes<uint32_t, float>(regs[38]);
			px39 = ConvertTypes<uint32_t, float>(regs[39]);
			px40 = ConvertTypes<uint32_t, float>(regs[40]);
			px41 = ConvertTypes<uint32_t, float>(regs[41]);
			px42 = ConvertTypes<uint32_t, float>(regs[42]);
			px43 = ConvertTypes<uint32_t, float>(regs[43]);
			px44 = ConvertTypes<uint32_t, float>(regs[44]);
			px45 = ConvertTypes<uint32_t, float>(regs[45]);
			px46 = ConvertTypes<uint32_t, float>(regs[46]);
			px47 = ConvertTypes<uint32_t, float>(regs[47]);
			px48 = ConvertTypes<uint32_t, float>(regs[48]);
			px49 = ConvertTypes<uint32_t, float>(regs[49]);
			px50 = ConvertTypes<uint32_t, float>(regs[50]);
			px51 = ConvertTypes<uint32_t, float>(regs[51]);
			px52 = ConvertTypes<uint32_t, float>(regs[52]);
			px53 = ConvertTypes<uint32_t, float>(regs[53]);
			px54 = ConvertTypes<uint32_t, float>(regs[54]);
			px55 = ConvertTypes<uint32_t, float>(regs[55]);
			px56 = ConvertTypes<uint32_t, float>(regs[56]);
			px57 = ConvertTypes<uint32_t, float>(regs[57]);
			px58 = ConvertTypes<uint32_t, float>(regs[58]);
			px59 = ConvertTypes<uint32_t, float>(regs[59]);
			px60 = ConvertTypes<uint32_t, float>(regs[60]);
			px61 = ConvertTypes<uint32_t, float>(regs[61]);
			px62 = ConvertTypes<uint32_t, float>(regs[62]);
			px63 = ConvertTypes<uint32_t, float>(regs[63]);
		}
		inline static int GetOpCode(){return 1052;}
	};
}

#endif
