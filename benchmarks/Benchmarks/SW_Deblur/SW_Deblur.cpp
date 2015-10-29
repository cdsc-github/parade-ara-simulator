#include "../../BenchmarkNode.h"
#include <iostream>
#include <cmath>

#define ITER_COUNT 2

class SW_Deblur : public BenchmarkNode
{
        float* u;
        float* g;
        float* f;
        float* conv;
	float* ret_u;
        int dataSize;
        int thread;
public:
	SW_Deblur()
	{
		std::cin >> dataSize;
	}
        virtual void Initialize(int threadID, int procID);
        virtual void Run();
        virtual void Shutdown();
};

BENCH_DECL(SW_Deblur);

/*void GaussianBlur(float *u, const int Size[3], float Ksigma)
{
#define GAUSSIAN_NUMSTEPS 3

    const int PlaneStep = Size[0]*Size[1];
    float *uPtr, *uCopy, *uEnd;
    float lambda = (Ksigma*Ksigma)/(2.0*GAUSSIAN_NUMSTEPS);
    float nu = (1.0 + 2.0*lambda - sqrt(1.0 + 4.0*lambda))/(2.0*lambda);
    float BoundaryScale = 1.0/(1.0 - nu);
    float PostScale = pow(nu/lambda,3*GAUSSIAN_NUMSTEPS);
    int Step = GAUSSIAN_NUMSTEPS;
    int n[3];


    uEnd = u + PlaneStep*Size[2];

    do
    {
        for(n[2] = 0, uPtr = u; n[2] < Size[2]; ++n[2], uPtr+=PlaneStep)
        {
            uCopy = uPtr;

            for(n[1] = 0; n[1] < Size[1]; ++n[1], uPtr+=Size[0])
            {
                // Filter downwards
                uPtr[0] *= BoundaryScale;
                ++uPtr;

                for(n[0] = 1; n[0] < Size[0]; ++n[0], ++uPtr)
                {
                    uPtr[0] += nu*uPtr[-1];
                }

                // Filter upwards
                --uPtr;
                uPtr[0] *= BoundaryScale;
                --uPtr;

                for(n[0] = Size[0]-2; n[0] >= 0; --n[0], --uPtr)
                {
                    uPtr[0] += nu*uPtr[1];
                }

                ++uPtr;
            }

            uPtr = uCopy;

            // Filter right
            for(n[0] = 0; n[0] < Size[0]; ++n[0], ++uPtr)
            {
                uPtr[0] *= BoundaryScale;
            }

            for(n[1] = 1; n[1] < Size[1]; ++n[1])
            {
                for(n[0] = 0; n[0] < Size[0]; ++n[0], ++uPtr)
                {
                    uPtr[0] += nu*uPtr[-Size[0]];
                }
            }
            --uPtr;

            // Filter left
            for(n[0] = Size[0]-1; n[0] >= 0; --n[0], --uPtr)
            {
                uPtr[0] *= BoundaryScale;
            }

            for(n[1] = Size[1]-2; n[1] >= 0; --n[1])
            {
                for(n[0] = Size[0]-1; n[0] >= 0; --n[0], --uPtr)
                {
                    uPtr[0] += nu*uPtr[Size[0]];
                }
            }

            ++uPtr;
        }
        n[0] = PlaneStep;
        uPtr = u;

        do
        {
            uPtr[0] *= BoundaryScale;
            ++uPtr;
        }while(--n[0]);

        for(n[2] = 1; n[2] < Size[2]; ++n[2])
        {
            n[0] = PlaneStep;

            do
            {
                uPtr[0] += nu*uPtr[-PlaneStep];
                ++uPtr;
            }while(--n[0]);
        }

        // Filter in
        n[0] = PlaneStep;
        do
        {
            --uPtr;
            uPtr[0] *= BoundaryScale;
        }while(--n[0]);


        for(n[2] = Size[2]-2; n[2] >= 0; --n[2])
        {
            n[0] = PlaneStep;

            do
            {
                --uPtr;
                uPtr[0] += nu*uPtr[PlaneStep];
            }while(--n[0]);
        }
    }while(--Step);

    do
    {
        u[0] *= PostScale;
    }while(++u < uEnd);
}//*/

