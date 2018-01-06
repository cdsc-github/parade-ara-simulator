#ifndef LCACC_MODE_FLUIDANIMATE1_H
#define LCACC_MODE_FLUIDANIMATE1_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_fluidAnimate1 : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_fluidAnimate1(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_cellPosX = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_cellPosY = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_cellPosZ = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_neighbor1PosX = argAddrVec[3];
			assert(4 < argAddrVec.size());
			uint64_t addr_neighbor1PosY = argAddrVec[4];
			assert(5 < argAddrVec.size());
			uint64_t addr_neighbor1PosZ = argAddrVec[5];
			assert(6 < argAddrVec.size());
			uint64_t addr_neighbor2PosX = argAddrVec[6];
			assert(7 < argAddrVec.size());
			uint64_t addr_neighbor2PosY = argAddrVec[7];
			assert(8 < argAddrVec.size());
			uint64_t addr_neighbor2PosZ = argAddrVec[8];
			assert(9 < argAddrVec.size());
			uint64_t addr_neighbor3PosX = argAddrVec[9];
			assert(10 < argAddrVec.size());
			uint64_t addr_neighbor3PosY = argAddrVec[10];
			assert(11 < argAddrVec.size());
			uint64_t addr_neighbor3PosZ = argAddrVec[11];
			assert(12 < argAddrVec.size());
			uint64_t addr_neighbor4PosX = argAddrVec[12];
			assert(13 < argAddrVec.size());
			uint64_t addr_neighbor4PosY = argAddrVec[13];
			assert(14 < argAddrVec.size());
			uint64_t addr_neighbor4PosZ = argAddrVec[14];
			assert(15 < argAddrVec.size());
			uint64_t addr_neighbor5PosX = argAddrVec[15];
			assert(16 < argAddrVec.size());
			uint64_t addr_neighbor5PosY = argAddrVec[16];
			assert(17 < argAddrVec.size());
			uint64_t addr_neighbor5PosZ = argAddrVec[17];
			assert(18 < argAddrVec.size());
			uint64_t addr_neighbor6PosX = argAddrVec[18];
			assert(19 < argAddrVec.size());
			uint64_t addr_neighbor6PosY = argAddrVec[19];
			assert(20 < argAddrVec.size());
			uint64_t addr_neighbor6PosZ = argAddrVec[20];
			assert(21 < argAddrVec.size());
			uint64_t addr_neighbor7PosX = argAddrVec[21];
			assert(22 < argAddrVec.size());
			uint64_t addr_neighbor7PosY = argAddrVec[22];
			assert(23 < argAddrVec.size());
			uint64_t addr_neighbor7PosZ = argAddrVec[23];
			assert(24 < argAddrVec.size());
			uint64_t addr_neighbor8PosX = argAddrVec[24];
			assert(25 < argAddrVec.size());
			uint64_t addr_neighbor8PosY = argAddrVec[25];
			assert(26 < argAddrVec.size());
			uint64_t addr_neighbor8PosZ = argAddrVec[26];
			assert(27 < argAddrVec.size());
			uint64_t addr_neighbor9PosX = argAddrVec[27];
			assert(28 < argAddrVec.size());
			uint64_t addr_neighbor9PosY = argAddrVec[28];
			assert(29 < argAddrVec.size());
			uint64_t addr_neighbor9PosZ = argAddrVec[29];

			if(argActive[0])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(0); i += GetArgumentWidth(0))
				{
					outputArgs.push_back(addr_cellPosX + i);
				}
			}
			if(argActive[1])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(1); i += GetArgumentWidth(1))
				{
					outputArgs.push_back(addr_cellPosY + i);
				}
			}
			if(argActive[2])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(2); i += GetArgumentWidth(2))
				{
					outputArgs.push_back(addr_cellPosZ + i);
				}
			}
			if(argActive[3])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(3); i += GetArgumentWidth(3))
				{
					outputArgs.push_back(addr_neighbor1PosX + i);
				}
			}
			if(argActive[4])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(4); i += GetArgumentWidth(4))
				{
					outputArgs.push_back(addr_neighbor1PosY + i);
				}
			}
			if(argActive[5])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(5); i += GetArgumentWidth(5))
				{
					outputArgs.push_back(addr_neighbor1PosZ + i);
				}
			}
			if(argActive[6])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(6); i += GetArgumentWidth(6))
				{
					outputArgs.push_back(addr_neighbor2PosX + i);
				}
			}
			if(argActive[7])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(7); i += GetArgumentWidth(7))
				{
					outputArgs.push_back(addr_neighbor2PosY + i);
				}
			}
			if(argActive[8])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(8); i += GetArgumentWidth(8))
				{
					outputArgs.push_back(addr_neighbor2PosZ + i);
				}
			}
			if(argActive[9])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(9); i += GetArgumentWidth(9))
				{
					outputArgs.push_back(addr_neighbor3PosX + i);
				}
			}
			if(argActive[10])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(10); i += GetArgumentWidth(10))
				{
					outputArgs.push_back(addr_neighbor3PosY + i);
				}
			}
			if(argActive[11])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(11); i += GetArgumentWidth(11))
				{
					outputArgs.push_back(addr_neighbor3PosZ + i);
				}
			}
			if(argActive[12])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(12); i += GetArgumentWidth(12))
				{
					outputArgs.push_back(addr_neighbor4PosX + i);
				}
			}
			if(argActive[13])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(13); i += GetArgumentWidth(13))
				{
					outputArgs.push_back(addr_neighbor4PosY + i);
				}
			}
			if(argActive[14])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(14); i += GetArgumentWidth(14))
				{
					outputArgs.push_back(addr_neighbor4PosZ + i);
				}
			}
			if(argActive[15])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(15); i += GetArgumentWidth(15))
				{
					outputArgs.push_back(addr_neighbor5PosX + i);
				}
			}
			if(argActive[16])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(16); i += GetArgumentWidth(16))
				{
					outputArgs.push_back(addr_neighbor5PosY + i);
				}
			}
			if(argActive[17])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(17); i += GetArgumentWidth(17))
				{
					outputArgs.push_back(addr_neighbor5PosZ + i);
				}
			}
			if(argActive[18])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(18); i += GetArgumentWidth(18))
				{
					outputArgs.push_back(addr_neighbor6PosX + i);
				}
			}
			if(argActive[19])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(19); i += GetArgumentWidth(19))
				{
					outputArgs.push_back(addr_neighbor6PosY + i);
				}
			}
			if(argActive[20])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(20); i += GetArgumentWidth(20))
				{
					outputArgs.push_back(addr_neighbor6PosZ + i);
				}
			}
			if(argActive[21])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(21); i += GetArgumentWidth(21))
				{
					outputArgs.push_back(addr_neighbor7PosX + i);
				}
			}
			if(argActive[22])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(22); i += GetArgumentWidth(22))
				{
					outputArgs.push_back(addr_neighbor7PosY + i);
				}
			}
			if(argActive[23])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(23); i += GetArgumentWidth(23))
				{
					outputArgs.push_back(addr_neighbor7PosZ + i);
				}
			}
			if(argActive[24])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(24); i += GetArgumentWidth(24))
				{
					outputArgs.push_back(addr_neighbor8PosX + i);
				}
			}
			if(argActive[25])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(25); i += GetArgumentWidth(25))
				{
					outputArgs.push_back(addr_neighbor8PosY + i);
				}
			}
			if(argActive[26])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(26); i += GetArgumentWidth(26))
				{
					outputArgs.push_back(addr_neighbor8PosZ + i);
				}
			}
			if(argActive[27])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(27); i += GetArgumentWidth(27))
				{
					outputArgs.push_back(addr_neighbor9PosX + i);
				}
			}
			if(argActive[28])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(28); i += GetArgumentWidth(28))
				{
					outputArgs.push_back(addr_neighbor9PosY + i);
				}
			}
			if(argActive[29])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(29); i += GetArgumentWidth(29))
				{
					outputArgs.push_back(addr_neighbor9PosZ + i);
				}
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(30 < argAddrVec.size());
			uint64_t addr_density = argAddrVec[30];

			if(argActive[30])
			{
				for(size_t i = 0; i < ((16)) * GetArgumentWidth(30); i += GetArgumentWidth(30))
				{
					outputArgs.push_back(addr_density + i);
				}
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 31);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_cellPosX = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_cellPosY = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_cellPosZ = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor1PosX = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor1PosY = LCACC_INTERNAL_argAddrVec[4];
			assert(5 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor1PosZ = LCACC_INTERNAL_argAddrVec[5];
			assert(6 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor2PosX = LCACC_INTERNAL_argAddrVec[6];
			assert(7 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor2PosY = LCACC_INTERNAL_argAddrVec[7];
			assert(8 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor2PosZ = LCACC_INTERNAL_argAddrVec[8];
			assert(9 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor3PosX = LCACC_INTERNAL_argAddrVec[9];
			assert(10 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor3PosY = LCACC_INTERNAL_argAddrVec[10];
			assert(11 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor3PosZ = LCACC_INTERNAL_argAddrVec[11];
			assert(12 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor4PosX = LCACC_INTERNAL_argAddrVec[12];
			assert(13 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor4PosY = LCACC_INTERNAL_argAddrVec[13];
			assert(14 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor4PosZ = LCACC_INTERNAL_argAddrVec[14];
			assert(15 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor5PosX = LCACC_INTERNAL_argAddrVec[15];
			assert(16 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor5PosY = LCACC_INTERNAL_argAddrVec[16];
			assert(17 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor5PosZ = LCACC_INTERNAL_argAddrVec[17];
			assert(18 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor6PosX = LCACC_INTERNAL_argAddrVec[18];
			assert(19 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor6PosY = LCACC_INTERNAL_argAddrVec[19];
			assert(20 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor6PosZ = LCACC_INTERNAL_argAddrVec[20];
			assert(21 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor7PosX = LCACC_INTERNAL_argAddrVec[21];
			assert(22 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor7PosY = LCACC_INTERNAL_argAddrVec[22];
			assert(23 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor7PosZ = LCACC_INTERNAL_argAddrVec[23];
			assert(24 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor8PosX = LCACC_INTERNAL_argAddrVec[24];
			assert(25 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor8PosY = LCACC_INTERNAL_argAddrVec[25];
			assert(26 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor8PosZ = LCACC_INTERNAL_argAddrVec[26];
			assert(27 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor9PosX = LCACC_INTERNAL_argAddrVec[27];
			assert(28 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor9PosY = LCACC_INTERNAL_argAddrVec[28];
			assert(29 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_neighbor9PosZ = LCACC_INTERNAL_argAddrVec[29];
			assert(30 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_density = LCACC_INTERNAL_argAddrVec[30];

			double cellPosX[16];
			double cellPosY[16];
			double cellPosZ[16];
			double neighbor1PosX[16];
			double neighbor1PosY[16];
			double neighbor1PosZ[16];
			double neighbor2PosX[16];
			double neighbor2PosY[16];
			double neighbor2PosZ[16];
			double neighbor3PosX[16];
			double neighbor3PosY[16];
			double neighbor3PosZ[16];
			double neighbor4PosX[16];
			double neighbor4PosY[16];
			double neighbor4PosZ[16];
			double neighbor5PosX[16];
			double neighbor5PosY[16];
			double neighbor5PosZ[16];
			double neighbor6PosX[16];
			double neighbor6PosY[16];
			double neighbor6PosZ[16];
			double neighbor7PosX[16];
			double neighbor7PosY[16];
			double neighbor7PosZ[16];
			double neighbor8PosX[16];
			double neighbor8PosY[16];
			double neighbor8PosZ[16];
			double neighbor9PosX[16];
			double neighbor9PosY[16];
			double neighbor9PosZ[16];
			double density[16];
			for(int i = 0; i < ((16)); i++)
			{
				cellPosX[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				cellPosY[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				cellPosZ[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor1PosX[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor1PosY[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor1PosZ[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor2PosX[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor2PosY[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor2PosZ[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor3PosX[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor3PosY[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor3PosZ[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor4PosX[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor4PosY[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor4PosZ[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor5PosX[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor5PosY[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor5PosZ[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor6PosX[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor6PosY[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor6PosZ[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor7PosX[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor7PosY[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor7PosZ[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor8PosX[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor8PosY[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor8PosZ[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor9PosX[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor9PosY[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				neighbor9PosZ[(i) % (16)] = (double)0;
			}
			for(int i = 0; i < ((16)); i++)
			{
				density[(i) % (16)] = (double)0;
			}

			for(size_t i = 0; i < ((16)); i++)
			{
				cellPosX[(i) % (16)] = ReadSPMFlt(0, addr_cellPosX, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				cellPosY[(i) % (16)] = ReadSPMFlt(1, addr_cellPosY, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				cellPosZ[(i) % (16)] = ReadSPMFlt(2, addr_cellPosZ, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor1PosX[(i) % (16)] = ReadSPMFlt(3, addr_neighbor1PosX, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor1PosY[(i) % (16)] = ReadSPMFlt(4, addr_neighbor1PosY, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor1PosZ[(i) % (16)] = ReadSPMFlt(5, addr_neighbor1PosZ, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor2PosX[(i) % (16)] = ReadSPMFlt(6, addr_neighbor2PosX, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor2PosY[(i) % (16)] = ReadSPMFlt(7, addr_neighbor2PosY, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor2PosZ[(i) % (16)] = ReadSPMFlt(8, addr_neighbor2PosZ, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor3PosX[(i) % (16)] = ReadSPMFlt(9, addr_neighbor3PosX, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor3PosY[(i) % (16)] = ReadSPMFlt(10, addr_neighbor3PosY, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor3PosZ[(i) % (16)] = ReadSPMFlt(11, addr_neighbor3PosZ, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor4PosX[(i) % (16)] = ReadSPMFlt(12, addr_neighbor4PosX, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor4PosY[(i) % (16)] = ReadSPMFlt(13, addr_neighbor4PosY, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor4PosZ[(i) % (16)] = ReadSPMFlt(14, addr_neighbor4PosZ, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor5PosX[(i) % (16)] = ReadSPMFlt(15, addr_neighbor5PosX, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor5PosY[(i) % (16)] = ReadSPMFlt(16, addr_neighbor5PosY, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor5PosZ[(i) % (16)] = ReadSPMFlt(17, addr_neighbor5PosZ, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor6PosX[(i) % (16)] = ReadSPMFlt(18, addr_neighbor6PosX, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor6PosY[(i) % (16)] = ReadSPMFlt(19, addr_neighbor6PosY, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor6PosZ[(i) % (16)] = ReadSPMFlt(20, addr_neighbor6PosZ, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor7PosX[(i) % (16)] = ReadSPMFlt(21, addr_neighbor7PosX, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor7PosY[(i) % (16)] = ReadSPMFlt(22, addr_neighbor7PosY, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor7PosZ[(i) % (16)] = ReadSPMFlt(23, addr_neighbor7PosZ, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor8PosX[(i) % (16)] = ReadSPMFlt(24, addr_neighbor8PosX, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor8PosY[(i) % (16)] = ReadSPMFlt(25, addr_neighbor8PosY, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor8PosZ[(i) % (16)] = ReadSPMFlt(26, addr_neighbor8PosZ, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor9PosX[(i) % (16)] = ReadSPMFlt(27, addr_neighbor9PosX, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor9PosY[(i) % (16)] = ReadSPMFlt(28, addr_neighbor9PosY, i);
			}
			for(size_t i = 0; i < ((16)); i++)
			{
				neighbor9PosZ[(i) % (16)] = ReadSPMFlt(29, addr_neighbor9PosZ, i);
			}

			#define SPMAddressOf(x) (addr_##x)
			//	const float hSqBias = hSq*hSq*hSq;
			//	int cindex = 0;
			//	for(int ck = 0; ck < nz; ++ck)
			//	{
			//		for(int cj = 0; cj < ny; ++cj)
			//		{
			//			for(int ci = 0; ci < nx; ++ci, ++cindex)
			//			{
			//				int numPars = cnumPars[cindex];
			//				if(numPars == 0)
			//					continue;
			//
			//				Cell &cell = cells[cindex];
			//
			//				for(int ipar = 0; ipar < numPars; ++ipar)
			//				{
			//					cell.density[ipar] = 0.f;
			//					for(int inc = 0; inc < numNeighCells; ++inc)
			//					{
			//						int cindexNeigh = neighCells[inc];
			//						Cell &neigh = cells[cindexNeigh];
			//						int numNeighPars = cnumPars[cindexNeigh];
			//						for(int iparNeigh = 0; iparNeigh < numNeighPars; ++iparNeigh)
			//						{
			//							float distSq = (cell.p[ipar] - neigh.p[iparNeigh]).GetLengthSq();
			//							if(distSq < hSq)
			//							{
			//								float t = hSq - distSq;
			//								float tc = t*t*t;
			//								cell.density[ipar] += tc;
			//							}
			//						}
			//					}
			//					cell.density[ipar] += hSqBias;
			//					cell.density[ipar] *= densityCoeff;
			//				}
			//			}
			//		}
			//	}
			#undef SPMAddressOf

			for(size_t i = 0; i < ((16)); i++)
			{
				WriteSPMFlt(30, addr_density, i, density[(i) % (16)]);
			}
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 256;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "fluidAnimate1";}
		inline virtual int ArgumentCount(){return 31;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 1101;}
	};
}

#endif
