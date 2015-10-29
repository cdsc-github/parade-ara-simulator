#include "../../BenchmarkNode.h"
#include "StreamCluster1LCacc.h"
#include "StreamCluster3LCacc.h"
#include "StreamCluster4LCacc.h"
#include "StreamCluster5LCacc.h"
#include "StreamCluster6LCacc.h"
#include <stdint.h>
#include <iostream>
#include <cmath>

#define ITER_COUNT 2

class TDLCA_StreamCluster : public BenchmarkNode
{
        int dataSize;
        int thread;

	uint32_t* inIsCenter; //whether a point is a center
  //uint32_t* outIsCenter;
	uint32_t* centerTable; //index table of centers
	float(* point)[32];
	float* inCost;
  //float* outCost;
	float* weight;
	uint32_t* assign;
	uint32_t* inSwitchMembership;
  //uint32_t* outSwitchMembership;
	float* low;
	float* globalLow;

	uint8_t* buf1; uint32_t buf1Size; uint8_t* constCluster1;
	uint8_t* buf3; uint32_t buf3Size; uint8_t* constCluster3;
	uint8_t* buf4; uint32_t buf4Size; uint8_t* constCluster4;
	uint8_t* buf5; uint32_t buf5Size; uint8_t* constCluster5;
	uint8_t* buf6; uint32_t buf6Size; uint8_t* constCluster6;

public:
	TDLCA_StreamCluster()
	{
		std::cin >> dataSize;
	}
        virtual void Initialize(int threadID, int procID);
        virtual void Run();
        virtual void Shutdown();
};

BENCH_DECL(TDLCA_StreamCluster);

void TDLCA_StreamCluster::Initialize(int threadID, int procID)
{
        thread = threadID;

	float x[32] = {0};
        inIsCenter = new uint32_t[dataSize];
        //outIsCenter = new uint32_t[dataSize];
        centerTable = new uint32_t[dataSize];
        point = new float[dataSize][32];
        inCost = new float[dataSize];
        //outCost = new float[dataSize];
        weight = new float[dataSize];
        assign = new uint32_t[dataSize];
        inSwitchMembership = new uint32_t[dataSize];
        //outSwitchMembership = new uint32_t[dataSize];
        low = new float[dataSize];
        globalLow = new float[dataSize];

	for (int i=0;i<dataSize;i++)
	{
	  //outIsCenter[i] = 0;
		centerTable[i] = 0;
		for(int j = 0; j < 32; j++)
		{
			point[i][j] = 0;
		}
		inCost[i] = 0;
		//outCost[i] = 0;
		weight[i] = 0;
		assign[i] = 0;
		inSwitchMembership[i] = 0;
		//outSwitchMembership[i] = 0;
		low[i] = 0;
		globalLow[i] = 0;
		if (i%2)
			inIsCenter[i] = false;	
		else
			inIsCenter[i]= true;
	}
	CreateBuffer_StreamCluster1LCacc_td(&buf1, &buf1Size, &constCluster1, thread, inIsCenter, centerTable, dataSize, 128);
	CreateBuffer_StreamCluster3LCacc_td(&buf3, &buf3Size, &constCluster3, thread, point, weight, inCost, assign, inSwitchMembership, inSwitchMembership, (uint64_t)centerTable, (uint64_t)low, x, dataSize, 128);
	CreateBuffer_StreamCluster4LCacc_td(&buf4, &buf4Size, &constCluster4, thread, inIsCenter, centerTable, low, (uint64_t)globalLow, dataSize, 128);
	CreateBuffer_StreamCluster5LCacc_td(&buf5, &buf5Size, &constCluster5, thread, assign, point, weight, inSwitchMembership, inCost, inCost, (uint64_t) centerTable, (uint64_t) globalLow, x, dataSize, 128);
	CreateBuffer_StreamCluster6LCacc_td(&buf6, &buf6Size, &constCluster6, thread, inIsCenter, centerTable, inIsCenter, (uint64_t) low, dataSize, 128);
	Touch(thread, buf1, buf1Size);
	Touch(thread, buf3, buf3Size);
	Touch(thread, buf4, buf4Size);
	Touch(thread, buf5, buf5Size);
	Touch(thread, buf6, buf6Size);
	/*std::cout << "buf1Size: " << buf1Size << std::endl;
	std::cout << "buf3Size: " << buf3Size << std::endl;
	std::cout << "buf4Size: " << buf4Size << std::endl;
	std::cout << "buf5Size: " << buf5Size << std::endl;
	std::cout << "buf6Size: " << buf6Size << std::endl;//*/
	Touch(thread, inIsCenter, dataSize * sizeof(uint32_t));
	//Touch(thread, outIsCenter, dataSize * sizeof(uint32_t));
	Touch(thread, centerTable, dataSize * sizeof(uint32_t));
	Touch(thread, point, dataSize * sizeof(float) * 32);
	Touch(thread, inCost, dataSize * sizeof(float));
	//Touch(thread, outCost, dataSize * sizeof(float));
	Touch(thread, assign, dataSize * sizeof(uint32_t));
	Touch(thread, inSwitchMembership, dataSize * sizeof(uint32_t));
	//Touch(thread, outSwitchMembership, dataSize * sizeof(uint32_t));
	Touch(thread, low, dataSize * sizeof(float));
	Touch(thread, globalLow, dataSize * sizeof(float));//*/
}

void TDLCA_StreamCluster::Run()
{
        for(int i = 0; i < ITER_COUNT; i++)
        {
		StreamCluster1LCacc_td_buf(buf1, buf1Size, thread);
		StreamCluster3LCacc_td_buf(buf3, buf3Size, thread);
		StreamCluster4LCacc_td_buf(buf4, buf4Size, thread);
		StreamCluster5LCacc_td_buf(buf5, buf5Size, thread);
		StreamCluster6LCacc_td_buf(buf6, buf6Size, thread);
        }
}
void TDLCA_StreamCluster::Shutdown()
{
        while(true);
}

