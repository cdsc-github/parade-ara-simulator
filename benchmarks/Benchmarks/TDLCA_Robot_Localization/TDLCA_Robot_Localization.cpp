#include "../../BenchmarkNode.h"
#include "RobLocLCacc.h"
#include <stdint.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

#define N_VAL 3

class TDLCA_Robot_Localization : public BenchmarkNode
{
    int dataSize;
    int thread;
    
    float (*ran1)[N_VAL][3];        // input
    float (*ran2)[N_VAL][3];        // input
    float (*quat)[N_VAL][4];        // input
    float (*accl)[N_VAL][3];        // input
    float (*pos_in)[N_VAL][3];      // input and output
    float (*vel_in)[N_VAL][3];      // input and output
    float (*pos_out)[N_VAL][3];     // input and output
    float (*vel_out)[N_VAL][3];     // input and output
    
    uint8_t* buf1;
    uint8_t* constCluster1;
    uint32_t bufSize1;

public:
    TDLCA_Robot_Localization()
    {
        std::cin >> dataSize;
    }
    virtual void Initialize(int threadID, int procID);
    virtual void Run();
    virtual void Shutdown();
};

BENCH_DECL(TDLCA_Robot_Localization);

void TDLCA_Robot_Localization::Initialize(int threadID, int procID)
{
    thread = threadID;
    
    ran1 = new float[dataSize][N_VAL][3];
    ran2 = new float[dataSize][N_VAL][3];
    quat = new float[dataSize][N_VAL][4];
    accl = new float[dataSize][N_VAL][3];
    pos_in = new float[dataSize][N_VAL][3];
    vel_in = new float[dataSize][N_VAL][3];
    pos_out = new float[dataSize][N_VAL][3];
    vel_out = new float[dataSize][N_VAL][3];
    
    for (int z = 0; z < dataSize; z++) {
        // for (int y = 0; y < N_VAL; y++) {
        //     for (int x = 0; x < 3; x++) {
        //         ran1[z][y][x] = ran2[z][y][x] = accl[z][y][x] = pos_in[z][y][x] = vel_in[z][y][x] = pos_out[z][y][x] = vel_out[z][y][x] = 0.0f;
        //     }
        //     for (int x = 0; x < 4; x++) {
        //         quat[z][y][x] = 0.0f;
        //     }
        // }

        ran1[z][0][0] = (-2.0 * log(0.9)) / 9;
        ran1[z][0][1] = (-2.0 * log(0.45)) / 4.5;
        ran1[z][0][2] = (-2.0 * log(0.3)) / 3;
        ran1[z][1][0] = (-2.0 * log(0.45)) / 4.5;
        ran1[z][1][1] = (-2.0 * log(0.9)) / 9;
        ran1[z][1][2] = (-2.0 * log(0.6)) / 6;
        ran1[z][2][0] = (-2.0 * log(0.3)) / 3;
        ran1[z][2][1] = (-2.0 * log(0.6)) / 6;
        ran1[z][2][2] = (-2.0 * log(0.9)) / 9;
        for (int i = 3; i < N_VAL; i++) {
            float x;
            x = 0.09 / (i+1);
            ran1[z][i][0] = (-2.0 * log(x)) / x;
            x = 0.18 / (i+1);
            ran1[z][i][1] = (-2.0 * log(x)) / x;
            x = 0.27 / (i+1);
            ran1[z][i][2] = (-2.0 * log(x)) / x;
        }
        
        ran2[z][0][0] = (-4.0 * log(0.9)) / 9;
        ran2[z][0][1] = (-4.0 * log(0.45)) / 4.5;
        ran2[z][0][2] = (-4.0 * log(0.3)) / 3;
        ran2[z][1][0] = (-4.0 * log(0.45)) / 4.5;
        ran2[z][1][1] = (-4.0 * log(0.9)) / 9;
        ran2[z][1][2] = (-4.0 * log(0.6)) / 6;
        ran2[z][2][0] = (-4.0 * log(0.3)) / 3;
        ran2[z][2][1] = (-4.0 * log(0.6)) / 6;
        ran2[z][2][2] = (-4.0 * log(0.9)) / 9;
        for (int i = 3; i < N_VAL; i++) {
            float x;
            x = 0.09 / (i+1);
            ran2[z][i][0] = (-4.0 * log(x)) / x;
            x = 0.18 / (i+1);
            ran2[z][i][1] = (-4.0 * log(x)) / x;
            x = 0.27 / (i+1);
            ran2[z][i][2] = (-4.0 * log(x)) / x;
        }
        
        for (int i = 0; i < N_VAL; i++) {
            for (int j = 0; j < 4; j++) {
                quat[z][i][j] = 0.0;
            }
            for (int j = 0; j < 3; j++) {
                accl[z][i][j] = 0.1;
                pos_in[z][i][j] = vel_in[z][i][j] = 0.0;
                pos_out[z][i][j] = vel_out[z][i][j] = 0.0;
            }
        }
    }
    
    Touch(thread, ran1, sizeof(float) * dataSize * N_VAL * 3);
    Touch(thread, ran2, sizeof(float) * dataSize * N_VAL * 3);
    Touch(thread, quat, sizeof(float) * dataSize * N_VAL * 4);
    Touch(thread, accl, sizeof(float) * dataSize * N_VAL * 3);
    Touch(thread, pos_in, sizeof(float) * dataSize * N_VAL * 3);
    Touch(thread, vel_in, sizeof(float) * dataSize * N_VAL * 3);
    Touch(thread, pos_out, sizeof(float) * dataSize * N_VAL * 3);
    Touch(thread, vel_out, sizeof(float) * dataSize * N_VAL * 3);
    
    CreateBuffer_RobLocLCacc_td(&buf1, &bufSize1, &constCluster1, thread, 
        (float (*)[3])ran1, (float (*)[3])ran2, (float (*)[4])quat, 
        (float (*)[3])accl, (float (*)[3])pos_in, (float (*)[3])vel_in, 
        (float (*)[3])pos_out, (float (*)[3])vel_out, 
        dataSize, N_VAL, 128, N_VAL);
}
void TDLCA_Robot_Localization::Run()
{
    RobLocLCacc_td_buf(buf1, bufSize1, thread);
}
void TDLCA_Robot_Localization::Shutdown()
{
    while(true);
}
