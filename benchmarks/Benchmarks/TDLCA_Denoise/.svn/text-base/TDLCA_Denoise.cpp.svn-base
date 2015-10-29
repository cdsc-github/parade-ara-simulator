#include "../../BenchmarkNode.h"
#include "Denoise1LCacc.h"
#include "Denoise2LCacc.h"
#include <stdint.h>
#include <iostream>

#define ITER_COUNT 2

class TDLCA_Denoise : public BenchmarkNode
{
        float* u;
        float* g;
        float* f;
	float* ret_u;
        int dataSize;
        uint8_t* buf1;
	uint32_t buf1Size;
        uint8_t* buf2;
	uint32_t buf2Size;
	uint8_t* constCluster1;
	uint8_t* constCluster2;
        int thread;
public:
	TDLCA_Denoise()
	{
		std::cin >> dataSize;
	}
        virtual void Initialize(int threadID, int procID);
        virtual void Run();
        virtual void Shutdown();
};

BENCH_DECL(TDLCA_Denoise);

void TDLCA_Denoise::Initialize(int threadID, int procID)
{
        thread = threadID;
        u = new float[dataSize * dataSize * dataSize];
        g = new float[dataSize * dataSize * dataSize];
        f = new float[dataSize * dataSize * dataSize];
	ret_u = new float[dataSize * dataSize * dataSize];
        for(int i = 0; i < dataSize * dataSize * dataSize; i++)
        {
                u[i] = g[i] = f[i] = ret_u[i] = 0;
        }
        CreateBuffer_Denoise1LCacc_td(&buf1, &buf1Size, &constCluster1, thread, u, g, dataSize, dataSize,  dataSize, 16, 16, 16);
        CreateBuffer_Denoise2LCacc_td(&buf2, &buf2Size, &constCluster2, thread, u, f, g, ret_u, dataSize, dataSize, dataSize, 16, 16, 16, 1.0f, 2.0f, 3.0f);
	Touch(thread, u, dataSize * dataSize * dataSize * sizeof(float));
	Touch(thread, g, dataSize * dataSize * dataSize * sizeof(float));
	Touch(thread, f, dataSize * dataSize * dataSize * sizeof(float));
	Touch(thread, ret_u, dataSize * dataSize * dataSize * sizeof(float));
	Touch(thread, buf1, buf1Size);
	Touch(thread, buf2, buf2Size);
	std::cout << "buf1Size: " << buf1Size << std::endl;
	std::cout << "buf2Size: " << buf2Size << std::endl;
}
void TDLCA_Denoise::Run()
{
	for(int i = 0; i < ITER_COUNT; i++)
	{
		Denoise1LCacc_td_buf(buf1, buf1Size, thread);
		Denoise2LCacc_td_buf(buf2, buf2Size, thread);
	}
}
void TDLCA_Denoise::Shutdown()
{
        while(true);
}
