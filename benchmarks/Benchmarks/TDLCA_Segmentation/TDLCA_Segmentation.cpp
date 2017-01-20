#include "../../BenchmarkNode.h"
#include "Segmentation1LCacc.h"
#include <stdint.h>
#include <iostream>
#include <cmath>

#define ITER_COUNT 2

class TDLCA_Segmentation : public BenchmarkNode
{
    int dataSize;
    int thread;
    float* phi;
    float* u0;
    float* result;
    uint8_t* buf;
    uint32_t bufSize;
    uint8_t* constCluster;
public:
    TDLCA_Segmentation()
    {
        std::cin >> dataSize;
    }
    virtual void Initialize(int threadID, int procID);
    virtual void Run();
    virtual void Shutdown();
};

BENCH_DECL(TDLCA_Segmentation);

void TDLCA_Segmentation::Initialize(int threadID, int procID)
{
    thread = threadID;
    phi = new float[dataSize * dataSize * dataSize];
    u0 = new float[dataSize * dataSize * dataSize];
    result = new float[dataSize * dataSize * dataSize];
    for (int i = 0; i < dataSize * dataSize * dataSize; i++) {
        phi[i] = u0[i] = result[i] = 0;
    }
    CreateBuffer_Segmentation1LCacc_td(&buf, &bufSize, &constCluster, 
        thread, phi, u0, result, dataSize, dataSize, dataSize, 8, 8, 8, 
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    // Touch(thread, buf, bufSize);
    Touch(thread, phi, dataSize * dataSize * dataSize * sizeof(float));
    Touch(thread, u0, dataSize * dataSize * dataSize * sizeof(float));
    Touch(thread, result, dataSize * dataSize * dataSize * sizeof(float));
}
void TDLCA_Segmentation::Run()
{
    for (int i = 0; i < ITER_COUNT; i++) {
        Segmentation1LCacc_td_buf(buf, bufSize, thread);
    }
}
void TDLCA_Segmentation::Shutdown()
{
    while(true);
}

