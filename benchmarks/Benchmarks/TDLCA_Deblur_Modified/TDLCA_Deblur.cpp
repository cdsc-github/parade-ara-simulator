#include "../../BenchmarkNode.h"
#include "Deblur1LCacc.h"
#include "Deblur2LCacc.h"
#include "Denoise1LCacc.h"
#include "Blur1LCacc.h"
#include "Blur2LCacc.h"
#include "Blur3LCacc.h"
#include <stdint.h>
#include <iostream>
#include <cmath>

#define ITER_COUNT 1

class TDLCA_Deblur : public BenchmarkNode
{
    float* u;
    float* g;
    float* f;
    float* conv;
    float* ret_u;
    int dataSize;
    int thread;
    uint8_t* buf1; uint32_t buf1Size; uint8_t* constCluster1;
    uint8_t* buf2; uint32_t buf2Size; uint8_t* constCluster2;
    uint8_t* buf3; uint32_t buf3Size; uint8_t* constCluster3;
    uint8_t* blur1_buf; uint32_t blur1_bufSize; uint8_t* blur1_constCluster;
    uint8_t* blur2_buf; uint32_t blur2_bufSize; uint8_t* blur2_constCluster;
    uint8_t* blur3_buf; uint32_t blur3_bufSize; uint8_t* blur3_constCluster;
public:
    TDLCA_Deblur()
    {
        std::cin >> dataSize;
    }
    virtual void Initialize(int threadID, int procID);
    virtual void Run();
    virtual void Shutdown();
};

BENCH_DECL(TDLCA_Deblur);

void
TDLCA_Deblur::Initialize(int threadID, int procID)
{
    thread = threadID;
    u = new float[dataSize * dataSize * dataSize];
    g = new float[dataSize * dataSize * dataSize];
    f = new float[dataSize * dataSize * dataSize];
    conv = new float[dataSize * dataSize * dataSize];
    ret_u = new float[dataSize * dataSize * dataSize];

    for (int i = 0; i < dataSize * dataSize * dataSize; i++) {
        u[i] = g[i] = f[i] = conv[i] = ret_u[i] = 0;
    }

    CreateBuffer_Denoise1LCacc_td(&buf1, &buf1Size, &constCluster1, thread,
        u, g, dataSize, dataSize, dataSize, 8, 8, 8);
    CreateBuffer_Deblur1LCacc_td(&buf2, &buf2Size, &constCluster2, thread,
        conv, f, conv, 0.20f, dataSize, dataSize, dataSize, 1, 2, dataSize - 2);
    CreateBuffer_Deblur2LCacc_td(&buf3, &buf3Size, &constCluster3, thread,
        u, g, conv, ret_u, dataSize, dataSize, dataSize, 8, 8, 8, 1.0f, 1.0f);
    CreateBuffer_Blur1LCacc_td(&blur1_buf, &blur1_bufSize, &blur1_constCluster,
        thread, conv, conv, 1, dataSize, dataSize, dataSize,
        0.20f, 1, 2, dataSize);
    CreateBuffer_Blur2LCacc_td(&blur2_buf, &blur2_bufSize, &blur2_constCluster,
        thread, conv, conv, 1, dataSize, dataSize, dataSize,
        0.20f, 1, 2, dataSize);
    CreateBuffer_Blur3LCacc_td(&blur3_buf, &blur3_bufSize, &blur3_constCluster,
        thread, conv, conv, 1, dataSize, dataSize, dataSize,
        0.20f, 1, 2, dataSize);

    Touch(thread, u, dataSize * dataSize * dataSize * sizeof(float));
    Touch(thread, g, dataSize * dataSize * dataSize * sizeof(float));
    Touch(thread, f, dataSize * dataSize * dataSize * sizeof(float));
    Touch(thread, conv, dataSize * dataSize * dataSize * sizeof(float));
    Touch(thread, ret_u, dataSize * dataSize * dataSize * sizeof(float));
    // Touch(thread, buf1, buf1Size);
    // Touch(thread, buf2, buf2Size);
    // Touch(thread, buf3, buf3Size);
    // Touch(thread, blur1_buf, blur1_bufSize);
    // Touch(thread, blur2_buf, blur2_bufSize);
    // Touch(thread, blur3_buf, blur3_bufSize);
}

void
TDLCA_Deblur::Run()
{
    for (int i = 0; i < ITER_COUNT; i++) {
        // Denoise1LCacc_td_buf(buf1, buf1Size, thread);
        // for (int j = 0; j < 1; j++) {
        //     Blur1LCacc_td_buf(blur1_buf, blur1_bufSize, thread);
        //     Blur2LCacc_td_buf(blur2_buf, blur2_bufSize, thread);
        //     Blur3LCacc_td_buf(blur3_buf, blur3_bufSize, thread);
        // }
        Deblur1LCacc_td_buf(buf2, buf2Size, thread);
        for (int j = 0; j < 1; j++) {
            Blur1LCacc_td_buf(blur1_buf, blur1_bufSize, thread);
            Blur2LCacc_td_buf(blur2_buf, blur2_bufSize, thread);
            // Blur3LCacc_td_buf(blur3_buf, blur3_bufSize, thread);
        }
        Deblur2LCacc_td_buf(buf3, buf3Size, thread);
    }
}

void
TDLCA_Deblur::Shutdown()
{
    while(true);
}

