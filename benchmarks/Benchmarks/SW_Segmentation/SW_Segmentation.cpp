#include "../../BenchmarkNode.h"
#include <stdint.h>
#include <iostream>
#include <cmath>

#define ITER_COUNT 2

class SW_Segmentation : public BenchmarkNode
{
        int dataSize;
        float* phi;
        float* u0;
        float* result;
public:
	SW_Segmentation()
	{
		std::cin >> dataSize;
	}
        virtual void Initialize(int threadID, int procID);
        virtual void Run();
        virtual void Shutdown();
};

BENCH_DECL(SW_Segmentation);

void SW_Segmentation::Initialize(int threadID, int procID)
{
        phi = new float[dataSize * dataSize * dataSize];
        u0 = new float[dataSize * dataSize * dataSize];
        result = new float[dataSize * dataSize * dataSize];
	for(int i = 0; i < dataSize * dataSize * dataSize; i++)
	{
		phi[i] = u0[i] = result[i] = 0.0f;
	}
}
void SW_Segmentation::Run()
{
        int xPitch = 1;
        int yPitch = dataSize;
        int zPitch = dataSize * dataSize;
        float dx = 1.0f;
        float dy = 1.0f;
        float dz = 1.0f;
        float dx2 = 2.0f;
        float dy2 = 2.0f;
        float dz2 = 2.0f;
        int n = dataSize;
        int m = dataSize;
        int p = dataSize;
        float lambda1 = 1.0f;
        float lambda2 = 1.0f;
        float mu = 1.0f;
        float c1 = 1.0f;
        float c2 = 1.0f;
#define INDEX(a,x,y,z) a[(x) * xPitch + (y) * yPitch + (z) * zPitch]
        for(int i=1; i < m-1; i++)
        {
                for(int j=1; j < n-1; j++)
                {
                        for(int k=1; k < p-1; k++)
                        {
                                float Dx_p = (INDEX(phi,i+1,j,k) - INDEX(phi,i,j,k))/dx;
                                float Dx_m = (INDEX(phi,i,j,k) - INDEX(phi,i-1,j,k))/dx;
                                float Dy_p = (INDEX(phi,i,j+1,k) - INDEX(phi,i,j,k))/dy;
                                float Dy_m = (INDEX(phi,i,j,k) - INDEX(phi,i,j-1,k))/dy;
                                float Dz_p = (INDEX(phi,i,j,k+1) - INDEX(phi,i,j,k))/dz;
                                float Dz_m = (INDEX(phi,i,j,k) - INDEX(phi,i,j,k-1))/dz;
                                float Dx_0 = (INDEX(phi,i+1,j,k) - INDEX(phi,i-1,j,k))/dx2;
                                float Dy_0 = (INDEX(phi,i,j+1,k) - INDEX(phi,i,j-1,k))/dy2;
                                float Dz_0 = (INDEX(phi,i,j,k+1) - INDEX(phi,i,j,k-1))/dz2;
                                float Dxx = (Dx_p - Dx_m) / dx;
                                float Dyy = (Dy_p - Dy_m) / dy;
                                float Dzz = (Dz_p - Dz_m) / dz;
                                float Dxy = ( INDEX(phi,i+1,j+1,k) - INDEX(phi,i+1,j-1,k) - INDEX(phi,i-1,j+1,k) + INDEX(phi,i-1,j-1,k) ) / (4*dx*dy);
                                float Dxz = ( INDEX(phi,i+1,j,k+1) - INDEX(phi,i+1,j,k-1) - INDEX(phi,i-1,j,k+1) + INDEX(phi,i-1,j,k-1) ) / (4*dx*dz);
                                float Dyz = ( INDEX(phi,i,j+1,k+1) - INDEX(phi,i,j+1,k-1) - INDEX(phi,i,j-1,k+1) + INDEX(phi,i,j-1,k-1) ) / (4*dy*dz);
                                float K  = (  Dx_0*Dx_0*Dyy - 2.0f*Dx_0*Dy_0*Dxy + Dy_0*Dy_0*Dxx
                                        + Dx_0*Dx_0*Dzz - 2.0f*Dx_0*Dz_0*Dxz + Dz_0*Dz_0*Dxx
                                        + Dy_0*Dy_0*Dzz - 2.0f*Dy_0*Dz_0*Dyz + Dz_0*Dz_0*Dyy )
                                        / ( pow(Dx_0*Dx_0 + Dy_0*Dy_0 + Dz_0*Dz_0, 1.5f) + 0.00005f );
                                float Grad = sqrt(Dx_0*Dx_0 + Dy_0*Dy_0 + Dz_0*Dz_0);
                                INDEX(result, i, j, k) = Grad * ( mu * K + lambda1*(INDEX(u0,i,j,k)-c1)*(INDEX(u0,i,j,k)-c1) - lambda2*(INDEX(u0,i,j,k)-c2)*(INDEX(u0,i,j,k)-c2) );
                        }
                }
        }
}
void SW_Segmentation::Shutdown()
{
        while(true);
}

