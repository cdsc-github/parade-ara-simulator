#include "../../BenchmarkNode.h"
#include <stdint.h>
#include <iostream>
#include <cmath>

#define ITER_COUNT 2

#define NUM_THREADS 4

struct thread_arg1_t {
  int tid;
  int dataSize;
  float* v1;
  float* v2;
  float* v3;
  float BoundaryScale;
  float nu;
};

struct thread_arg2_t {
  int tid;
  int dataSize;
  float* u1;
  float* u2;
  float* u3;
  float* v1;
  float* v2;
  float* v3;
};

struct thread_arg3_t {
  int tid;
  int dataSize;
  float* v1;
  float* v2;
  float* v3;
  float PostScale;
};

void* U_cal_thread(void* thread_arg);
void* V_cal_thread(void* thread_arg);
void* z1_thread(void* thread_arg);
void* z2_thread(void* thread_arg);
void* outer_thread(void* thread_arg);

class SW_Registration : public BenchmarkNode
{
        int dataSize;
        float* u1;
        float* u2;
        float* u3;
        float* v1;
        float* v2;
        float* v3;
public:
	SW_Registration()
	{
		std::cin >> dataSize;
	}
        virtual void Initialize(int threadID, int procID);
        virtual void Run();
        virtual void Shutdown();
};

BENCH_DECL(SW_Registration);

void V_cal(float* v1, float* v2, float* v3, int dataSize) {
  int i,j,k,counter;
  float std = 4.5f;
  float lambda = (std*std)/(2*3);
  float nu = (1 + 2*lambda - sqrtf(1+4*lambda))/(2*lambda);
  float BoundaryScale = 1/(1-nu);
  float PostScale = 1;
  for(i = 0; i < 3*3; i++)
    PostScale *= nu/lambda;

  pthread_t thread_id[NUM_THREADS];
  int step;
  for( step = 0; step < 3; step++) {
    struct thread_arg1_t thread1_arg[NUM_THREADS];
    for(int p = 0; p < NUM_THREADS; p++) {
      thread1_arg[p].tid = p;
      thread1_arg[p].dataSize = dataSize;
      thread1_arg[p].v1 = v1;
      thread1_arg[p].v2 = v2;
      thread1_arg[p].v3 = v3;
      thread1_arg[p].BoundaryScale = BoundaryScale;
      thread1_arg[p].nu = nu;
      pthread_create(&thread_id[p], NULL, V_cal_thread, (void *)&thread1_arg[p]);
    }
    for(int p = 0; p < NUM_THREADS; p++) {
      pthread_join(thread_id[p], NULL);
    }

    struct thread_arg1_t thread3_arg[NUM_THREADS];
    for(int p = 0; p < NUM_THREADS; p++) {
      thread3_arg[p].tid = p;
      thread3_arg[p].dataSize = dataSize;
      thread3_arg[p].v1 = v1;
      thread3_arg[p].v2 = v2;
      thread3_arg[p].v3 = v3;
      thread3_arg[p].BoundaryScale = BoundaryScale;
      thread3_arg[p].nu = nu;
      pthread_create(&thread_id[p], NULL, z1_thread, (void *)&thread3_arg[p]);
    }
    for(int p = 0; p < NUM_THREADS; p++) {
      pthread_join(thread_id[p], NULL);
    }

    struct thread_arg1_t thread4_arg[NUM_THREADS];
    for(int p = 0; p < NUM_THREADS; p++) {
      thread4_arg[p].tid = p;
      thread4_arg[p].dataSize = dataSize;
      thread4_arg[p].v1 = v1;
      thread4_arg[p].v2 = v2;
      thread4_arg[p].v3 = v3;
      thread4_arg[p].BoundaryScale = BoundaryScale;
      thread4_arg[p].nu = nu;
      pthread_create(&thread_id[p], NULL, z2_thread, (void *)&thread4_arg[p]);
    }
    for(int p = 0; p < NUM_THREADS; p++) {
      pthread_join(thread_id[p], NULL);
    }
  }

  struct thread_arg3_t thread5_arg[NUM_THREADS];
  for(int p = 0; p < NUM_THREADS; p++) {
    thread5_arg[p].tid = p;
    thread5_arg[p].dataSize = dataSize;
    thread5_arg[p].v1 = v1;
    thread5_arg[p].v2 = v2;
    thread5_arg[p].v3 = v3;
    thread5_arg[p].PostScale = PostScale;
    pthread_create(&thread_id[p], NULL, outer_thread, (void *)&thread5_arg[p]);
  }
  for(int p = 0; p < NUM_THREADS; p++) {
    pthread_join(thread_id[p], NULL);
  }
}