void GaussianBlur(float *u, const int Size[3], float Ksigma)
{
#define GAUSSIAN_NUMSTEPS 3
  float lambda = (Ksigma*Ksigma)/(2.0*GAUSSIAN_NUMSTEPS);
  float nu = (1.0 + 2.0*lambda - sqrt(1.0 + 4.0*lambda))/(2.0*lambda);
  float BoundaryScale = 1.0/(1.0 - nu);
  float PostScale = pow(nu/lambda,3*GAUSSIAN_NUMSTEPS);
  int i,j,k,counter;
  //assume cubic size
  int dataSize = Size[0];

  int step;

  for( step = 0; step < 3; step++) {
    for( counter = 0; counter < dataSize; counter++) {

      for(i = 0; i < dataSize; i++) {
	u[counter * dataSize * dataSize + dataSize * i] = u[counter * dataSize * dataSize + dataSize * i]* BoundaryScale;
      }
      for(j = 1; j < dataSize; j++) {
	for( i = 0; i < dataSize; i++) {
	  u[counter * dataSize * dataSize + i * dataSize + j] = u[counter * dataSize * dataSize + i * dataSize + j] + u[counter * dataSize * dataSize + i * dataSize + j-1] * nu;
	}
      }

      for(i = 0; i < dataSize; i++) {
	u[counter * dataSize * dataSize + i * dataSize + dataSize-1] = u[counter * dataSize * dataSize + i * dataSize + dataSize-1] * BoundaryScale;
      }
      for(j = dataSize-2; j >= 0; j--) {
	for( i = 0; i < dataSize; i++) {
	  u[counter * dataSize * dataSize + i * dataSize + j] = u[counter * dataSize * dataSize + i * dataSize + j] + u[counter * dataSize * dataSize + i * dataSize + j+1] * nu;
	}
      }

      for(j = 0; j < dataSize; j++) {
	u[counter * dataSize * dataSize + j] = u[counter * dataSize * dataSize + j] * BoundaryScale;
      }
      for(i = 1; i < dataSize; i++) {
	for( j = 0; j < dataSize; j++) {
	  u[counter * dataSize * dataSize + i * dataSize + j] = u[counter * dataSize * dataSize + i * dataSize + j] + u[counter * dataSize * dataSize + (i - 1) * dataSize + j] * nu;
	}
      }

      for(j = 0; j < dataSize; j++) {
	u[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] = u[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] * BoundaryScale;
      }
      for(i = dataSize-2; i >=0; i--) {
	for( j = 0; j < dataSize; j++) {
	  u[counter * dataSize * dataSize + i * dataSize + j] = u[counter * dataSize * dataSize + i * dataSize + j] + u[counter * dataSize * dataSize + (i + 1) * dataSize + j] * nu;
	} 
      }
    }

    for(i = 0; i < dataSize; i++) {
      for(j = 0; j < dataSize; j++) {
	u[i * dataSize + j] = u[i * dataSize + j] * BoundaryScale;
      }
    }

    for(counter = 1; counter < dataSize; counter++){
      for(i = 0; i < dataSize; i++) {
	for( j = 0; j < dataSize; j++) {
	  u[counter * dataSize * dataSize + i * dataSize + j] = u[counter * dataSize * dataSize + i * dataSize + j] + u[(counter - 1) * dataSize * dataSize + i * dataSize + j] * nu;
	}
      }
    }

    for(i = 0; i < dataSize; i++) {
      for(j = 0; j < dataSize; j++){
	u[(counter - 1) * dataSize * dataSize + i * dataSize + j] = u[(counter - 1) * dataSize * dataSize + i * dataSize + j] * BoundaryScale;
      }
    }

    for( counter = dataSize-2; counter >=0; counter--) {
      for(i = 0; i < dataSize; i++) {
	for( j = 0; j < dataSize; j++){
	  u[counter * dataSize * dataSize + i * dataSize + j] = u[counter * dataSize * dataSize + i * dataSize + j] + u[(counter + 1) * dataSize * dataSize + i * dataSize + j] * nu;
	}
      }
    }
  }

  for( counter = 0; counter <dataSize; counter++) {
    for(i = 0; i < dataSize; i++) {
      for( j = 0; j < dataSize; j++) {
	u[counter * dataSize * dataSize + i * dataSize + j] *= PostScale;
      }
    }
  }

}

