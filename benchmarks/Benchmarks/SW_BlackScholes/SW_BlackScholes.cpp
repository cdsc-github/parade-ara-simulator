#include "../../BenchmarkNode.h"
#include <cmath>
#include <stdint.h>
#include <iostream>

float inv_sqrt_2xPI = 0.0053f;
class SW_BlackScholes : public BenchmarkNode
{
    int dataSize;
    int thread;
    float* sptprice;
    float* strike;
    float* rate;
    float* volatility;
    float* otime;
    int* otype;
    float* price;
    float CNDF(float InputX);
    float BlkSchlsEqEuroNoDiv(float& sptprice, float& strike, float& rate, float& volatility, float& time, int& otype);
public:
    SW_BlackScholes()
    {
        std::cin >> dataSize;
    }
    virtual void Initialize(int threadID, int procID);
    virtual void Run();
    virtual void Shutdown();
};

BENCH_DECL(SW_BlackScholes);

float SW_BlackScholes::CNDF(float InputX)
{
    int sign;
    float InputX1;
    float OutputX;
    float OutputX2;
    float xInput;
    float xNPrimeofX;
    float expValues;
    float xK2;
    float xK2_2, xK2_3;
    float xK2_4, xK2_5;
    float xLocal, xLocal_1;
    float xLocal_2, xLocal_3;

    InputX1 = -InputX;
    InputX = (InputX < 0.0f) ? InputX : InputX1;
    sign = (InputX < 0.0f) ? 0 : 1;

    xInput = InputX;

    expValues = expf(-0.5f * InputX * InputX);
    xNPrimeofX = expValues;
    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

    xK2 = 0.2316419f * xInput;
    xK2 = 1.0f + xK2;
    xK2 = 1.0f / xK2;
    xK2_2 = xK2 * xK2;
    xK2_3 = xK2_2 * xK2;
    xK2_4 = xK2_3 * xK2;
    xK2_5 = xK2_4 * xK2;

    xLocal_1 = xK2 * 0.319381530f;
    xLocal_2 = xK2_2 * (-0.356563782f);
    xLocal_3 = xK2_3 * 1.781477937f;
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_4 * (-1.821255978f);
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_5 * 1.330274429f;
    xLocal_2 = xLocal_2 + xLocal_3;

    xLocal_1 = xLocal_2 + xLocal_1;
    xLocal   = xLocal_1 * xNPrimeofX;
    xLocal   = 1.0f - xLocal;

    OutputX  = xLocal;
    OutputX2 = 1.0f - OutputX;
    OutputX = (sign < 1) ? OutputX2 : OutputX;

    return OutputX;
}
float SW_BlackScholes::BlkSchlsEqEuroNoDiv(float& sptprice, float& strike, float& rate, float& volatility, float& time, int& otype)
{
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

    NofXd1 = CNDF( d1);
    NofXd2 = CNDF( d2);

    FutureValueX = strike * ( expf( -(rate)*(time) ) );
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
     NegNofXd1 = (1.0f - NofXd1);
        NegNofXd2 = (1.0f - NofXd2);
        OptionPrice2 = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
        OptionPrice = (otype < 1) ? OptionPrice2 : OptionPrice;

    return OptionPrice;
}
void SW_BlackScholes::Initialize(int threadID, int procID)
{
    thread = threadID;
    sptprice = new float[dataSize];
    strike = new float[dataSize];
    rate = new float[dataSize];
    volatility = new float[dataSize];
    otime = new float[dataSize];
    otype = new int[dataSize];
    price = new float[dataSize];

    for (int i = 0; i < dataSize; i++)
    {
        sptprice[i] = strike[i] = rate[i] = volatility[i] = otime[i] = price[i] = 0.0;
        otype[i] = 0;
    }
}
void SW_BlackScholes::Run()
{
    for (int i = 0; i < dataSize; i++)
    {
        price[i] = BlkSchlsEqEuroNoDiv(sptprice[i], strike[i], rate[i], volatility[i], otime[i], otype[i]);
    }
}
void SW_BlackScholes::Shutdown()
{
    while(true);
}
