#include "../../BenchmarkNode.h"
#include "JacobiansLCacc.h"
#include "SphericalCoordsLCacc.h"
#include <stdint.h>
#include <iostream>
#include <cmath>

class TDLCA_EKF_SLAM : public BenchmarkNode
{
    int thread;
    int dataSize;

    float (*X)[7];
    float (*U)[7];
    float (*out_X_oplus_U)[7];
    float (*out_df_dx)[7*4];
    float (*cpose)[7];
    float (*point)[3];
    float (*out_jacob_dryp_dpoint)[3*3];
    float (*out_jacob_dryp_dpose)[3*7];
    
    uint8_t* buf1;
    uint32_t buf1Size;
    uint8_t* buf2;
    uint32_t buf2Size;
    uint8_t* constCluster1;
    uint8_t* constCluster2;

public:
    TDLCA_EKF_SLAM()
    {
        std::cin >> dataSize;
    }
    virtual void Initialize(int threadID, int procID);
    virtual void Run();
    virtual void Shutdown();
};

BENCH_DECL(TDLCA_EKF_SLAM);

void TDLCA_EKF_SLAM::Initialize(int threadID, int procID)
{
    thread = threadID;

    X = new float[dataSize][7];
    U = new float[dataSize][7];
    out_df_dx = new float[dataSize][7*4];
    out_X_oplus_U = new float[dataSize][7];
    cpose = new float[dataSize][7];
    point = new float[dataSize][3];
    out_jacob_dryp_dpoint = new float[dataSize][3*3];
    out_jacob_dryp_dpose = new float[dataSize][3*7];

    for (int i = 0; i < dataSize; i++) {               
        for (int x = 0; x < 7; x++) {
            X[i][x] = 0;
            U[i][x] = 0;
            cpose[i][x] = 0;
            out_X_oplus_U[i][x] = 0;
        }
        for (int x = 0; x < 3; x++) {
            point[i][x] = 0;
        }
        for (int x = 0; x < 7 * 4; x++) {
            out_df_dx[i][x] = 0;
        }
        for (int x = 0; x < 7 * 3; x++) {
            out_jacob_dryp_dpose[i][x] = 0;
        }
        for (int x = 0; x < 3 * 3; x++) {
            out_jacob_dryp_dpoint[i][x] = 0;
        }
    }

    CreateBuffer_JacobiansLCacc_td(&buf1, &buf1Size, &constCluster1, 
        thread, X, U, out_df_dx, out_X_oplus_U, dataSize, 128);
    CreateBuffer_SphericalCoordsLCacc_td(&buf2, &buf2Size, &constCluster2, 
        thread, cpose, point, out_jacob_dryp_dpoint, out_jacob_dryp_dpose, 
        dataSize, 128);
    // Touch(thread, buf1, buf1Size);
    // Touch(thread, buf2, buf2Size);
    Touch(thread, X, dataSize * sizeof(float) * 7);
    Touch(thread, U, dataSize * sizeof(float) * 7);
    Touch(thread, out_df_dx, dataSize * sizeof(float) * 7 * 4);
    Touch(thread, out_X_oplus_U, dataSize * sizeof(float) * 7);
    Touch(thread, cpose, dataSize * sizeof(float) * 7);
    Touch(thread, point, dataSize * sizeof(float) * 3);
    Touch(thread, out_jacob_dryp_dpoint, dataSize * sizeof(float) * 3 * 3);
    Touch(thread, out_jacob_dryp_dpose, dataSize * sizeof(float) * 3 * 7);
}
void TDLCA_EKF_SLAM::Run()
{
    JacobiansLCacc_td_buf(buf1, buf1Size, thread);
    SphericalCoordsLCacc_td_buf(buf2, buf2Size, thread);
}
void TDLCA_EKF_SLAM::Shutdown()
{
    while(true);
}

