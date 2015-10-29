#include "../../BenchmarkNode.h"
#include <stdint.h>
#include <iostream>
#include <cmath>

#define ITER_COUNT 2

class SW_Registration : public BenchmarkNode
{
        int dataSize;
        float* u1;
        float* u2;
        float* u3;
        float* v1;
        float* v2;
        float* v3;
public:
	SW_Registration()
	{
		std::cin >> dataSize;
	}
        virtual void Initialize(int threadID, int procID);
        virtual void Run();
        virtual void Shutdown();
};

BENCH_DECL(SW_Registration);

void V_cal(float* v1, float* v2, float* v3, int dataSize)
{
        int i,j,k;
        float std = 4.5f;

        float lambda = (std*std)/(2*3);
        float nu = (1 + 2*lambda - sqrtf(1+4*lambda))/(2*lambda);
        float BoundryScale = 1/(1-nu);
        int counter;
        float PostScale = 1;
        for(i = 0; i < 3*3; i++)
                PostScale *= nu/lambda;

        int step;

	for( step = 0; step < 3; step++) {
	  for( counter = 0; counter < dataSize; counter++) {

	    for(i = 0; i < dataSize; i++) {
	      v1[counter * dataSize * dataSize + dataSize * i] = v1[counter * dataSize * dataSize + dataSize * i]* BoundryScale;
	      v2[counter * dataSize * dataSize + dataSize * i] = v2[counter * dataSize * dataSize + dataSize * i]* BoundryScale;
	      v3[counter * dataSize * dataSize + dataSize * i] = v3[counter * dataSize * dataSize + dataSize * i]* BoundryScale;
	    }
	    for(j = 1; j < dataSize; j++) {
	      for( i = 0; i < dataSize; i++) {
		v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + i * dataSize + j-1] * nu;
		v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[counter * dataSize * dataSize + i * dataSize + j-1] * nu;
		v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[counter * dataSize * dataSize + i * dataSize + j-1] * nu;
	      }
	    }

	    for(i = 0; i < dataSize; i++) {
	      v1[counter * dataSize * dataSize + i * dataSize + dataSize-1] = v1[counter * dataSize * dataSize + i * dataSize + dataSize-1] * BoundryScale;
	      v2[counter * dataSize * dataSize + i * dataSize + dataSize-1] = v2[counter * dataSize * dataSize + i * dataSize + dataSize-1] * BoundryScale;
	      v3[counter * dataSize * dataSize + i * dataSize + dataSize-1] = v3[counter * dataSize * dataSize + i * dataSize + dataSize-1] * BoundryScale;
	    }
	    for(j = dataSize-2; j >= 0; j--) {
	      for( i = 0; i < dataSize; i++) {
		v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + i * dataSize + j+1] * nu;
		v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[counter * dataSize * dataSize + i * dataSize + j+1] * nu;
		v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[counter * dataSize * dataSize + i * dataSize + j+1] * nu;
	      }
	    }

	    for(j = 0; j < dataSize; j++) {
	      v1[counter * dataSize * dataSize + j] = v1[counter * dataSize * dataSize + j] * BoundryScale;
	      v2[counter * dataSize * dataSize + j] = v2[counter * dataSize * dataSize + j] * BoundryScale;
	      v3[counter * dataSize * dataSize + j] = v3[counter * dataSize * dataSize + j] * BoundryScale;
	    }
	    for(i = 1; i < dataSize; i++) {
	      for( j = 0; j < dataSize; j++) {
		v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + (i - 1) * dataSize + j] * nu;
		v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[counter * dataSize * dataSize + (i - 1) * dataSize + j] * nu;
		v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[counter * dataSize * dataSize + (i - 1) * dataSize + j] * nu;
	      }
	    }

	    for(j = 0; j < dataSize; j++) {
	      v1[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] = v1[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] * BoundryScale;
	      v2[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] = v2[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] * BoundryScale;
	      v3[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] = v3[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] * BoundryScale;
	    }
	    for(i = dataSize-2; i >=0; i--) {
	      for( j = 0; j < dataSize; j++) {
		v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + (i + 1) * dataSize + j] * nu;
		v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[counter * dataSize * dataSize + (i + 1) * dataSize + j] * nu;
		v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[counter * dataSize * dataSize + (i + 1) * dataSize + j] * nu;
	      } 
	    }
	  }

	  for(i = 0; i < dataSize; i++) {
	    for(j = 0; j < dataSize; j++) {
	      v1[i * dataSize + j] = v1[i * dataSize + j] * BoundryScale;
	      v2[i * dataSize + j] = v2[i * dataSize + j] * BoundryScale;
	      v3[i * dataSize + j] = v3[i * dataSize + j] * BoundryScale;
	    }
	  }

	  for(counter = 1; counter < dataSize; counter++){
	    for(i = 0; i < dataSize; i++) {
	      for( j = 0; j < dataSize; j++) {
		v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[(counter - 1) * dataSize * dataSize + i * dataSize + j] * nu;
		v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[(counter - 1) * dataSize * dataSize + i * dataSize + j] * nu;
		v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[(counter - 1) * dataSize * dataSize + i * dataSize + j] * nu;
	      }
	    }
	  }

	  for(i = 0; i < dataSize; i++) {
	    for(j = 0; j < dataSize; j++){
	      v1[(counter - 1) * dataSize * dataSize + i * dataSize + j] = v1[(counter - 1) * dataSize * dataSize + i * dataSize + j] * BoundryScale;
	      v2[(counter - 1) * dataSize * dataSize + i * dataSize + j] = v2[(counter - 1) * dataSize * dataSize + i * dataSize + j] * BoundryScale;
	      v3[(counter - 1) * dataSize * dataSize + i * dataSize + j] = v3[(counter - 1) * dataSize * dataSize + i * dataSize + j] * BoundryScale;
	    }
	  }

	  for( counter = dataSize-2; counter >=0; counter--) {
	    for(i = 0; i < dataSize; i++) {
	      for( j = 0; j < dataSize; j++){
		v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[(counter + 1) * dataSize * dataSize + i * dataSize + j] * nu;
		v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[(counter + 1) * dataSize * dataSize + i * dataSize + j] * nu;
		v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[(counter + 1) * dataSize * dataSize + i * dataSize + j] * nu;
	      }
	    }
	  }
	}

	for( counter = 0; counter <dataSize; counter++) {
	  for(i = 0; i < dataSize; i++) {
	    for( j = 0; j < dataSize; j++) {
	      v1[counter * dataSize * dataSize + i * dataSize + j] *= PostScale;
	      v2[counter * dataSize * dataSize + i * dataSize + j] *= PostScale;
	      v3[counter * dataSize * dataSize + i * dataSize + j] *= PostScale;
	    }
	  }
	}
}
void U_cal(float* u1, float* u2, float* u3, float* v1, float* v2, float* v3, int dataSize)
{
int i,j,k;
#define xu1(i,j,k) u1[(i) * dataSize * dataSize + (j) * dataSize + (k)]
#define xu2(i,j,k) u2[(i) * dataSize * dataSize + (j) * dataSize + (k)]
#define xu3(i,j,k) u3[(i) * dataSize * dataSize + (j) * dataSize + (k)]
#define xv1(i,j,k) v1[(i) * dataSize * dataSize + (j) * dataSize + (k)]
#define xv2(i,j,k) v2[(i) * dataSize * dataSize + (j) * dataSize + (k)]
#define xv3(i,j,k) v3[(i) * dataSize * dataSize + (j) * dataSize + (k)]
for( i = 1; i < dataSize-1; i++ )
{
        for( j = 1; j < dataSize-1; j++ )
        {
                for( k = 1; k < dataSize-1; k++ )
                {
                        float   du1_dx = (xu1(i+1,j,k) - xu1(i-1,j,k)) *0.5f;
                        float   du2_dx = (xu2(i+1,j,k) - xu2(i-1,j,k)) *0.5f;
                        float   du3_dx = (xu3(i+1,j,k) - xu3(i-1,j,k)) *0.5f;

                        float   du1_dy = (xu1(i,j+1,k) - xu1(i,j-1,k)) *0.5f;
                        float   du2_dy = (xu2(i,j+1,k) - xu2(i,j-1,k)) *0.5f;
                        float   du3_dy = (xu3(i,j+1,k) - xu3(i,j-1,k)) *0.5f;

                        float   du1_dz = (xu1(i,j,k+1) - xu1(i,j,k-1)) *0.5f;
                        float   du2_dz = (xu2(i,j,k+1) - xu2(i,j,k-1)) *0.5f;
                        float   du3_dz = (xu3(i,j,k+1) - xu3(i,j,k-1)) *0.5f;

                        float r1 = xv1(i,j,k) - xv1(i,j,k)*du1_dx - xv2(i,j,k)*du1_dy - xv3(i,j,k)*du1_dz;
                        float r2 = xv2(i,j,k) - xv1(i,j,k)*du2_dx - xv2(i,j,k)*du2_dy - xv3(i,j,k)*du2_dz;
                        float r3 = xv3(i,j,k) - xv1(i,j,k)*du3_dx - xv2(i,j,k)*du3_dy - xv3(i,j,k)*du3_dz;

                        xv1(i,j,k)=r1;
                        xv2(i,j,k)=r2;
                        xv3(i,j,k)=r3;
                }
        }
}
/*float   dt = 0.1f/1.0f;
for( i = 1; i < dataSize-1; i++ )
for( j = 1; j < dataSize-1; j++ )
for( k = 1; k < dataSize-1; k++ )
{
        xu1(i,j,k)+=dt*xv1(i,j,k);
        xu2(i,j,k)+=dt*xv2(i,j,k);
        xu3(i,j,k)+=dt*xv3(i,j,k);
}*/
}
void SW_Registration::Initialize(int threadID, int procID)
{
        u1 = new float[dataSize * dataSize * dataSize];
        u2 = new float[dataSize * dataSize * dataSize];
        u3 = new float[dataSize * dataSize * dataSize];
        v1 = new float[(dataSize + 1) * (dataSize + 1) * (dataSize + 1)];
        v2 = new float[(dataSize + 1) * (dataSize + 1) * (dataSize + 1)];
        v3 = new float[(dataSize + 1) * (dataSize + 1) * (dataSize + 1)];
	for(int i = 0; i < dataSize * dataSize * dataSize; i++)
	{
		u1[i] = u2[i] = u3[i] = 0.0f;
	}
	for(int i = 0; i < (dataSize + 1) * (dataSize + 1) * (dataSize + 1); i++)
	{
		v1[i] = v2[i] = v3[i] = 0.0f;
	}
}
void SW_Registration::Run()
{
        for(int i = 0; i < ITER_COUNT; i++)
        {
                V_cal(v1, v2, v3, dataSize);
                U_cal(u1, u2, u3, v1, v2, v3, dataSize);
        }
}
void SW_Registration::Shutdown()
{
        while(true);
}


