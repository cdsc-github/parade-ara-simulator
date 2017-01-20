#include "../../BenchmarkNode.h"
#include "Registration1LCacc.h"
#include "Blur1LCacc.h"
#include "Blur2LCacc.h"
#include "Blur3LCacc.h"
#include <stdint.h>
#include <iostream>
#include <cmath>

#define ITER_COUNT 1

class TDLCA_Registration : public BenchmarkNode
{
    int thread;
    int dataSize;
    float* vSet;
    float* u1;
    float* u2;
    float* u3;
    float* result_1;
    float* result_2;
    float* result_3;
    uint8_t* buf1; uint32_t buf1Size; uint8_t* constCluster1;
    uint8_t* buf2; uint32_t buf2Size; uint8_t* constCluster2;
    uint8_t* buf3; uint32_t buf3Size; uint8_t* constCluster3;
    uint8_t* buf4; uint32_t buf4Size; uint8_t* constCluster4;
public:
    TDLCA_Registration()
    {
        std::cin >> dataSize;
    }
    virtual void Initialize(int threadID, int procID);
    virtual void Run();
    virtual void Shutdown();
};

BENCH_DECL(TDLCA_Registration);

void
TDLCA_Registration::Initialize(int threadID, int procID)
{
    thread = threadID;
    vSet = new float[3 * dataSize * dataSize * dataSize];
    u1 = new float[dataSize * dataSize * dataSize];
    result_1 = new float[dataSize * dataSize * dataSize];
    u2 = new float[dataSize * dataSize * dataSize];
    result_2 = new float[dataSize * dataSize * dataSize];
    u3 = new float[dataSize * dataSize * dataSize];
    result_3 = new float[dataSize * dataSize * dataSize];

    for (int i = 0; i < 3 * dataSize * dataSize * dataSize; i++) {
        vSet[i] = 0.0f;
    }
    for (int i = 0; i < dataSize * dataSize * dataSize; i++) {
        u1[i] = result_1[i] = 0.0f;
        u2[i] = result_2[i] = 0.0f;
        u3[i] = result_3[i] = 0.0f;
    }

    CreateBuffer_Blur1LCacc_td(&buf1, &buf1Size, &constCluster1, thread,
        vSet, vSet, 3, dataSize, dataSize, dataSize, 0.20f, 1, 2, dataSize);
    CreateBuffer_Blur2LCacc_td(&buf2, &buf2Size, &constCluster2, thread,
        vSet, vSet, 3, dataSize, dataSize, dataSize, 0.20f, 1, 2, dataSize);
    CreateBuffer_Blur3LCacc_td(&buf3, &buf3Size, &constCluster3, thread,
        vSet, vSet, 3, dataSize, dataSize, dataSize, 0.20f, 1, 2, dataSize);
    CreateBuffer_Registration1LCacc_td(&buf4, &buf4Size, &constCluster4, thread,
        vSet, vSet + (dataSize * dataSize * dataSize),
        vSet + (dataSize * dataSize * dataSize * 2), u1, u2, u3,
        result_1, result_2, result_3, dataSize, dataSize, dataSize, 8, 8, 8);

    Touch(thread, u1, sizeof(float) * dataSize * dataSize * dataSize);
    Touch(thread, u2, sizeof(float) * dataSize * dataSize * dataSize);
    Touch(thread, u3, sizeof(float) * dataSize * dataSize * dataSize);
    Touch(thread, result_1, sizeof(float) * dataSize * dataSize * dataSize);
    Touch(thread, result_2, sizeof(float) * dataSize * dataSize * dataSize);
    Touch(thread, result_3, sizeof(float) * dataSize * dataSize * dataSize);
    Touch(thread, vSet, 3 * sizeof(float) * dataSize * dataSize * dataSize);
    // Touch(thread, buf1, buf1Size);
    // Touch(thread, buf2, buf2Size);
    // Touch(thread, buf3, buf3Size);
    // Touch(thread, buf4, buf4Size);
}

void
TDLCA_Registration::Run()
{
    for (int i = 0; i < ITER_COUNT; i++) {
        for(int x = 0; x < 1; x++) {
            Blur1LCacc_td_buf(buf1, buf1Size, thread);
            Blur2LCacc_td_buf(buf2, buf2Size, thread);
            // Blur3LCacc_td_buf(buf3, buf3Size, thread);
        }
        Registration1LCacc_td_buf(buf4, buf4Size, thread);
    }
}

void
TDLCA_Registration::Shutdown()
{
    while(true);
}

