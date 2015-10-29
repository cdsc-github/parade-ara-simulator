#include "../../BenchmarkNode.h"
#include <iostream>
#include <cmath>

#define ITER_COUNT 2
#define NUM_THREADS 4

struct thread_arg1_t {
  int tid;
  int dataSize;
  float* v1;
  float BoundaryScale;
  float nu;
};

struct thread_arg2_t {
  int tid;
  int dataSize;
  float* u;
  float* g;
};

struct thread_arg3_t {
  int tid;
  int dataSize;
  float* f;
  float* conv;
  float sigma2;
};

struct thread_arg4_t {
  int tid;
  int dataSize;
  float* u;
  float* g;
  float* f;
  float* ret_u;
};

struct thread_arg5_t {
  int tid;
  int dataSize;
  float* v1;
  float PostScale;
};

void* denoise1_thread(void* thread_arg);
void* V_cal_thread(void* thread_arg);
void* deblur1_thread(void* thread_arg);
void* deblur2_thread(void* thread_arg);
void* z1_thread(void* thread_arg);
void* z2_thread(void* thread_arg);
void* outer_thread(void* thread_arg);

class SW_Deblur : public BenchmarkNode
{
        float* u;
        float* g;
        float* f;
        float* conv;
	float* ret_u;
        int dataSize;
        int thread;
public:
	SW_Deblur()
	{
		std::cin >> dataSize;
	}
        virtual void Initialize(int threadID, int procID);
        virtual void Run();
        virtual void Shutdown();
};

BENCH_DECL(SW_Deblur);

void GaussianBlur(float *u, const int Size[3], float Ksigma)
{
#define GAUSSIAN_NUMSTEPS 3
  float lambda = (Ksigma*Ksigma)/(2.0*GAUSSIAN_NUMSTEPS);
  float nu = (1.0 + 2.0*lambda - sqrt(1.0 + 4.0*lambda))/(2.0*lambda);
  float BoundaryScale = 1.0/(1.0 - nu);
  float PostScale = pow(nu/lambda,3*GAUSSIAN_NUMSTEPS);
  int i,j,k,counter;
  //assume cubic size
  int dataSize = Size[0];

  pthread_t thread_id[NUM_THREADS];
  int step;
  for( step = 0; step < 3; step++) {
    struct thread_arg1_t thread1_arg[NUM_THREADS];
    for(int p = 0; p < NUM_THREADS; p++) {
      thread1_arg[p].tid = p;
      thread1_arg[p].dataSize = dataSize;
      thread1_arg[p].v1 = u;
      thread1_arg[p].BoundaryScale = BoundaryScale;
      thread1_arg[p].nu = nu;
      pthread_create(&thread_id[p], NULL, V_cal_thread, (void *)&thread1_arg[p]);
    }
    for(int p = 0; p < NUM_THREADS; p++) {
      pthread_join(thread_id[p], NULL);
    }
    //std::cout << "gaussian V_cal done" << std::endl;

    struct thread_arg1_t thread5_arg[NUM_THREADS];
    for(int p = 0; p < NUM_THREADS; p++) {
      thread5_arg[p].tid = p;
      thread5_arg[p].dataSize = dataSize;
      thread5_arg[p].v1 = u;
      thread5_arg[p].BoundaryScale = BoundaryScale;
      thread5_arg[p].nu = nu;
      pthread_create(&thread_id[p], NULL, z1_thread, (void *)&thread5_arg[p]);
    }
    for(int p = 0; p < NUM_THREADS; p++) {
      pthread_join(thread_id[p], NULL);
    }
    //std::cout << "gaussian z1 done" << std::endl;

    struct thread_arg1_t thread6_arg[NUM_THREADS];
    for(int p = 0; p < NUM_THREADS; p++) {
      thread6_arg[p].tid = p;
      thread6_arg[p].dataSize = dataSize;
      thread6_arg[p].v1 = u;
      thread6_arg[p].BoundaryScale = BoundaryScale;
      thread6_arg[p].nu = nu;
      pthread_create(&thread_id[p], NULL, z2_thread, (void *)&thread6_arg[p]);
    }
    for(int p = 0; p < NUM_THREADS; p++) {
      pthread_join(thread_id[p], NULL);
    }
    //std::cout << "gaussian z2 done" << std::endl;

  }

  struct thread_arg5_t thread7_arg[NUM_THREADS];
  for(int p = 0; p < NUM_THREADS; p++) {
    thread7_arg[p].tid = p;
    thread7_arg[p].dataSize = dataSize;
    thread7_arg[p].v1 = u;
    thread7_arg[p].PostScale = PostScale;
    pthread_create(&thread_id[p], NULL, outer_thread, (void *)&thread7_arg[p]);
  }
  for(int p = 0; p < NUM_THREADS; p++) {
    pthread_join(thread_id[p], NULL);
  }
  //std::cout << "gaussian outer done" << std::endl;
}