void SW_Deblur::Initialize(int threadID, int procID)
{
        thread = threadID;
        u = new float[dataSize * dataSize * dataSize];
        g = new float[dataSize * dataSize * dataSize];
        f = new float[dataSize * dataSize * dataSize];
        conv = new float[dataSize * dataSize * dataSize];
	ret_u = new float[dataSize * dataSize * dataSize];
	for(int i = 0; i < dataSize * dataSize * dataSize; i++)
	{
		u[i] = g[i] = f[i] = conv[i] = ret_u[i] = 0.0f;
	}
}
void SW_Deblur::Run()
{
#define SQR(x) ((x)*(x))
#define CENTER   (m+M*(n+N*p))
#define RIGHT    (m+M*(n+N*p)+M)
#define LEFT     (m+M*(n+N*p)-M)
#define DOWN     (m+M*(n+N*p)+1)
#define UP       (m+M*(n+N*p)-1)
#define ZOUT     (m+M*(n+N*p+N))
#define ZIN      (m+M*(n+N*p-N))
#define DEFAULT_DT       0.0001
#define DEFAULT_KSIGMA   1.8
#define DEFAULT_SIGMA    0.008
#define DEFAULT_LAMBDA   0.085

        float Ksigma = DEFAULT_KSIGMA;
        float sigma = DEFAULT_SIGMA;
        float lambda = DEFAULT_LAMBDA;
        float sigma2 = 0.0f;
        float gamma = 0.0f;
        float dt = DEFAULT_DT;
        int M = dataSize;
        int N = dataSize;
        int P = dataSize;
        int Size[3] = {dataSize, dataSize, dataSize};
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
                //memcpy(conv, u, sizeof(float) * M * N * P);
                GaussianBlur(conv, Size, Ksigma);
                for(int p = 0; p < P; p++)
                {
                        for(int n = 0; n < N; n++)
                        {
                                for(int m = 0; m < M; m++)
                                {
                                        float r = conv[CENTER] * f[CENTER] / sigma2;
                                        r = (r * (2.38944f + r * (0.950037f + r))) / (4.65314f + r * (2.57541f + r * (1.48937f + r)));
                                        conv[CENTER] -= f[CENTER] * r;
                                }
                        }
                }
                GaussianBlur(conv, Size, Ksigma);
                for(int p = 1; p < P - 1; p++)
                {
                        for(int n = 1; n < N - 1; n++)
                        {
                                for(int m = 1; m < M - 1; m++)
                                {
                                        ret_u[CENTER] = (u[CENTER] + dt * (u[RIGHT] * g[RIGHT] + u[LEFT] * g[LEFT] + u[DOWN] * g[DOWN] + u[UP] * g[UP] + u[ZOUT] * g[ZOUT] + u[ZIN] * g[ZIN] + gamma * f[CENTER]) ) / (1.0f + dt*(g[RIGHT] + g[LEFT] + g[DOWN] + g[UP] + g[ZOUT] + g[ZIN]));
                                }
                        }
                }
        }
}
void SW_Deblur::Shutdown()
{
        while(true);
}