void* z1_thread(void* thread_arg) {
  struct thread_arg1_t *my_thread_arg = (struct thread_arg1_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* v1 = my_thread_arg->v1;
  float* v2 = my_thread_arg->v2;
  float* v3 = my_thread_arg->v3;
  float BoundaryScale = my_thread_arg->BoundaryScale;
  float nu = my_thread_arg->nu;

  int i,j,counter;
  int COUNTER_SIZE = dataSize/NUM_THREADS;
  int upper_bound = (tid == NUM_THREADS-1) ? dataSize : (tid+1)*COUNTER_SIZE;
  for(i = tid*COUNTER_SIZE; i < upper_bound; i++) {
    for(j = 0; j < dataSize; j++) {
      v1[i * dataSize + j] = v1[i * dataSize + j] * BoundaryScale;
      v2[i * dataSize + j] = v2[i * dataSize + j] * BoundaryScale;
      v3[i * dataSize + j] = v3[i * dataSize + j] * BoundaryScale;
    }
  }

  for(counter = 1; counter < dataSize; counter++){
    for(i = tid*COUNTER_SIZE; i < upper_bound; i++) {
      for(j = 0; j < dataSize; j++) {
	v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[(counter - 1) * dataSize * dataSize + i * dataSize + j] * nu;
	v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[(counter - 1) * dataSize * dataSize + i * dataSize + j] * nu;
	v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[(counter - 1) * dataSize * dataSize + i * dataSize + j] * nu;
      }
    }
  }
}

void* z2_thread(void* thread_arg) {
  struct thread_arg1_t *my_thread_arg = (struct thread_arg1_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* v1 = my_thread_arg->v1;
  float* v2 = my_thread_arg->v2;
  float* v3 = my_thread_arg->v3;
  float BoundaryScale = my_thread_arg->BoundaryScale;
  float nu = my_thread_arg->nu;

  int i,j,counter;
  int COUNTER_SIZE = dataSize/NUM_THREADS;
  int upper_bound = (tid == NUM_THREADS-1) ? dataSize : (tid+1)*COUNTER_SIZE;
  for(int i = tid*COUNTER_SIZE; i < upper_bound; i++) {
    for(j = 0; j < dataSize; j++){
      v1[(dataSize - 1) * dataSize * dataSize + i * dataSize + j] = v1[(dataSize - 1) * dataSize * dataSize + i * dataSize + j] * BoundaryScale;
      v2[(dataSize - 1) * dataSize * dataSize + i * dataSize + j] = v2[(dataSize - 1) * dataSize * dataSize + i * dataSize + j] * BoundaryScale;
      v3[(dataSize - 1) * dataSize * dataSize + i * dataSize + j] = v3[(dataSize - 1) * dataSize * dataSize + i * dataSize + j] * BoundaryScale;
    }
  }

  for(counter = dataSize-2; counter >=0; counter--) {
    for(i = tid*COUNTER_SIZE; i < upper_bound; i++) {
      for( j = 0; j < dataSize; j++){
	v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[(counter + 1) * dataSize * dataSize + i * dataSize + j] * nu;
	v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[(counter + 1) * dataSize * dataSize + i * dataSize + j] * nu;
	v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[(counter + 1) * dataSize * dataSize + i * dataSize + j] * nu;
      }
    }
  }
}

void* outer_thread(void* thread_arg) {
  struct thread_arg3_t *my_thread_arg = (struct thread_arg3_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* v1 = my_thread_arg->v1;
  float* v2 = my_thread_arg->v2;
  float* v3 = my_thread_arg->v3;
  float PostScale = my_thread_arg->PostScale;

  int i,j,counter;
  int COUNTER_SIZE = dataSize/NUM_THREADS;
  int upper_bound = (tid == NUM_THREADS-1) ? dataSize : (tid+1)*COUNTER_SIZE;
  for(int counter = tid*COUNTER_SIZE; counter < upper_bound; counter++) {
    for(i = 0; i < dataSize; i++) {
      for( j = 0; j < dataSize; j++) {
	v1[counter * dataSize * dataSize + i * dataSize + j] *= PostScale;
	v2[counter * dataSize * dataSize + i * dataSize + j] *= PostScale;
	v3[counter * dataSize * dataSize + i * dataSize + j] *= PostScale;
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
  float* v2 = my_thread_arg->v2;
  float* v3 = my_thread_arg->v3;
  float BoundaryScale = my_thread_arg->BoundaryScale;
  float nu = my_thread_arg->nu;

  int i,j,k,counter;
  int COUNTER_SIZE = dataSize/NUM_THREADS;
  int upper_bound = (tid == NUM_THREADS-1) ? dataSize : (tid+1)*COUNTER_SIZE;
  for(int counter = tid*COUNTER_SIZE; counter < upper_bound; counter++) {
    for(i = 0; i < dataSize; i++) {
      v1[counter * dataSize * dataSize + dataSize * i] = v1[counter * dataSize * dataSize + dataSize * i]* BoundaryScale;
      v2[counter * dataSize * dataSize + dataSize * i] = v2[counter * dataSize * dataSize + dataSize * i]* BoundaryScale;
      v3[counter * dataSize * dataSize + dataSize * i] = v3[counter * dataSize * dataSize + dataSize * i]* BoundaryScale;
    }
    for(j = 1; j < dataSize; j++) {
      for( i = 0; i < dataSize; i++) {
	v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + i * dataSize + j-1] * nu;
	v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[counter * dataSize * dataSize + i * dataSize + j-1] * nu;
	v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[counter * dataSize * dataSize + i * dataSize + j-1] * nu;
      }
    }

    for(i = 0; i < dataSize; i++) {
      v1[counter * dataSize * dataSize + i * dataSize + dataSize-1] = v1[counter * dataSize * dataSize + i * dataSize + dataSize-1] * BoundaryScale;
      v2[counter * dataSize * dataSize + i * dataSize + dataSize-1] = v2[counter * dataSize * dataSize + i * dataSize + dataSize-1] * BoundaryScale;
      v3[counter * dataSize * dataSize + i * dataSize + dataSize-1] = v3[counter * dataSize * dataSize + i * dataSize + dataSize-1] * BoundaryScale;
    }
    for(j = dataSize-2; j >= 0; j--) {
      for( i = 0; i < dataSize; i++) {
	v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + i * dataSize + j+1] * nu;
	v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[counter * dataSize * dataSize + i * dataSize + j+1] * nu;
	v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[counter * dataSize * dataSize + i * dataSize + j+1] * nu;
      }
    }

    for(j = 0; j < dataSize; j++) {
      v1[counter * dataSize * dataSize + j] = v1[counter * dataSize * dataSize + j] * BoundaryScale;
      v2[counter * dataSize * dataSize + j] = v2[counter * dataSize * dataSize + j] * BoundaryScale;
      v3[counter * dataSize * dataSize + j] = v3[counter * dataSize * dataSize + j] * BoundaryScale;
    }
    for(i = 1; i < dataSize; i++) {
      for( j = 0; j < dataSize; j++) {
	v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + (i - 1) * dataSize + j] * nu;
	v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[counter * dataSize * dataSize + (i - 1) * dataSize + j] * nu;
	v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[counter * dataSize * dataSize + (i - 1) * dataSize + j] * nu;
      }
    }

    for(j = 0; j < dataSize; j++) {
      v1[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] = v1[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] * BoundaryScale;
      v2[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] = v2[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] * BoundaryScale;
      v3[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] = v3[counter * dataSize * dataSize + (dataSize - 1) * dataSize + j] * BoundaryScale;
    }
    for(i = dataSize-2; i >=0; i--) {
      for( j = 0; j < dataSize; j++) {
	v1[counter * dataSize * dataSize + i * dataSize + j] = v1[counter * dataSize * dataSize + i * dataSize + j] + v1[counter * dataSize * dataSize + (i + 1) * dataSize + j] * nu;
	v2[counter * dataSize * dataSize + i * dataSize + j] = v2[counter * dataSize * dataSize + i * dataSize + j] + v2[counter * dataSize * dataSize + (i + 1) * dataSize + j] * nu;
	v3[counter * dataSize * dataSize + i * dataSize + j] = v3[counter * dataSize * dataSize + i * dataSize + j] + v3[counter * dataSize * dataSize + (i + 1) * dataSize + j] * nu;
      } 
    }
  }

}

void U_cal(float* u1, float* u2, float* u3, float* v1, float* v2, float* v3, int dataSize) {
  pthread_t thread_id[NUM_THREADS];
  struct thread_arg2_t thread2_arg[NUM_THREADS];
  for(int p = 0; p < NUM_THREADS; p++) {
    thread2_arg[p].tid = p;
    thread2_arg[p].dataSize = dataSize;
    thread2_arg[p].u1 = u1;
    thread2_arg[p].u2 = u2;
    thread2_arg[p].u3 = u3;
    thread2_arg[p].v1 = v1;
    thread2_arg[p].v2 = v2;
    thread2_arg[p].v3 = v3;
    pthread_create(&thread_id[p], NULL, U_cal_thread, (void *)&thread2_arg[p]);
  }
  for(int p = 0; p < NUM_THREADS; p++) {
    pthread_join(thread_id[p], NULL);
  }
}

void* U_cal_thread(void* thread_arg) {
  struct thread_arg2_t *my_thread_arg = (struct thread_arg2_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float* u1 = my_thread_arg->u1;
  float* u2 = my_thread_arg->u2;
  float* u3 = my_thread_arg->u3;
  float* v1 = my_thread_arg->v1;
  float* v2 = my_thread_arg->v2;
  float* v3 = my_thread_arg->v3;

  int i,j,k;
#define xu1(i,j,k) u1[(i) * dataSize * dataSize + (j) * dataSize + (k)]
#define xu2(i,j,k) u2[(i) * dataSize * dataSize + (j) * dataSize + (k)]
#define xu3(i,j,k) u3[(i) * dataSize * dataSize + (j) * dataSize + (k)]
#define xv1(i,j,k) v1[(i) * dataSize * dataSize + (j) * dataSize + (k)]
#define xv2(i,j,k) v2[(i) * dataSize * dataSize + (j) * dataSize + (k)]
#define xv3(i,j,k) v3[(i) * dataSize * dataSize + (j) * dataSize + (k)]
  int II = dataSize/NUM_THREADS;
  for(int i = tid*II+1; i <= (tid+1)*II; i++) {
    for( j = 1; j < dataSize-1; j++ ) {
      for( k = 1; k < dataSize-1; k++ ) {
	float   du1_dx = (xu1(i+1,j,k) - xu1(i-1,j,k)) *0.5f;
	float   du2_dx = (xu2(i+1,j,k) - xu2(i-1,j,k)) *0.5f;
	float   du3_dx = (xu3(i+1,j,k) - xu3(i-1,j,k)) *0.5f;

	float   du1_dy = (xu1(i,j+1,k) - xu1(i,j-1,k)) *0.5f;
	float   du2_dy = (xu2(i,j+1,k) - xu2(i,j-1,k)) *0.5f;
	float   du3_dy = (xu3(i,j+1,k) - xu3(i,j-1,k)) *0.5f;

	float   du1_dz = (xu1(i,j,k+1) - xu1(i,j,k-1)) *0.5f;
	float   du2_dz = (xu2(i,j,k+1) - xu2(i,j,k-1)) *0.5f;
	float   du3_dz = (xu3(i,j,k+1) - xu3(i,j,k-1)) *0.5f;

	float r1 = xv1(i,j,k) - xv1(i,j,k)*du1_dx - xv2(i,j,k)*du1_dy - xv3(i,j,k)*du1_dz;
	float r2 = xv2(i,j,k) - xv1(i,j,k)*du2_dx - xv2(i,j,k)*du2_dy - xv3(i,j,k)*du2_dz;
	float r3 = xv3(i,j,k) - xv1(i,j,k)*du3_dx - xv2(i,j,k)*du3_dy - xv3(i,j,k)*du3_dz;

	xv1(i,j,k)=r1;
	xv2(i,j,k)=r2;
	xv3(i,j,k)=r3;
      }
    }
  }
}

void SW_Registration::Initialize(int threadID, int procID)
{
        u1 = new float[dataSize * dataSize * dataSize];
        u2 = new float[dataSize * dataSize * dataSize];
        u3 = new float[dataSize * dataSize * dataSize];
        v1 = new float[(dataSize + 1) * (dataSize + 1) * (dataSize + 1)];
        v2 = new float[(dataSize + 1) * (dataSize + 1) * (dataSize + 1)];
        v3 = new float[(dataSize + 1) * (dataSize + 1) * (dataSize + 1)];
	for(int i = 0; i < dataSize * dataSize * dataSize; i++)
	{
		u1[i] = u2[i] = u3[i] = 0.0f;
	}
	for(int i = 0; i < (dataSize + 1) * (dataSize + 1) * (dataSize + 1); i++)
	{
		v1[i] = v2[i] = v3[i] = 0.0f;
	}
}
void SW_Registration::Run()
{
        for(int i = 0; i < ITER_COUNT; i++)
        {
                V_cal(v1, v2, v3, dataSize);
                U_cal(u1, u2, u3, v1, v2, v3, dataSize);
        }
}
void SW_Registration::Shutdown()
{
        while(true);
}


