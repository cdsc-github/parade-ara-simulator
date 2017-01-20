#include "../../BenchmarkNode.h"
#include "Swaptions1LCacc.h"
#include "Swaptions2LCacc.h"
#include "Swaptions3LCacc.h"
#include "Swaptions4LCacc.h"


#include <stdint.h>
#include <iostream>
#include <cmath>

#define ITER_COUNT 2

class TDLCA_Swaptions : public BenchmarkNode
{
    int dataSize;
    int thread;

    float* randSeeds;
    float (*randVals)[3][11];
    float (*pdZ)[3][11];
    float (*pdTotalDrift)[11];
    float (*ppdFactors)[3][11];
    float (*shock)[55];
    float (*in_ppdHJMPath)[11][11];
    float (*out_ppdHJMPath)[11][11];
    
    uint8_t* buf1; uint32_t buf1Size; uint8_t* constCluster1;
    uint8_t* buf2; uint32_t buf2Size; uint8_t* constCluster2;
    uint8_t* buf3; uint32_t buf3Size; uint8_t* constCluster3;
    uint8_t* buf4; uint32_t buf4Size; uint8_t* constCluster4;

public:
    TDLCA_Swaptions()
    {
        std::cin >> dataSize;
    }
    virtual void Initialize(int threadID, int procID);
    virtual void Run();
    virtual void Shutdown();
};

BENCH_DECL(TDLCA_Swaptions);

void TDLCA_Swaptions::Initialize(int threadID, int procID)
{
    thread = threadID;

    randSeeds = new float[dataSize];
    randVals = new float[dataSize][3][11];
    pdZ = new float[dataSize][3][11];
    pdTotalDrift = new float[dataSize][11];
    ppdFactors = new float[dataSize][3][11];
    shock = new float[dataSize][55];
    in_ppdHJMPath = new float[dataSize][11][11];
    out_ppdHJMPath = new float[dataSize][11][11];

    for (int i = 0; i < dataSize; i++) {
        for (int j = 0; j < 11; j++) {
            for(int k = 0; k < 11; k++) {
                in_ppdHJMPath[i][j][k] = out_ppdHJMPath[i][j][k] = 0.0f;
            }
            pdTotalDrift[i][j] = 0.0f;
        }
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 11; k++) {
                randVals[i][j][k] = 0.0f;
                pdZ[i][j][k] = 0.0f;
                ppdFactors[i][j][k] = 0.0f;
            }
        }
        for (int j = 0; j < 55; j++) {
            shock[i][j] = 0.0f;
        }
    }
    
    CreateBuffer_Swaptions1LCacc_td(&buf1, &buf1Size, &constCluster1, 
        thread, randSeeds, randVals, dataSize, 32);
    CreateBuffer_Swaptions2LCacc_td(&buf2, &buf2Size, &constCluster2, 
        thread, (float*)randVals, (float*)pdZ, dataSize, 3, 11, 32);
    CreateBuffer_Swaptions3LCacc_td(&buf3, &buf3Size, &constCluster3, 
        thread, ppdFactors, pdZ, shock, dataSize, 32);
    CreateBuffer_Swaptions4LCacc_td(&buf4, &buf4Size, &constCluster4, 
        thread, pdTotalDrift, shock, in_ppdHJMPath, out_ppdHJMPath, 0.0f, 0.0f, 
        dataSize, 32);
    // Touch(thread, buf1, buf1Size);
    // Touch(thread, buf2, buf2Size);
    // Touch(thread, buf3, buf3Size);
    // Touch(thread, buf4, buf4Size);
    Touch(thread, randSeeds, dataSize * sizeof(float));
    Touch(thread, randVals, dataSize * sizeof(float) * 3 * 11);
    Touch(thread, pdZ, dataSize * sizeof(float) * 3 * 11);
    Touch(thread, pdTotalDrift, dataSize * sizeof(float) * 11);
    Touch(thread, ppdFactors, dataSize * sizeof(float) * 3 * 11);
    Touch(thread, shock, dataSize * sizeof(float) * 55);
    Touch(thread, in_ppdHJMPath, dataSize * sizeof(float) * 11 * 11);
    Touch(thread, out_ppdHJMPath, dataSize * sizeof(float) * 11 * 11);
}

void TDLCA_Swaptions::Run()
{
    for (int i = 0; i < ITER_COUNT; i++) {
        Swaptions1LCacc_td_buf(buf1, buf1Size, thread);
        Swaptions2LCacc_td_buf(buf2, buf2Size, thread);
        Swaptions3LCacc_td_buf(buf3, buf3Size, thread);
        Swaptions4LCacc_td_buf(buf4, buf4Size, thread);
    }
}
void TDLCA_Swaptions::Shutdown()
{
    while(true);
}


