#include "../../BenchmarkNode.h"
#include <stdint.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

#define IMG_W       64
#define IMG_H       64
#define WIN         8
#define SHIFT       64

#define NUM_THREADS 4

void* Disparity_Map_thread(void* thread_arg);

class SW_Disparity_Map : public BenchmarkNode
{
  int dataSize;
  int thread;
        
  float * Sum;
  float (*ILeft)[IMG_H + WIN][IMG_W + WIN];
  float (*IRight)[IMG_H + WIN][IMG_W + WIN];
  float *(*ILeft_p)[SHIFT][IMG_H + WIN][IMG_W + WIN];
  float (*IRight_mov)[SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  float (*SAD)[SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  float (*integralRow)[SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  float (*integralImg)[SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  float *(*IImg1)[IMG_H][IMG_W][SHIFT];
  float *(*IImg2)[IMG_H][IMG_W][SHIFT];
  float *(*IImg3)[IMG_H][IMG_W][SHIFT];
  float *(*IImg4)[IMG_H][IMG_W][SHIFT];
  float (*retDisp)[IMG_H][IMG_W][SHIFT][1];
public:	
  void DispMapCompSAD(float in_ILeft[1], float in_IRight_mov[1], float out_SAD[1]);
  void DispMapIntegSum(float *Sum,float in_reset, float in_val[1], float out_sum[1]);
  void DispMapFindDisp(float* internal_min, float* internal_index, float in_IImg1[1], float in_IImg2[1], float in_IImg3[1], float in_IImg4[1], float in_index, float out_Disp[1]);

  //public:
  SW_Disparity_Map()
  {
    std::cin >> dataSize;
  }
  virtual void Initialize(int threadID, int procID);
  virtual void Run();
  virtual void Shutdown();
};

BENCH_DECL(SW_Disparity_Map);

struct thread_arg1_t {

  int tid;
  int dataSize;

  float * Sum;
  float (*ILeft)[IMG_H + WIN][IMG_W + WIN];
  float (*IRight)[IMG_H + WIN][IMG_W + WIN];
  float *(*ILeft_p)[SHIFT][IMG_H + WIN][IMG_W + WIN];
  float (*IRight_mov)[SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  float (*SAD)[SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  float (*integralRow)[SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  float (*integralImg)[SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  float *(*IImg1)[IMG_H][IMG_W][SHIFT];
  float *(*IImg2)[IMG_H][IMG_W][SHIFT];
  float *(*IImg3)[IMG_H][IMG_W][SHIFT];
  float *(*IImg4)[IMG_H][IMG_W][SHIFT];
  float (*retDisp)[IMG_H][IMG_W][SHIFT][1];

  SW_Disparity_Map * benchmark;
};

void SW_Disparity_Map::DispMapCompSAD(float in_ILeft[1], float in_IRight_mov[1], float out_SAD[1])
{
  out_SAD[0] = in_ILeft[0] * in_ILeft[0] - 2 * in_ILeft[0] * in_IRight_mov[0] + in_IRight_mov[0] * in_IRight_mov[0];
}
void SW_Disparity_Map::DispMapIntegSum(float* internal_sum,float in_reset, float in_val[1], float out_sum[1])
{
  //	float internal_sum = 0.f;
  if (in_reset == 0.f)
    *internal_sum = 0.f;
  out_sum[0] = *internal_sum + in_val[0];
  *internal_sum = out_sum[0];
}
void SW_Disparity_Map::DispMapFindDisp(float* internal_min, float* internal_index, float in_IImg1[1], float in_IImg2[1], float in_IImg3[1], float in_IImg4[1], float in_index, float out_Disp[1])
{
  // Compute SAD (Sum of Absolute Differences) value
  float SAD = in_IImg1[0] + in_IImg2[0] - in_IImg3[0] - in_IImg4[0];

  // Check for minimum SAD value and return its index (i.e. disparity value)
  //static float internal_min = 0.f, internal_index = 0.f;
  if (in_index == 0.f || SAD < *internal_min) {
    *internal_min = SAD;
    *internal_index = in_index;
  }
  out_Disp[0] = *internal_index;
}
void SW_Disparity_Map::Initialize(int threadID, int procID)
{
  thread = threadID;
  Sum=new float[NUM_THREADS];
  ILeft = new float[dataSize][IMG_H + WIN][IMG_W + WIN];
  IRight = new float[dataSize][IMG_H + WIN][IMG_W + WIN];
  ILeft_p = new float*[dataSize][SHIFT][IMG_H + WIN][IMG_W + WIN];
  IRight_mov = new float[dataSize][SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  SAD = new float[dataSize][SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  integralRow = new float[dataSize][SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  integralImg = new float[dataSize][SHIFT][IMG_H + WIN][IMG_W + WIN][1];
  IImg1 = new float*[dataSize][IMG_H][IMG_W][SHIFT];
  IImg2 = new float*[dataSize][IMG_H][IMG_W][SHIFT];
  IImg3 = new float*[dataSize][IMG_H][IMG_W][SHIFT];
  IImg4 = new float*[dataSize][IMG_H][IMG_W][SHIFT];
  retDisp = new float[dataSize][IMG_H][IMG_W][SHIFT][1];
  for (int d = 0; d < NUM_THREADS; d++)
    Sum[d]=0;
			
    	
  for (int d = 0; d < dataSize; d++) {
    for (int i = 0; i < IMG_H + WIN; i++) {
      for (int j = 0; j < IMG_W + WIN; j++) {
	ILeft[d][i][j] = IRight[d][i][j] = 0.0;
      }
    }
    for (int k = 0; k < SHIFT; k++) {
      for (int i = 0; i < IMG_H + WIN; i++) {
	for (int j = 0; j < k; j++) {
	  IRight_mov[d][k][i][j][0] = 0.0;
	}
	for (int j = 0; j < IMG_W + WIN - k; j++) {
	  IRight_mov[d][k][i][j+k][0] = IRight[d][i][j];
	}
	for (int j = 0; j < IMG_W + WIN; j++) {
	  ILeft_p[d][k][i][j] = &(ILeft[d][i][j]);
	  SAD[d][k][i][j][0] = 255.0;
	  integralRow[d][k][i][j][0] = 0.0;
	  integralImg[d][k][i][j][0] = 0.0;
	}
      }
      for (int i = 0; i < IMG_H; i++) {
	for (int j = 0; j < IMG_W; j++) {
	  IImg1[d][i][j][k] = (float*) integralImg[d][k][i+WIN][j+WIN];
	  IImg2[d][i][j][k] = (float*) integralImg[d][k][i+1][j+1];
	  IImg3[d][i][j][k] = (float*) integralImg[d][k][i+1][j+WIN];
	  IImg4[d][i][j][k] = (float*) integralImg[d][k][i+WIN][j+1];
	  retDisp[d][i][j][k][0] = 0.0;
	}
      }
    }
  }
}
void SW_Disparity_Map::Run()
{ 
  pthread_t thread_id[NUM_THREADS];
  struct thread_arg1_t thread1_arg[NUM_THREADS];
  for(int p = 0; p < NUM_THREADS; p++) {
    thread1_arg[p].tid = p;
    thread1_arg[p].dataSize = dataSize;
    thread1_arg[p].Sum = Sum;
    thread1_arg[p].ILeft = ILeft;
    thread1_arg[p].IRight = IRight;
    thread1_arg[p].ILeft_p = ILeft_p;
    thread1_arg[p].IRight_mov= IRight_mov;
    thread1_arg[p].SAD = SAD;
    thread1_arg[p].integralRow=integralRow;
    thread1_arg[p].integralImg=integralImg;
    thread1_arg[p].IImg1=IImg1;
    thread1_arg[p].IImg2=IImg2;
    thread1_arg[p].IImg3=IImg3;
    thread1_arg[p].IImg4=IImg4;
    thread1_arg[p].retDisp=retDisp;
    thread1_arg[p].benchmark = this;
    pthread_create(&thread_id[p], NULL, Disparity_Map_thread, (void *)&thread1_arg[p]);
  }
  for(int p = 0; p < NUM_THREADS; p++) {
    pthread_join(thread_id[p], NULL);
  }
}


void* Disparity_Map_thread(void* thread_arg)
{
  struct thread_arg1_t *my_thread_arg = (struct thread_arg1_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float (* Sum)=my_thread_arg->Sum;
  float (*ILeft)[IMG_H + WIN][IMG_W + WIN]=my_thread_arg->ILeft;
  float (*IRight)[IMG_H + WIN][IMG_W + WIN]=my_thread_arg->IRight;
  float *(*ILeft_p)[SHIFT][IMG_H + WIN][IMG_W + WIN]=my_thread_arg->ILeft_p;
  float (*IRight_mov)[SHIFT][IMG_H + WIN][IMG_W + WIN][1]=my_thread_arg->IRight_mov;
  float (*SAD)[SHIFT][IMG_H + WIN][IMG_W + WIN][1]=my_thread_arg->SAD;
  float (*integralRow)[SHIFT][IMG_H + WIN][IMG_W + WIN][1]=my_thread_arg->integralRow;
  float (*integralImg)[SHIFT][IMG_H + WIN][IMG_W + WIN][1]=my_thread_arg->integralImg;
  float *(*IImg1)[IMG_H][IMG_W][SHIFT]=my_thread_arg->IImg1;
  float *(*IImg2)[IMG_H][IMG_W][SHIFT]=my_thread_arg->IImg2;
  float *(*IImg3)[IMG_H][IMG_W][SHIFT]=my_thread_arg->IImg3;
  float *(*IImg4)[IMG_H][IMG_W][SHIFT]=my_thread_arg->IImg4;
  float (*retDisp)[IMG_H][IMG_W][SHIFT][1]=my_thread_arg->retDisp;
  SW_Disparity_Map* benchmark = my_thread_arg->benchmark;       

  int PP = SHIFT/NUM_THREADS;
        
  for (int d = 0; d < dataSize; d++)
    for(int k = tid*PP; k < (tid+1)*PP; k++)
      for (int i = 0; i < IMG_H + WIN; i++)
	for (int j = 0; j < IMG_W + WIN; j++)
	  benchmark->DispMapCompSAD(ILeft_p[d][k][i][j], IRight_mov[d][k][i][j], SAD[d][k][i][j]);
	
  Sum[tid]=0;   
  for (int d = 0; d < dataSize; d++)
    for(int k = tid*PP; k < (tid+1)*PP; k++)
      for (int i = 0; i < IMG_H + WIN; i++)
	for (int j = 0; j < IMG_W + WIN; j++)
	  benchmark->DispMapIntegSum(&Sum[tid],j, SAD[d][k][i][j], integralRow[d][k][i][j]);

  for (int d = 0; d < dataSize; d++)
    for(int k = tid*PP; k < (tid+1)*PP; k++)
      for (int j = 0; j < IMG_W + WIN; j++)
	for (int i = 0; i < IMG_H + WIN; i++)
	  benchmark->DispMapIntegSum(&Sum[tid],i, integralRow[d][k][i][j], integralImg[d][k][i][j]);

  float internal_min = 0.0f;
  float internal_index = 0.0f;
  int MM = IMG_H/NUM_THREADS;
  for (int d = 0; d < dataSize; d++)
    for(int i = tid*MM; i < (tid+1)*MM; i++)
      for (int j = 0; j < IMG_W; j++)
	for (int k = 0; k < SHIFT; k++)
	  benchmark->DispMapFindDisp(&internal_min, &internal_index, IImg1[d][i][j][k], IImg2[d][i][j][k], IImg3[d][i][j][k], IImg4[d][i][j][k], k, retDisp[d][i][j][k]);
}

void SW_Disparity_Map::Shutdown()
{
  delete [] ILeft;
  delete [] IRight;
  delete [] ILeft_p;
  delete [] IRight_mov;
  delete [] SAD;
  delete [] integralRow;
  delete [] integralImg;
  delete [] IImg1;
  delete [] IImg2;
  delete [] IImg3;
  delete [] IImg4;
  delete [] retDisp;
	
  while(true);
}
