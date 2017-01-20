#include "../../BenchmarkNode.h"
#include "DispMapCompSADLCacc.h"
#include "DispMapIntegSum1LCacc.h"
#include "DispMapIntegSum2LCacc.h"
#include "DispMapFindDispLCacc.h"
#include <stdint.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

#define IMG_W       64
#define IMG_H       64
#define WIN         8
#define SHIFT       64

class TDLCA_Disparity_Map : public BenchmarkNode
{
    int dataSize;
    int thread;
    
    float (*ILeft)[IMG_H + WIN][IMG_W + WIN];
    float (*IRight)[IMG_H + WIN][IMG_W + WIN + SHIFT];
    float (*SAD)[SHIFT][IMG_H + WIN][IMG_W + WIN];
    float (*retDisp)[SHIFT][IMG_H][IMG_W];

    uint8_t* buf1;
    uint8_t* buf2;
    uint8_t* buf3;
    uint8_t* buf4;
    uint8_t* constCluster1;
    uint8_t* constCluster2;
    uint8_t* constCluster3;
    uint8_t* constCluster4;
    uint32_t bufSize1, bufSize2, bufSize3, bufSize4;

public:
    TDLCA_Disparity_Map()
    {
        std::cin >> dataSize;
    }
    virtual void Initialize(int threadID, int procID);
    virtual void Run();
    virtual void Shutdown();
};

BENCH_DECL(TDLCA_Disparity_Map);

void TDLCA_Disparity_Map::Initialize(int threadID, int procID)
{
    thread = threadID;
    
    ILeft = new float[dataSize][IMG_H + WIN][IMG_W + WIN];
    IRight = new float[dataSize][IMG_H + WIN][IMG_W + WIN + SHIFT];
    SAD = new float[dataSize][SHIFT][IMG_H + WIN][IMG_W + WIN];
    retDisp = new float[dataSize][SHIFT][IMG_H][IMG_W];

    for (int d = 0; d < dataSize; d++) {
      for (int i = 0; i < IMG_H + WIN; i++) {
        for (int j = 0; j < IMG_W + WIN; j++) {
          ILeft[d][i][j] = 0.0;
        }
        for (int j = 0; j < IMG_W + WIN + SHIFT; j++) {
          IRight[d][i][j] = 0.0;
        }
      }
      for (int k = 0; k < SHIFT; k++) {
        for (int i = 0; i < IMG_H + WIN; i++) {
          for (int j = 0; j < IMG_W + WIN; j++) {
        SAD[d][k][i][j] = 255.0;
          }
        }
        for (int i = 0; i < IMG_H; i++) {
          for (int j = 0; j < IMG_W; j++) {
        retDisp[d][k][i][j] = 0.0;
          }
        }
      }
    }

    Touch(thread, ILeft, sizeof(float) * dataSize * (IMG_H + WIN) * (IMG_W + WIN));
    Touch(thread, IRight, sizeof(float) * dataSize * (IMG_H + WIN) * (IMG_W + WIN + SHIFT));
    Touch(thread, SAD, sizeof(float) * dataSize * SHIFT * (IMG_H + WIN) * (IMG_W + WIN));
    Touch(thread, retDisp, sizeof(float) * dataSize * SHIFT * IMG_H * IMG_W);
    
    CreateBuffer_DispMapCompSADLCacc_td(&buf1, &bufSize1, &constCluster1, 
        thread, (float*)ILeft, (float*)IRight, (float*)SAD, dataSize, 
        SHIFT, IMG_H + WIN, IMG_W + WIN, 1, 1, 1, IMG_W + WIN);
    CreateBuffer_DispMapIntegSum1LCacc_td(&buf2, &bufSize2, &constCluster2, 
        thread, (float*)SAD, (float*)SAD, dataSize, 
        SHIFT, IMG_H + WIN, IMG_W + WIN, 1, 1, 1, IMG_W + WIN);
    CreateBuffer_DispMapIntegSum2LCacc_td(&buf3, &bufSize3, &constCluster3, 
        thread, (float*)SAD, (float*)SAD, dataSize, 
        SHIFT, IMG_H + WIN, IMG_W + WIN, 1, 1, 1, IMG_W + WIN);
    CreateBuffer_DispMapFindDispLCacc_td(&buf4, &bufSize4, &constCluster4, 
        thread, (float*)SAD, (float*)retDisp, dataSize, 
        IMG_H, IMG_W, SHIFT, WIN, 1, 1, 512 / IMG_W, IMG_W);
    // Touch(thread, buf1, bufSize1);
    // Touch(thread, buf2, bufSize2);
    // Touch(thread, buf3, bufSize3);
    // Touch(thread, buf4, bufSize4);
}
void TDLCA_Disparity_Map::Run()
{
    DispMapCompSADLCacc_td_buf(buf1, bufSize1, thread);
    DispMapIntegSum1LCacc_td_buf(buf2, bufSize2, thread);
    DispMapIntegSum2LCacc_td_buf(buf3, bufSize3, thread);
    DispMapFindDispLCacc_td_buf(buf4, bufSize4, thread);
}
void TDLCA_Disparity_Map::Shutdown()
{
    while(true);
}
