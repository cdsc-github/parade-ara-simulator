#include "../../BenchmarkNode.h"
#include <iostream>
#include <cmath>
#include <pthread.h>

#define NUM_THREADS 4

struct thread_arg1_t {
  int tid;
  int dataSize;
  float* u;
  float* g;
};

struct thread_arg2_t {
  int tid;
  int dataSize;
  float* u;
  float* f;
  float* g;
  float* u_tmp;
};

struct thread_arg3_t {
  int tid;
  int dataSize;
  float* u;
  float* u_tmp;
};

void* denoise1_thread(void* thread_arg);
void* denoise2_thread(void* thread_arg);
void* denoise3_thread(void* thread_arg);

class SW_Denoise : public BenchmarkNode
{
	float* u;
        float* u_tmp;
	float* g;
	float* f;
	int dataSize;
	int thread;
public:
	SW_Denoise()
	{
		std::cin >> dataSize;
	}
	virtual void Initialize(int threadID, int procID);
	virtual void Run();
	virtual void Shutdown();
};

BENCH_DECL(SW_Denoise);

#define ITER_COUNT 2 

void SW_Denoise::Initialize(int threadID, int procID)
{
        thread = threadID;
        u = new float[dataSize * dataSize * dataSize];
        u_tmp = new float[dataSize * dataSize * dataSize];
        g = new float[dataSize * dataSize * dataSize];
        f = new float[dataSize * dataSize * dataSize];
	for(int i = 0; i < dataSize * dataSize * dataSize; i++)
	{
		u[i] = g[i] = f[i] = u_tmp[i] = 0.0f;
	}
}
void SW_Denoise::Run()
{
  for(int i = 0; i < ITER_COUNT; i++) {
    pthread_t thread_id[NUM_THREADS];
    struct thread_arg1_t thread1_arg[NUM_THREADS];
    for(int p = 0; p < NUM_THREADS; p++) {
      thread1_arg[p].tid = p;
      thread1_arg[p].dataSize = dataSize;
      thread1_arg[p].u = u;
      thread1_arg[p].g = g;
      pthread_create(&thread_id[p], NULL, denoise1_thread, (void *)&thread1_arg[p]);
    }
    for(int p = 0; p < NUM_THREADS; p++) {
      pthread_join(thread_id[p], NULL);
    }

    struct thread_arg2_t thread2_arg[NUM_THREADS];
    for(int p = 0; p < NUM_THREADS; p++) {
      thread2_arg[p].tid = p;
      thread2_arg[p].dataSize = dataSize;
      thread2_arg[p].u = u;
      thread2_arg[p].f = f;
      thread2_arg[p].g = g;
      thread2_arg[p].u_tmp = u_tmp;
      pthread_create(&thread_id[p], NULL, denoise2_thread, (void *)&thread2_arg[p]);
    }
    for(int p = 0; p < NUM_THREADS; p++) {
      pthread_join(thread_id[p], NULL);
    }

    /*struct thread_arg3_t thread3_arg[NUM_THREADS];
    for(int p = 0; p < NUM_THREADS; p++) {
      thread3_arg[p].tid = p;
      thread3_arg[p].dataSize = dataSize;
      thread3_arg[p].u = u;
      thread3_arg[p].u_tmp = u_tmp;
      pthread_create(&thread_id[p], NULL, denoise3_thread, (void *)&thread3_arg[p]);
    }
    for(int p = 0; p < NUM_THREADS; p++) {
      pthread_join(thread_id[p], NULL);
    }//*/
  }
}

void* denoise1_thread(void* thread_arg) {
  struct thread_arg1_t *my_thread_arg = (struct thread_arg1_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* u = my_thread_arg->u;
  float* g = my_thread_arg->g;

#define SQR(x) ((x)*(x))
#define CENTER   (m+M*(n+N*p))
#define RIGHT    (m+M*(n+N*p)+M)
#define LEFT     (m+M*(n+N*p)-M)
#define DOWN     (m+M*(n+N*p)+1)
#define UP       (m+M*(n+N*p)-1)
#define ZOUT     (m+M*(n+N*p+N))
#define ZIN      (m+M*(n+N*p-N))        
  int M = dataSize;
  int N = dataSize;
  int P = dataSize;
  int PP = dataSize/NUM_THREADS;
  for(int p = tid*PP+1; p <= (tid+1)*PP; p++) {
    for(int n = 1; n < N - 1; n++) {
      for(int m = 1; m < M - 1; m++) {
	g[CENTER] = 1.0f/sqrt(
			      SQR(u[CENTER] - u[RIGHT])
			      + SQR(u[CENTER] - u[LEFT])
			      + SQR(u[CENTER] - u[DOWN])
			      + SQR(u[CENTER] - u[UP])
			      + SQR(u[CENTER] - u[ZOUT])
			      + SQR(u[CENTER] - u[ZIN]));
      }
    }
  }
}

void* denoise2_thread(void* thread_arg) {
  struct thread_arg2_t *my_thread_arg = (struct thread_arg2_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* u = my_thread_arg->u;
  float* f = my_thread_arg->f;
  float* g = my_thread_arg->g;
  float* u_tmp = my_thread_arg->u_tmp;

#define SQR(x) ((x)*(x))
#define CENTER   (m+M*(n+N*p))
#define RIGHT    (m+M*(n+N*p)+M)
#define LEFT     (m+M*(n+N*p)-M)
#define DOWN     (m+M*(n+N*p)+1)
#define UP       (m+M*(n+N*p)-1)
#define ZOUT     (m+M*(n+N*p+N))
#define ZIN      (m+M*(n+N*p-N))        
#define DT 5.0f
  float sigma = 0.03f;
  float lambda = 1.0f;
  float vGamma = lambda / sigma;
  int M = dataSize;
  int N = dataSize;
  int P = dataSize;
  int PP = dataSize/NUM_THREADS;
  for(int p = tid*PP+1; p <= (tid+1)*PP; p++) {
    for(int n = 1; n < N - 1; n++) {
      for(int m = 1; m < M - 1; m++) {
	float r = u[CENTER] * f[CENTER] / sigma;
	r = (r * (2.38944f + r * (0.950037f + r))) / (4.65314f + r * (2.57541f + r * (1.48937f + r)));
	u_tmp[CENTER] = (u[CENTER] + DT * (u[RIGHT] * g[RIGHT] + u[LEFT] * g[LEFT] + u[DOWN] * g[DOWN] + u[UP] * g[UP] + u[ZOUT] * g[ZOUT] + u[ZIN] * g[ZIN] + vGamma * f[CENTER]*r) ) / (1.0f + DT*(g[RIGHT] + g[LEFT] + g[DOWN] + g[UP] + g[ZOUT] + g[ZIN] + vGamma));
      }
    }
  }
}

void* denoise3_thread(void* thread_arg) {
  struct thread_arg3_t *my_thread_arg = (struct thread_arg3_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* u = my_thread_arg->u;
  float* u_tmp = my_thread_arg->u_tmp;

  int M = dataSize;
  int N = dataSize;
  int P = dataSize;
  int PP = dataSize/NUM_THREADS;
  for(int p = tid*PP+1; p <= (tid+1)*PP; p++) {
    for(int n = 0; n < N; n++) {
      for(int m = 0; m < M; m++) {
	u[CENTER] = u_tmp[CENTER];
      }
    }
  }
}

void SW_Denoise::Shutdown()
{
        while(true);
}