void* z1_thread(void* thread_arg) {
  struct thread_arg1_t *my_thread_arg = (struct thread_arg1_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* u = my_thread_arg->v1;
  float BoundaryScale = my_thread_arg->BoundaryScale;
  float nu = my_thread_arg->nu;

  int i,j,counter;
  int COUNTER_SIZE = dataSize/NUM_THREADS;
  int upper_bound = (tid == NUM_THREADS-1) ? dataSize : (tid+1)*COUNTER_SIZE;
  for(i = tid*COUNTER_SIZE; i < upper_bound; i++) {
    for(j = 0; j < dataSize; j++) {
      u[i * dataSize + j] = u[i * dataSize + j] * BoundaryScale;
    }
  }

  for(counter = 1; counter < dataSize; counter++){
    for(i = tid*COUNTER_SIZE; i < upper_bound; i++) {
      for(j = 0; j < dataSize; j++) {
	u[counter * dataSize * dataSize + i * dataSize + j] = u[counter * dataSize * dataSize + i * dataSize + j] + u[(counter - 1) * dataSize * dataSize + i * dataSize + j] * nu;
      }
    }
  }
}

void* z2_thread(void* thread_arg) {
  struct thread_arg1_t *my_thread_arg = (struct thread_arg1_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* u = my_thread_arg->v1;
  float BoundaryScale = my_thread_arg->BoundaryScale;
  float nu = my_thread_arg->nu;

  int i,j,counter;
  int COUNTER_SIZE = dataSize/NUM_THREADS;
  int upper_bound = (tid == NUM_THREADS-1) ? dataSize : (tid+1)*COUNTER_SIZE;
  for(int i = tid*COUNTER_SIZE; i < upper_bound; i++) {
    for(j = 0; j < dataSize; j++){
      u[(dataSize - 1) * dataSize * dataSize + i * dataSize + j] = u[(dataSize - 1) * dataSize * dataSize + i * dataSize + j] * BoundaryScale;
    }
  }

  for(counter = dataSize-2; counter >=0; counter--) {
    for(i = tid*COUNTER_SIZE; i < upper_bound; i++) {
      for( j = 0; j < dataSize; j++){
	u[counter * dataSize * dataSize + i * dataSize + j] = u[counter * dataSize * dataSize + i * dataSize + j] + u[(counter + 1) * dataSize * dataSize + i * dataSize + j] * nu;
      }
    }
  }
}

void* outer_thread(void* thread_arg) {
  struct thread_arg5_t *my_thread_arg = (struct thread_arg5_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* u = my_thread_arg->v1;
  float PostScale = my_thread_arg->PostScale;

  int i,j,counter;
  int COUNTER_SIZE = dataSize/NUM_THREADS;
  int upper_bound = (tid == NUM_THREADS-1) ? dataSize : (tid+1)*COUNTER_SIZE;
  for(int counter = tid*COUNTER_SIZE; counter < upper_bound; counter++) {
    for(i = 0; i < dataSize; i++) {
      for( j = 0; j < dataSize; j++) {
	u[counter * dataSize * dataSize + i * dataSize + j] *= PostScale;
      }
    }
  }
}

void* V_cal_thread(void* thread_arg) {
  struct thread_arg1_t *my_thread_arg = (struct thread_arg1_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* v1 = my_thread_arg->v1;
  float BoundaryScale = my_thread_arg->BoundaryScale;
  float nu = my_thread_arg->nu;

  int i,j,k,counter;
  int COUNTER_SIZE = dataSize/NUM_THREADS;
  int upper_bound = (tid == NUM_THREADS-1) ? dataSize : (tid+1)*COUNTER_SIZE;
  for(int counter = tid*COUNTER_SIZE; counter < upper_bound; counter++) {
    for(i = 0; i < dataSize; i++) {
      v1[counter * dataSize * dataSize + dataSize * i] = v1[counter * dataSize * dataSize + dataSize * i]* BoundaryScale;
    }
    for(j = 1; j < dataSize; j++) {
      for( i = 0; i < dataSize; i++) {
	v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + i * dataSize + j-1] * nu;
      }
    }

    for(i = 0; i < dataSize; i++) {
      v1[counter * dataSize * dataSize + i * dataSize + dataSize-1] = v1[counter * dataSize * dataSize + i * dataSize + dataSize-1] * BoundaryScale;
    }
    for(j = dataSize-2; j >= 0; j--) {
      for( i = 0; i < dataSize; i++) {
	v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + i * dataSize + j+1] * nu;
      }
    }

    for(j = 0; j < dataSize; j++) {
      v1[counter * dataSize * dataSize + j] = v1[counter * dataSize * dataSize + j] * BoundaryScale;
    }
    for(i = 1; i < dataSize; i++) {
      for( j = 0; j < dataSize; j++) {
	v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + (i - 1) * dataSize + j] * nu;
      }
    }

    for(j = 0; j < dataSize; j++) {
      v1[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] = v1[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] * BoundaryScale;
    }
    for(i = dataSize-2; i >=0; i--) {
      for( j = 0; j < dataSize; j++) {
	v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + (i + 1) * dataSize + j] * nu;
      } 
    }
  }

}

