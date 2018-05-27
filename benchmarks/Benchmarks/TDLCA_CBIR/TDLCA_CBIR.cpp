#include "../../BenchmarkNode.h"
#include "cnn_cfg.hpp"
#include "MatMulLCacc.h"
#include "MatMul400LCacc.h"
#include "ReluLCacc.h"
#include "PoolLCacc.h"
#include "ManhattanDistLCacc.h"
#include "PartialSortLCacc.h"
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>

#define NUM_LAYER 4
#define IMG_SIZE 32
#define FOUT11 32
#define K 1000
#define N_FEA 96
#define N_QUERY 4
#define N_CENT 1024
#define N_ACC 16

int ly[13][9] = {
  {DATA, CONV11, CONV11_I, CONV11_I, CONV11_R, CONV11_R, 0, 0, RELU},
  {CONV11, CONV12, CONV12_I, CONV12_I, CONV12_R, CONV12_R, 0, POOL, RELU},
  {POOL12, CONV21, CONV21_I, CONV21_I, CONV21_R, CONV21_R, 0, 0, RELU},
  {CONV21, CONV22, CONV22_I, CONV22_I, CONV22_R, CONV22_R, 0, POOL, RELU},
  {POOL22, CONV31, CONV31_I, CONV31_I, CONV31_R, CONV31_R, 0, 0, RELU},
  {CONV31, CONV32, CONV32_I, CONV32_I, CONV32_R, CONV32_R, 0, 0, RELU},
  {CONV32, CONV33, CONV33_I, CONV33_I, CONV33_R, CONV33_R, 0, POOL, RELU},
  {POOL33, CONV41, CONV41_I, CONV41_I, CONV41_R, CONV41_R, 0, 0, RELU},
  {CONV41, CONV42, CONV42_I, CONV42_I, CONV42_R, CONV42_R, 0, 0, RELU},
  {CONV42, CONV43, CONV43_I, CONV43_I, CONV43_R, CONV43_R, 0, POOL, RELU},
  {POOL43, CONV51, CONV51_I, CONV51_I, CONV51_R, CONV51_R, 0, 0, RELU},
  {CONV51, CONV52, CONV52_I, CONV52_I, CONV52_R, CONV52_R, 0, 0, RELU},
  {CONV52, CONV53, CONV53_I, CONV53_I, CONV53_R, CONV53_R, 0, POOL, RELU}
};

class TDLCA_CBIR : public BenchmarkNode
{
  int dataSize;
  int thread;

  float* in;
  float* out;
  float* weights;

  float (*dataset)[N_FEA];
  float *query;
  float *cent;
  float *dists;
  float *result;
  float *mat_result;
  float *partial_result;

  uint8_t* buf1;
  uint8_t* buf2;
  uint8_t* buf3;
  uint8_t* buf4;
  uint8_t* buf5;
  uint8_t* buf6;
  uint8_t* buf7;
  uint32_t buf1Size;
  uint32_t buf2Size;
  uint32_t buf3Size;
  uint32_t buf4Size;
  uint32_t buf5Size;
  uint32_t buf6Size;
  uint32_t buf7Size;
  uint8_t* constCluster1;
  uint8_t* constCluster2;
  uint8_t* constCluster3;
  uint8_t* constCluster4;
  uint8_t* constCluster5;
  uint8_t* constCluster6;
  uint8_t* constCluster7;

public:
  TDLCA_CBIR()
  {
    std::cin >> dataSize;
  }
  virtual void Initialize(int threadID, int procID);
  virtual void Run();
  virtual void Shutdown();
};

BENCH_DECL(TDLCA_CBIR);

