#ifndef LCACC_MODE_BLACKSCHOLES_H
#define LCACC_MODE_BLACKSCHOLES_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_blackScholes : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_blackScholes(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_sptprice = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_strike = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_rate = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_volatility = argAddrVec[3];
			assert(4 < argAddrVec.size());
			uint64_t addr_time = argAddrVec[4];
			assert(5 < argAddrVec.size());
			uint64_t addr_otype = argAddrVec[5];

			if(argActive[0])
			{
				outputArgs.push_back(addr_sptprice);
			}
			if(argActive[1])
			{
				outputArgs.push_back(addr_strike);
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_rate);
			}
			if(argActive[3])
			{
				outputArgs.push_back(addr_volatility);
			}
			if(argActive[4])
			{
				outputArgs.push_back(addr_time);
			}
			if(argActive[5])
			{
				outputArgs.push_back(addr_otype);
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(6 < argAddrVec.size());
			uint64_t addr_output = argAddrVec[6];

			if(argActive[6])
			{
				outputArgs.push_back(addr_output);
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 7);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_sptprice = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_strike = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_rate = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_volatility = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_time = LCACC_INTERNAL_argAddrVec[4];
			assert(5 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_otype = LCACC_INTERNAL_argAddrVec[5];
			assert(6 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output = LCACC_INTERNAL_argAddrVec[6];

			double sptprice;
			double strike;
			double rate;
			double volatility;
			double time;
			int64_t otype;
			double output;
			sptprice = 0;
			strike = 0;
			rate = 0;
			volatility = 0;
			time = 0;
			otype = 0;
			output = 0;

			sptprice = ReadSPMFlt(0, addr_sptprice, 0);
			strike = ReadSPMFlt(1, addr_strike, 0);
			rate = ReadSPMFlt(2, addr_rate, 0);
			volatility = ReadSPMFlt(3, addr_volatility, 0);
			time = ReadSPMFlt(4, addr_time, 0);
			otype = ReadSPMInt(5, addr_otype, 0, true);

			#define SPMAddressOf(x) (addr_##x)
			float inv_sqrt_2xPI = 0.0053f;
			#define CNDF(InputX, output) \
			{ \
			    int sign; \
			    float InputX1;\
			    float OutputX;\
			    float OutputX2;\
			    float xInput;\
			    float xNPrimeofX;\
			    float expValues;\
			    float xK2;\
			    float xK2_2, xK2_3;\
			    float xK2_4, xK2_5;\
			    float xLocal, xLocal_1;\
			    float xLocal_2, xLocal_3;\
			\
			    InputX1 = -InputX;\
			    InputX = (InputX < 0.0f) ? InputX : InputX1;\
			    sign = (InputX < 0.0f) ? 0 : 1;\
			\
			    xInput = InputX;\
			\
			    expValues = expf(-0.5f * InputX * InputX);\
			    xNPrimeofX = expValues;\
			    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;\
			\
			    xK2 = 0.2316419f * xInput;\
			    xK2 = 1.0f + xK2;\
			    xK2 = 1.0f / xK2;\
			    xK2_2 = xK2 * xK2;\
			    xK2_3 = xK2_2 * xK2;\
			    xK2_4 = xK2_3 * xK2;\
			    xK2_5 = xK2_4 * xK2;\
			\
			    xLocal_1 = xK2 * 0.319381530f;\
			    xLocal_2 = xK2_2 * (-0.356563782f);\
			    xLocal_3 = xK2_3 * 1.781477937f;\
			    xLocal_2 = xLocal_2 + xLocal_3;\
			    xLocal_3 = xK2_4 * (-1.821255978f);\
			    xLocal_2 = xLocal_2 + xLocal_3;\
			    xLocal_3 = xK2_5 * 1.330274429f;\
			    xLocal_2 = xLocal_2 + xLocal_3;\
			\
			    xLocal_1 = xLocal_2 + xLocal_1;\
			    xLocal   = xLocal_1 * xNPrimeofX;\
			    xLocal   = 1.0f - xLocal;\
			\
			    OutputX  = xLocal;\
			    OutputX2 = 1.0f - OutputX;\
			    OutputX = (sign < 1) ? OutputX2 : OutputX;\
			\
			    output = OutputX;\
			}

			    float OptionPrice;
			    float OptionPrice2;

			    float xStockPrice;
			    float xStrikePrice;
			    float xRiskFreeRate;
			    float xVolatility;
			    float xTime;
			    float xSqrtTime;

			    float logValues;
			    float xLogTerm;
			    float xD1;
			    float xD2;
			    float xPowerTerm;
			    float xDen;
			    float d1;
			    float d2;
			    float FutureValueX;
			    float NofXd1;
			    float NofXd2;
			    float NegNofXd1;
			    float NegNofXd2;

			    xStockPrice = sptprice;
			    xStrikePrice = strike;
			    xRiskFreeRate = rate;
			    xVolatility = volatility;

			    xTime = time;
			    xSqrtTime = sqrt(xTime);

			    logValues = log( sptprice / strike );

			    xLogTerm = logValues;

			    xPowerTerm = xVolatility * xVolatility;
			    xPowerTerm = xPowerTerm * 0.5f;

			    xD1 = xRiskFreeRate + xPowerTerm;
			    xD1 = xD1 * xTime;
			    xD1 = xD1 + xLogTerm;

			    xDen = xVolatility * xSqrtTime;
			    xD1 = xD1 / xDen;
			    xD2 = xD1 -  xDen;
			   d1 = xD1;
			    d2 = xD2;

			    CNDF( d1, NofXd1);
			    CNDF( d2, NofXd2);

			    FutureValueX = strike * ( expf( -(rate)*(time) ) );
			        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
			     NegNofXd1 = (1.0f - NofXd1);
			        NegNofXd2 = (1.0f - NofXd2);
			        OptionPrice2 = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
			        OptionPrice = (otype < 1) ? OptionPrice2 : OptionPrice;

			    output= OptionPrice;


			#undef CNDF
			#undef SPMAddressOf

			WriteSPMFlt(6, addr_output, 0, output);
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 2;}
		inline virtual int InitiationInterval(){return 2;}
		inline virtual int PipelineDepth(){return 1;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "blackScholes";}
		inline virtual int ArgumentCount(){return 7;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 901;}
	};
}

#endif
