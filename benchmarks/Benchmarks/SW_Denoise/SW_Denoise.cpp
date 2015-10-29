#include "../../BenchmarkNode.h"
#include <iostream>
#include <cmath>

class SW_Denoise : public BenchmarkNode
{
	float* u;
	float* g;
	float* f;
	float* u_ret;
	int dataSize;
	int thread;
public:
	SW_Denoise()
	{
		std::cin >> dataSize;
	}
	virtual void Initialize(int threadID, int procID);
	virtual void Run();
	virtual void Shutdown();
};

BENCH_DECL(SW_Denoise);

#define ITER_COUNT 2 

void SW_Denoise::Initialize(int threadID, int procID)
{
        thread = threadID;
        u = new float[dataSize * dataSize * dataSize];
        g = new float[dataSize * dataSize * dataSize];
        f = new float[dataSize * dataSize * dataSize];
        u_ret = new float[dataSize * dataSize * dataSize];
	for(int i = 0; i < dataSize * dataSize * dataSize; i++)
	{
		u[i] = g[i] = f[i] = u_ret[i] = 0.0f;
	}
}
void SW_Denoise::Run()
{
#define SQR(x) ((x)*(x))
#define CENTER   (m+M*(n+N*p))
#define RIGHT    (m+M*(n+N*p)+M)
#define LEFT     (m+M*(n+N*p)-M)
#define DOWN     (m+M*(n+N*p)+1)
#define UP       (m+M*(n+N*p)-1)
#define ZOUT     (m+M*(n+N*p+N))
#define ZIN      (m+M*(n+N*p-N))        
#define DT 5.0f
        float sigma = 0.03f;
        float lambda = 1.0f;
        float vGamma = lambda / sigma;
        int M = dataSize;
        int N = dataSize;
        int P = dataSize;
        for(int i = 0; i < ITER_COUNT; i++)
        {
                for(int p = 1; p < P - 1; p++)
                {
                        for(int n = 1; n < N - 1; n++)
                        {
                                for(int m = 1; m < M - 1; m++)
                                {
                                        g[CENTER] = 1.0f/sqrt(
                                                SQR(u[CENTER] - u[RIGHT])
                                                + SQR(u[CENTER] - u[LEFT])
                                                + SQR(u[CENTER] - u[DOWN])
                                                + SQR(u[CENTER] - u[UP])
                                                + SQR(u[CENTER] - u[ZOUT])
                                                + SQR(u[CENTER] - u[ZIN]));
                                }
                        }
                }
                for(int p = 1; p < P - 1; p++)
                {
                        for(int n = 1; n < N - 1; n++)
                        {
                                for(int m = 1; m < M - 1; m++)
                                {
                                        float r = u[CENTER] * f[CENTER] / sigma;
                                        r = (r * (2.38944f + r * (0.950037f + r))) / (4.65314f + r * (2.57541f + r * (1.48937f + r)));
                                        u_ret[CENTER] = (u[CENTER] + DT * (u[RIGHT] * g[RIGHT] + u[LEFT] * g[LEFT] + u[DOWN] * g[DOWN] + u[UP] * g[UP] + u[ZOUT] * g[ZOUT] + u[ZIN] * g[ZIN] + vGamma * f[CENTER]*r) ) / (1.0f + DT*(g[RIGHT] + g[LEFT] + g[DOWN] + g[UP] + g[ZOUT] + g[ZIN] + vGamma));
                                }
                        }
                }
                /*for(int p = 1; p < P - 1; p++)
                {
                        for(int n = 1; n < N - 1; n++)
                        {
                                for(int m = 1; m < M - 1; m++)
                                {
				        u[CENTER] = u_ret[CENTER];
                                }
                        }
		}//*/
        }
}
void SW_Denoise::Shutdown()
{
        while(true);
}

