#include "../../BenchmarkNode.h"
#include "TexSynth1LCacc.h"
#include "TexSynth2LCacc.h"
#include <ctime>
#include <stdint.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

#define WIDTHin         32
#define HEIGHTin        32
#define WIDTHout        512
#define HEIGHTout       32

class TDLCA_Texture_Synthesis : public BenchmarkNode
{
	int dataSize;
	int thread;
	
	float (*image)[HEIGHTin][WIDTHin];
	float (*target)[HEIGHTout][WIDTHout];
	float (*result)[HEIGHTout][WIDTHout];
	unsigned int rand_seeds[100];
	int (*atlas)[HEIGHTout][WIDTHout][2];
	
//	InstanceData_sig__TexSynth1LCacc instance1;
//	InstanceData_sig__TexSynth2LCacc** instance2;
	uint8_t* buf1;
	uint32_t buf1Size;
	uint8_t* constCluster1;
	uint8_t** buf2;
	uint32_t* buf2Size;
	uint8_t** constCluster2;
public:
	TDLCA_Texture_Synthesis()
	{
		std::cin >> dataSize;
	}
	virtual void Initialize(int threadID, int procID);
	virtual void Run();
	virtual void Shutdown();
};

BENCH_DECL(TDLCA_Texture_Synthesis);

void TDLCA_Texture_Synthesis::Initialize(int threadID, int procID)
{
	thread = threadID;
	
	srand((unsigned int) time(NULL));
	
	image = new float[dataSize][HEIGHTin][WIDTHin];
	target = new float[dataSize][HEIGHTout][WIDTHout];
	result = new float[dataSize][HEIGHTout][WIDTHout];
	atlas = new int[dataSize][HEIGHTout][WIDTHout][2];
	
	for (int d = 0; d < dataSize; d++) {
		for (int i = 0; i < HEIGHTin; i++)
			for (int j = 0; j < WIDTHin; j++)
				image[d][i][j] = 0.0;
		for (int i = 0; i < HEIGHTout; i++)
			for (int j = 0; j < WIDTHout; j++)
				target[d][i][j] = result[d][i][j] = 1.0;
		for (int i = 0; i < HEIGHTout; i++)
			for (int j = 0; j < WIDTHout; j++)
				for (int k = 0; k < 2; k++)
					atlas[d][i][j][k] = 0;
	}
	for(int i = 0; i < 100; i++)
	{
		rand_seeds[i] = (unsigned int) rand();
	}
	buf2 = new uint8_t*[HEIGHTout];
	buf2Size = new uint32_t[HEIGHTout];
	constCluster2 = new uint8_t*[HEIGHTout];
	CreateBuffer_TexSynth1LCacc_td(&buf1, &buf1Size, &constCluster1, thread, (int (*)[2])atlas, (float*)result, rand_seeds, HEIGHTin, WIDTHin, HEIGHTout, WIDTHout, dataSize, (intptr_t)image, WIDTHout / 16);
	Touch(thread, buf1, buf1Size);
	for(int i = 0; i < HEIGHTout; i++)
	{
		CreateBuffer_TexSynth2LCacc_td(&(buf2[i]), &(buf2Size[i]), &(constCluster2[i]), thread, (float*)result, HEIGHTin, WIDTHin, HEIGHTout, WIDTHout, dataSize, (intptr_t)image, (intptr_t)target, (intptr_t)atlas, i, WIDTHout / 16);
		Touch(thread, buf2[i], buf2Size[i]);
	}
	Touch(thread, image, dataSize * HEIGHTin * WIDTHin * sizeof(float));
	Touch(thread, target, dataSize * HEIGHTout * WIDTHout * sizeof(float));
	Touch(thread, result, dataSize * HEIGHTout * WIDTHout * sizeof(float));
	Touch(thread, atlas, dataSize * HEIGHTout * WIDTHout * 2 * sizeof(int));
}
void TDLCA_Texture_Synthesis::Run()
{
	TexSynth1LCacc_td_buf(buf1, buf1Size, thread);
	for(int i = 0; i < HEIGHTout; i++)
	{
		TexSynth2LCacc_td_buf(buf2[i], buf2Size[i], thread);
	}
}
void TDLCA_Texture_Synthesis::Shutdown()
{
	while(true);
}