void* denoise1_thread(void* thread_arg) {
  struct thread_arg2_t *my_thread_arg = (struct thread_arg2_t *)thread_arg;
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

void* deblur1_thread(void* thread_arg) {
  struct thread_arg3_t *my_thread_arg = (struct thread_arg3_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* f = my_thread_arg->f;
  float* conv  = my_thread_arg->conv;
  float sigma2 = my_thread_arg->sigma2;

#define CENTER   (m+M*(n+N*p))
  int M = dataSize;
  int N = dataSize;
  int P = dataSize;
  int PP = dataSize/NUM_THREADS;
  for(int p = tid*PP+1; p <= (tid+1)*PP; p++) {
    for(int n = 0; n < N; n++) {
      for(int m = 0; m < M; m++) {
	float r = conv[CENTER] * f[CENTER] / sigma2;
	r = (r * (2.38944f + r * (0.950037f + r))) / (4.65314f + r * (2.57541f + r * (1.48937f + r)));
	conv[CENTER] -= f[CENTER] * r;
      }
    }
  }
}

void* deblur2_thread(void* thread_arg) {
  struct thread_arg4_t *my_thread_arg = (struct thread_arg4_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* u = my_thread_arg->u;
  float* g = my_thread_arg->g;
  float* f = my_thread_arg->f;
  float* ret_u = my_thread_arg->ret_u;


#define CENTER   (m+M*(n+N*p))
#define RIGHT    (m+M*(n+N*p)+M)
#define LEFT     (m+M*(n+N*p)-M)
#define DOWN     (m+M*(n+N*p)+1)
#define UP       (m+M*(n+N*p)-1)
#define ZOUT     (m+M*(n+N*p+N))
#define ZIN      (m+M*(n+N*p-N))        
#define DEFAULT_DT       0.0001
#define DEFAULT_KSIGMA   1.8
#define DEFAULT_SIGMA    0.008
#define DEFAULT_LAMBDA   0.085

  float Ksigma = DEFAULT_KSIGMA;
  float sigma = DEFAULT_SIGMA;
  float lambda = DEFAULT_LAMBDA;
  float sigma2 = 0.0f;
  float gamma = 0.0f;
  float dt = DEFAULT_DT;
  int M = dataSize;
  int N = dataSize;
  int P = dataSize;
  int PP = dataSize/NUM_THREADS;
  for(int p = tid*PP+1; p <= (tid+1)*PP; p++) {
    for(int n = 1; n < N - 1; n++) {
      for(int m = 1; m < M - 1; m++) {
	ret_u[CENTER] = (u[CENTER] + dt * (u[RIGHT] * g[RIGHT] + u[LEFT] * g[LEFT] + u[DOWN] * g[DOWN] + u[UP] * g[UP] + u[ZOUT] * g[ZOUT] + u[ZIN] * g[ZIN] + gamma * f[CENTER]) ) / (1.0f + dt*(g[RIGHT] + g[LEFT] + g[DOWN] + g[UP] + g[ZOUT] + g[ZIN]));

      }
    }
  }
}

void SW_Deblur::Initialize(int threadID, int procID)
{
        thread = threadID;
        u = new float[dataSize * dataSize * dataSize];
        g = new float[dataSize * dataSize * dataSize];
        f = new float[dataSize * dataSize * dataSize];
        conv = new float[dataSize * dataSize * dataSize];
	ret_u = new float[dataSize * dataSize * dataSize];
	for(int i = 0; i < dataSize * dataSize * dataSize; i++)
	{
		u[i] = g[i] = f[i] = conv[i] = ret_u[i] = 0.0f;
	}
}
void SW_Deblur::Run()
{
#define DEFAULT_DT       0.0001
#define DEFAULT_KSIGMA   1.8
#define DEFAULT_SIGMA    0.008
#define DEFAULT_LAMBDA   0.085

        float Ksigma = DEFAULT_KSIGMA;
        float sigma = DEFAULT_SIGMA;
        float lambda = DEFAULT_LAMBDA;
        float sigma2 = 0.0f;
        float gamma = 0.0f;
        float dt = DEFAULT_DT;
        int Size[3] = {dataSize, dataSize, dataSize};
        for(int i = 0; i < ITER_COUNT; i++)
        {
	  pthread_t thread_id[NUM_THREADS];
	  struct thread_arg2_t thread2_arg[NUM_THREADS];
	  for(int p = 0; p < NUM_THREADS; p++) {
	    thread2_arg[p].tid = p;
	    thread2_arg[p].dataSize = dataSize;
	    thread2_arg[p].u = u;
	    thread2_arg[p].g = g;
	    pthread_create(&thread_id[p], NULL, denoise1_thread, (void *)&thread2_arg[p]);
	  }
	  for(int p = 0; p < NUM_THREADS; p++) {
	    pthread_join(thread_id[p], NULL);
	    }//*/
	  //std::cout << "denoise1 done" << std::endl;

	  //memcpy(conv, u, sizeof(float) * M * N * P);
	  GaussianBlur(conv, Size, Ksigma);
	  //std::cout << "gaussian1 done" << std::endl;

	  struct thread_arg3_t thread3_arg[NUM_THREADS];
	  for(int p = 0; p < NUM_THREADS; p++) {
	    thread3_arg[p].tid = p;
	    thread3_arg[p].dataSize = dataSize;
	    thread3_arg[p].f = f;
	    thread3_arg[p].conv = conv;
	    thread3_arg[p].sigma2 = sigma2;
	    pthread_create(&thread_id[p], NULL, deblur1_thread, (void *)&thread3_arg[p]);
	  }
	  for(int p = 0; p < NUM_THREADS; p++) {
	    pthread_join(thread_id[p], NULL);
	  }
	  //std::cout << "deblur1 done" << std::endl;

	  GaussianBlur(conv, Size, Ksigma);
	  //std::cout << "gaussian2 done" << std::endl;

	  struct thread_arg4_t thread4_arg[NUM_THREADS];
	  for(int p = 0; p < NUM_THREADS; p++) {
	    thread4_arg[p].tid = p;
	    thread4_arg[p].dataSize = dataSize;
	    thread4_arg[p].u = u;
	    thread4_arg[p].g = g;
	    thread4_arg[p].f = f;
	    thread4_arg[p].ret_u = ret_u;
	    pthread_create(&thread_id[p], NULL, deblur2_thread, (void *)&thread4_arg[p]);
	  }
	  for(int p = 0; p < NUM_THREADS; p++) {
	    pthread_join(thread_id[p], NULL);
	  }
	  //std::cout << "deblur2 done" << std::endl;

        }
}
void SW_Deblur::Shutdown()
{
        while(true);
}

