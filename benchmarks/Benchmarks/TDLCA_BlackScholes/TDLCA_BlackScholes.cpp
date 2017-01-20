#include "../../BenchmarkNode.h"
#include "BlackScholesLCacc.h"
#include <stdint.h>
#include <iostream>

class TDCLA_BlackScholes : public BenchmarkNode
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
	uint8_t* buf;
	uint8_t* constCluster;
	uint32_t bufSize;
public:
	TDCLA_BlackScholes()
	{
		std::cin >> dataSize;
	}
	virtual void Initialize(int threadID, int procID);
	virtual void Run();
	virtual void Shutdown();
};

BENCH_DECL(TDCLA_BlackScholes);

void TDCLA_BlackScholes::Initialize(int threadID, int procID)
{
	thread = threadID;
        sptprice = new float[dataSize];
        strike = new float[dataSize];
        rate = new float[dataSize];
        volatility = new float[dataSize];
        otime = new float[dataSize];
        otype = new int[dataSize];
        price = new float[dataSize];
	for(int i = 0; i < dataSize; i++)
	{
		sptprice[i] = strike[i] = rate[i] = volatility[i] = otime[i] = price[i] = 0.0;
		otype[i] = 0;
	}
	CreateBuffer_BlackScholesLCacc_td(&buf, &bufSize, &constCluster, thread, sptprice, strike, rate, volatility, otime, otype, price, dataSize, 32);
	Touch(thread, sptprice, dataSize * sizeof(float));
	Touch(thread, strike, dataSize * sizeof(float));
	Touch(thread, rate, dataSize * sizeof(float));
	Touch(thread, volatility, dataSize * sizeof(float));
	Touch(thread, otime, dataSize * sizeof(float));
	Touch(thread, otype, dataSize * sizeof(int));
	Touch(thread, price, dataSize * sizeof(float));
	// Touch(thread, buf, bufSize);
}
void TDCLA_BlackScholes::Run()
{
	BlackScholesLCacc_td_buf(buf, bufSize, thread);
}
void TDCLA_BlackScholes::Shutdown()
{
	while(true);
}
