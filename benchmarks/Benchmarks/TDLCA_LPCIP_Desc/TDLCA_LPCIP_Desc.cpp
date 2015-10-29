#include "../../BenchmarkNode.h"
#include "LPCIPLCacc.h"
#include <stdint.h>
#include <iostream>

#define IMGW                    640
#define IMGH                    480
#define PATCH_H                 10
#define PATCH_W                 10

class TDLCA_LPCIP_Desc : public BenchmarkNode
{
	int thread;
	int dataSize;
	int binSize;

	uint8_t (*img);
	uint64_t (*imgStart);
	float (*centerX);
	float (*centerY);
	float (*rho);
	uint8_t (*result)[PATCH_H * PATCH_W];

	uint8_t *buf;
	uint32_t bufSize;
	uint8_t *constCluster;
public:
	TDLCA_LPCIP_Desc()
	{
		std::cin >> dataSize;
	}
	virtual void Initialize(int threadID, int procID);
	virtual void Run();
	virtual void Shutdown();
};

BENCH_DECL(TDLCA_LPCIP_Desc);

void TDLCA_LPCIP_Desc::Initialize(int threadID, int procID)
{
	thread = threadID;

	img = new uint8_t[IMGW * IMGH];
	imgStart = new uint64_t[dataSize];
	centerX = new float[dataSize];
	centerY = new float[dataSize];
	rho = new float[dataSize];
	result = new uint8_t[dataSize][PATCH_H * PATCH_W];

	for (int i = 0; i < IMGW * IMGH; i++) {
		img[i] = 0;
	}
	for (int i = 0; i < dataSize; i++) {
		imgStart[i] = (uint64_t)img;
		centerX[i] = 100.0f;
		centerY[i] = 100.0f;
		rho[i] = 1.0f;
		for (int x = 0; x < PATCH_H * PATCH_W; x++) {
			result[i][x] = 0;
		}
	}

	CreateBuffer_LPCIPLCacc_td(&buf, &bufSize, &constCluster, thread, imgStart, centerX, centerY, rho, result, dataSize, 128);
	Touch(thread, buf, bufSize);
	Touch(thread, img, IMGW * IMGH);
	Touch(thread, imgStart, dataSize * sizeof(uint64_t));
	Touch(thread, centerX, dataSize * sizeof(float));
	Touch(thread, centerY, dataSize * sizeof(float));
	Touch(thread, rho, dataSize * sizeof(float));
	Touch(thread, result, dataSize * sizeof(uint8_t) * PATCH_H * PATCH_W);
}
void TDLCA_LPCIP_Desc::Run()
{
	LPCIPLCacc_td_buf(buf, bufSize, thread);
}
void TDLCA_LPCIP_Desc::Shutdown()
{
	while(true);
}
