#include "../../BenchmarkNode.h"
#include "VectorAddSampleLCacc.h"
#include <stdint.h>
#include <iostream>

class Sample : public BenchmarkNode
{
	int dataSize;
	int thread;
	float* input_1;
	float* input_2;
	float* output;
	uint8_t* buf;
	uint8_t* constCluster;
	uint32_t bufSize;
public:
	Sample()
	{
		std::cin >> dataSize;
	}
	virtual void Initialize(int threadID, int procID);
	virtual void Run();
	virtual void Shutdown();
};

BENCH_DECL(Sample);

void Sample::Initialize(int threadID, int procID)
{
	thread = threadID;
	input_1 = new float[dataSize];
	input_2 = new float[dataSize];
	output = new float[dataSize];
	for(int i = 0; i < dataSize; i++)
	{
		input_1[i] = input_2[i] = (float)i;
		output[i] = 0.0f;
	}
	CreateBuffer_VectorAddSampleLCacc_td(&buf, &bufSize, &constCluster, thread, input_1, input_2, output, dataSize, 32);
	Touch(thread, input_1, dataSize * sizeof(float));
	Touch(thread, input_2, dataSize * sizeof(float));
	Touch(thread, output, dataSize * sizeof(float));

	Touch(thread, buf, bufSize);
}
void Sample::Run()
{
	VectorAddSampleLCacc_td_buf(buf, bufSize, thread);
}
void Sample::Shutdown()
{
	while(true);
}