void TDLCA_CBIR::Initialize(int threadID, int procID)
{
  thread = threadID;

  in = new float[IMG_SIZE*IMG_SIZE*9];
  out = new float[IMG_SIZE*IMG_SIZE*FOUT11];
  weights = new float[FOUT11*3*3];

  dataset = new float[dataSize][N_FEA];
  query = new float[N_QUERY * N_FEA];
  cent = new float[N_CENT * N_FEA];
  mat_result = new float[N_QUERY * N_CENT];
  dists = new float[dataSize];
  result = new float[K];
  partial_result = new float[N_ACC * K];

  for (int i = 0; i < IMG_SIZE * IMG_SIZE * 9; i++)
    in[i] = 0.0;

  for (int i = 0; i < IMG_SIZE * IMG_SIZE * FOUT11; i++)
    out[i] = 0.0;

  for (int i = 0; i < FOUT11 * 3 * 3; i++)
    weights[i] = 0.0;

  for (int i = 0; i < dataSize; i++)
    for (int j = 0; j < N_FEA; j++)
      dataset[i][j] = 0.0;

  for (int i = 0; i < N_QUERY * N_FEA; i++)
    query[i] = 0.0;

  for (int i = 0; i < N_CENT * N_FEA; i++)
    cent[i] = 0.0;

  for (int i = 0; i < N_QUERY * N_CENT; i++)
    mat_result[i] = 0.0;

  for (int i = 0; i < dataSize; i++)
    dists[i] = 0.0;

  for (int i = 0; i < K; i++)
    result[i] = 0.0;

  for (int i = 0; i < N_ACC * K; i++)
    partial_result[i] = 0.0;

  CreateBuffer_MatMul400LCacc_td(&buf1, &buf1Size, &constCluster1,
    thread, weights, in, out, FOUT11, KSIZE * KSIZE, IMG_SIZE * IMG_SIZE);
  CreateBuffer_ReluLCacc_td(&buf2, &buf2Size, &constCluster2,
    thread, out, out, FOUT11 * IMG_SIZE * IMG_SIZE, 32);
  // CreateBuffer_PoolLCacc_td(&buf3, &buf3Size, &constCluster3,
  //   thread, out, out, FOUT11, IMG_SIZE / 2, IMG_SIZE / 2, 16, 16);

  CreateBuffer_MatMulLCacc_td(&buf4, &buf4Size, &constCluster4,
    thread, query, cent, mat_result, N_QUERY, N_FEA, N_CENT);

  CreateBuffer_ManhattanDistLCacc_td(&buf5, &buf5Size, &constCluster5,
    thread, dataset, dists, query, dataSize, 128);
  CreateBuffer_PartialSortLCacc_td(&buf6, &buf6Size, &constCluster6,
    thread, dists, result, dataSize, dataSize / N_ACC / 8);
  CreateBuffer_PartialSortLCacc_td(&buf7, &buf7Size, &constCluster7,
    thread, partial_result, result, K * N_ACC, K * N_ACC);

  Touch(thread, in, IMG_SIZE * IMG_SIZE * 9 * sizeof(float));
  Touch(thread, out, IMG_SIZE * IMG_SIZE * FOUT11 * sizeof(float));
  Touch(thread, weights, FOUT11 * 3 * 3 * sizeof(float));
  Touch(thread, dataset, dataSize * N_FEA * sizeof(float));
  Touch(thread, query, N_QUERY * N_FEA * sizeof(float));
  Touch(thread, cent, N_CENT * N_FEA * sizeof(float));
  Touch(thread, dists, dataSize * sizeof(float));
  Touch(thread, mat_result, N_QUERY * N_CENT * sizeof(float));
  Touch(thread, partial_result, N_ACC * K * sizeof(float));
  Touch(thread, result, K * sizeof(float));
}

void TDLCA_CBIR::Run()
{
  for (int i = 0; i < NUM_LAYER; i++) {
    // conv
    MatMul400LCacc_td_buf(buf1, buf1Size, thread);
    // relu
    ReluLCacc_td_buf(buf2, buf2Size, thread);
    // pooling
    PoolLCacc_td_buf(buf3, buf3Size, thread);
  }

  DumpStats();

  // indexing
  MatMulLCacc_td_buf(buf4, buf4Size, thread);

  DumpStats();

  for (int i = 0; i < N_QUERY; i++) {
    // distance computation
    ManhattanDistLCacc_td_buf(buf5, buf5Size, thread);
    // local sort
    PartialSortLCacc_td_buf(buf6, buf6Size, thread);
    // global sort
    PartialSortLCacc_td_buf(buf7, buf7Size, thread);
  }
}

void TDLCA_CBIR::Shutdown()
{
  while (true);
}

