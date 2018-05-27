#include "../../BenchmarkNode.h"
#include <stdint.h>
#include "MatMulLCacc.h"

class TDLCA_MatMul : public BenchmarkNode
{
  int dataSize;
  int thread;

  int sizeN;
  int sizeM;
  int sizeP;

  float* input1;
  float* input2;
  float* output;

  uint8_t* buf;
  uint32_t bufSize;
  uint8_t* constCluster;

public:
  TDLCA_MatMul()
  {
    std::cin >> dataSize;
  }
  virtual void Initialize(int threadID, int procID);
  virtual void Run();
  virtual void Shutdown();
};

BENCH_DECL(TDLCA_MatMul);

void TDLCA_MatMul::Initialize(int threadID, int procID)
{
  thread = threadID;

  sizeN = dataSize;
  sizeM = dataSize;
  sizeP = dataSize;

  float* input1 = new float[sizeN * sizeM];
	float* input2 = new float[sizeM * sizeP];
	float* output = new float[sizeN * sizeP];

	for(int n = 0; n < sizeN; n++)
	{
		for(int m = 0; m < sizeM; m++)
		{
			input1[n * sizeM + m] = 0.0f;
		}
	}
	for(int m = 0; m < sizeM; m++)
	{
		for(int p = 0; p < sizeP; p++)
		{
			input2[m * sizeP + p] = 0.0f;
		}
	}
	for(int n = 0; n < sizeN; n++)
	{
		for(int p = 0; p < sizeP; p++)
		{
			output[n * sizeP + p] = 0.0f;
		}
	}

  DumpStats();

  CreateBuffer_MatMulLCacc_td(&buf, &bufSize, &constCluster, thread,
    input1, input2, output, dataSize, dataSize, dataSize);

  Touch(thread, input1, sizeN * sizeM * sizeof(float));
  Touch(thread, input2, sizeM * sizeP * sizeof(float));
  Touch(thread, output, sizeN * sizeP * sizeof(float));
}

void TDLCA_MatMul::Run()
{
  MatMulLCacc_td_buf(buf, bufSize, thread);
}

void TDLCA_MatMul::Shutdown()
{
  delete input1;
  delete input2;
  delete output;

  while (true);
}

