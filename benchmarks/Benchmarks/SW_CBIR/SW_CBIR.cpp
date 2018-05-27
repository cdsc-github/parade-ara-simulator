#include "../../BenchmarkNode.h"
#include "cnn_cfg.hpp"
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>

#define NUM_LAYER 1
#define K 100
#define N_FEA 96
#define N_QUERY 16
#define N_CENT 512
#define FOUT11 32

int ly[13][9] = {
  {DATA, FOUT11, CONV11_I, CONV11_I, CONV11_R, CONV11_R, 0, 0, RELU},
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

class SW_CBIR : public BenchmarkNode
{
  int dataSize;
  int thread;

  float* in;
  float* Cout;
  float* Pout;
  float* weight;
  float* bias;

  float (*dataset)[N_FEA];
  float (*query)[N_FEA];
  float (*cent)[N_FEA];
  float *dists;

  void feature_extraction();

  void indexing();

  void ConvCore(float* in, float* Cout, float* weight, float* bias, int fin,
    int fout, int finr, int finc, int foutr, int foutc, int ksize, int kstride);

  void PoolKernel(float* Cout, float* Pout, int fout, int foutr, int foutc,
    int ksize, int kstride);

  void ReluKernel(float* Pout, int fout, int foutr, int foutc);

  void FcnReorder(float Pout[HWFOut][HWFR][HWFC], int frow, int fcol);

  void find_nearest_neighbors(float* query, float* dists);

public:
  SW_CBIR()
  {
    std::cin >> dataSize;
  }
  virtual void Initialize(int threadID, int procID);
  virtual void Run();
  virtual void Shutdown();
};

BENCH_DECL(SW_CBIR);

void SW_CBIR::Initialize(int threadID, int procID)
{
  thread = threadID;

  in = new float[CONV11_I*CONV11_I*FOUT11];
  Cout = new float[CONV11_I*CONV11_I*FOUT11];
  Pout = new float[CONV11_I*CONV11_I*FOUT11];
  weight = new float[512*512*3*3];
  bias = new float[512];

  dataset = new float[dataSize][N_FEA];
  query = new float[N_QUERY][N_FEA];
  cent = new float[N_CENT][N_FEA];
  dists = new float[dataSize];
}

void SW_CBIR::Run()
{
  feature_extraction();

  DumpStats();

  indexing();

  DumpStats();

  for (int i = 0; i < N_QUERY; i++) {
    find_nearest_neighbors(query[i], dists);
  }
}

void SW_CBIR::feature_extraction()
{
  for (int i = 0; i < NUM_LAYER; i++) {
    ConvCore(in, Cout, weight, bias, ly[i][0], ly[i][1], ly[i][2], ly[i][3],
      ly[i][4], ly[i][5], KSIZE, STRIDE);
   if (ly[i][8]) ReluKernel(Cout, ly[i][1], ly[i][4], ly[i][5]);
   if (ly[i][7]) PoolKernel(Cout, Pout, ly[i][1], ly[i][4], ly[i][5], 2, 2);
   // if (ly[i][6]) FcnReorder(Cout, mprow, mcol);
 }
}

void SW_CBIR::ConvCore(float* in, float* Cout, float* weight, float* bias,
  int fin, int fout, int finr, int finc, int foutr, int foutc,
  int ksize, int kstride)
{
  int Couti, Coutj, Coutk;
  int Cini;
  int Cwi, Cwj;

  for (Coutj = 0; Coutj < foutr; Coutj++) {
    for (Coutk = 0; Coutk < foutc; Coutk++) {
      for (Couti = 0; Couti < fout; Couti++) {
        Cout[Couti * foutr * foutc + Coutj * foutc + Coutk] = bias[Couti];

        for (Cini = 0; Cini < fin; Cini++) {
          for (Cwi = 0; Cwi < ksize; Cwi++) {
            for (Cwj = 0; Cwj < ksize; Cwj++) {
              Cout[Couti * foutr * foutc + Coutj * foutc + Coutk] +=
                in[Cini*finr*finc + (Coutj*kstride+Cwi)*finc + (Coutk*kstride+Cwj)] *
                weight[Couti*fin*ksize*ksize + Cini*ksize*ksize + Cwi*ksize + Cwj];
            }
          }
        }
      }
    }
  }
}

void SW_CBIR::PoolKernel(float* Cout, float* Pout, int fout, int foutr,
  int foutc, int ksize, int kstride)
{
  int Pouti, Poutj, Poutk;
  int Pini, Pinj;

  for (Pouti = 0; Pouti < fout; Pouti++) {
    for (Poutj = 0; Poutj < foutr; Poutj++) {
      for (Poutk = 0; Poutk < foutc; Poutk++) {
        Pout[Pouti * foutr * foutc + Poutj * foutc + Poutk] =
          Cout[Pouti * foutr * foutc + Poutj * foutc + Poutk];

        for (Pini = 0; Pini < ksize; Pini++) {
          for (Pinj = 0; Pinj < ksize; Pinj++) {
            if (Cout[Pouti*foutr*foutc + (Poutj*kstride+Pini)*foutc + (Poutk*kstride+Pinj)] > Pout[Pouti * foutr * foutc + Poutj * foutc + Poutk])
              Pout[Pouti * foutr * foutc + Poutj * foutc + Poutk] =
                Cout[Pouti*foutr*foutc + (Poutj*kstride+Pini)*foutc + (Poutk*kstride+Pinj)];
          }
        }
      }
    }
  }
}

void SW_CBIR::ReluKernel(float* Pout, int fout, int foutr, int foutc)
{
  for (int i = 0; i < fout * foutr * foutc; i++) {
    if (Pout[i] < 0.0) {
      Pout[i] = 0.0;
    }
  }
}

void SW_CBIR::FcnReorder(float Pout[HWFOut][HWFR][HWFC], int frow, int fcol)
{
  int i = 0;
  int j = 0;
  int Si = 0;
  int Sj = 0;
  float tmp[HWFOut][HWFOut];

  for (int l = 0; l < frow * fcol; l += HWFOut) {
    for (int s = 0; s < HWFOut; s++) {
      for (int p = 0; p < HWFOut - 1; p++) {
        for (int q = 0; q < HWFOut; q++) {
          tmp[p][q] = tmp[p + 1][q];
        }
      }
      for (int q = 0; q < HWFOut; q++) {
        tmp[HWFOut - 1][q] = Pout[q][i][j];
      }
      j += 1;
      if (j >= fcol) {
        j = 0;
        i += 1;
      }
    }
    i = Si;
    j = Sj;

    for (int s = 0; s < HWFOut; s++) {
      for (int q = 0; q < HWFOut; q++) {
        Pout[q][i][j] = tmp[q][0];
      }
      for (int p = 0; p < HWFOut - 1; p++) {
        for (int q = 0; q < HWFOut; q++) {
          tmp[q][p] = tmp[q][p + 1];
        }
      }
      j += 1;
      if (j >= fcol) {
        j = 0;
        i += 1;
      }
    }
    Si = i;
    Sj = j;
  }
}

void SW_CBIR::find_nearest_neighbors(float* query, float* dists)
{
  for (int i = 0; i < dataSize; i++) {
    float dist = 0;
    for (int j = 0; j < N_FEA; j++) {
      float q = dataset[i][j] - query[j];
      dist += (q * q);
    }
    dists[i] = dist;
  }

  std::vector<float> res(dists, dists + dataSize);
  std::partial_sort(res.begin(), res.begin() + K, res.end());
}

void SW_CBIR::indexing()
{
  for (int i = 0; i < N_CENT; i++) {
    for (int j = 0 ; j < N_QUERY; j++) {
      float sum = 0;
      for (int k = 0; k < N_FEA; k++) {
        sum += -2.0 * query[j][k] * cent[i][k];
      }
    }
  }
}

void SW_CBIR::Shutdown()
{
  while (true);
}

