#include "../../BenchmarkNode.h"

#include <stdint.h>
#include <iostream>
#include <cmath>
#include <pthread.h>

#define NUM_THREADS 4

#define ITER_COUNT 2
#define CACHE_LINE 32 // cache line in byte

/* this structure represents a point */
/* these will be passed around to avoid copying coordinates */
typedef struct {
  float weight;
  float *coord;
  long assign;  /* number of point where this one is assigned */
  float cost;  /* cost of that assignment, weight*distance */
} Point;

/* this is the array of points */
typedef struct {
  long num; /* number of points; may not be N if this is a sample */
  int dim;  /* dimensionality */
  Point *p; /* the array itself */
} Points;

struct thread_arg1_t {
  int tid;
  int dataSize;
  Points* points;
  int* center_table;
  int *switch_membership;
  float *lower;
  float* cost_of_opening_x_local;
  int x;
};

struct thread_arg2_t {
  int tid;
  int dataSize;
  bool *is_center;
  int* center_table;
  float *low;
  float *gl_lower;
  int* number_of_centers_to_close_local;
};

struct thread_arg3_t {
  int tid;
  int dataSize;
  Points* points;
  bool *is_center;
  int* center_table;
  float *gl_lower;
  int *switch_membership;
  int x;
};

float  dist(Point p1, Point p2, int dim);
void* cost_thread(void* thread_arg);
void* gl_lower_thread(void* thread_arg);
void* close_thread(void* thread_arg);
pthread_mutex_t mutexassign;

class SW_StreamCluster : public BenchmarkNode
{
        int dataSize;
	int dim;
        int thread;

	bool* is_center; //whether a point is a center
	int* center_table; //index table of centers
  //int* work_mem;

        Points* points;
        int *switch_membership;
        float *lower;
        float cost_of_opening_x;

	float *low;
	float *gl_lower;
	int number_of_centers_to_close;

	int x;

public:
	SW_StreamCluster()
	{
		std::cin >> dataSize;
	}
        virtual void Initialize(int threadID, int procID);
        virtual void Run();
        virtual void Shutdown();
  //double pgain(long x, Points *points, double z, long int *numcenters, int pid);
};

BENCH_DECL(SW_StreamCluster);

void SW_StreamCluster::Initialize(int threadID, int procID)
{
        thread = threadID;
	uint8_t* constCluster;
	//dataSize=1024;
	dim = 32;

	is_center = new bool[dataSize];
	center_table = new int[dataSize];
	//work_mem = new int[dataSize];
	
	points = (Points *)malloc(sizeof(Points));
	points->dim = dim;
	points->num = dataSize;
	points->p = (Point *)malloc(dataSize*sizeof(Point));
	for( int i = 0; i < dataSize; i++ ) {
		points->p[i].coord = (float*)malloc( dim*sizeof(float) );
	}
	switch_membership = new int[dataSize];
	lower = new float[dataSize];
        low = new float[dataSize];
        gl_lower = new float[dataSize];
	
	//memset(center_table, 0, dataSize*sizeof(int));
	//memset(work_mem, 0, dataSize*sizeof(int));
	//memset(switch_membership, 0, dataSize*sizeof(int));
	//memset(lower, 0, dataSize*sizeof(float));
	//memset(low, 0, dataSize*sizeof(float));
	//memset(gl_lower, 0, dataSize*sizeof(float));

	for (int i=0;i<dataSize;i++)
	{
	  center_table[i] = 0;
	  switch_membership[i] = 0;
	  lower[i] = 0;
	  low[i] = 0;
	  gl_lower[i] = 0;
		if (i%2)
			is_center[i] = false;	
		else
			is_center[i]= true;
	}
}

void SW_StreamCluster::Shutdown()
{
        while(true);
}

void* cost_thread(void* thread_arg) {
  struct thread_arg1_t *my_thread_arg = (struct thread_arg1_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  Points points = *(my_thread_arg->points);
  int* center_table = my_thread_arg->center_table; //index table of centers
  int *switch_membership = my_thread_arg->switch_membership;
  float *lower = my_thread_arg->lower;
  float* cost_of_opening_x_local = my_thread_arg->cost_of_opening_x_local;
  int x = my_thread_arg->x;

  int PP = dataSize/NUM_THREADS;
  for(int i = tid*PP; i < (tid+1)*PP; i++) {
    float x_cost = dist(points.p[i], points.p[x], points.dim) * points.p[i].weight;
    float current_cost = points.p[i].cost;
    if ( x_cost < current_cost ) {
      switch_membership[i] = 1;
      cost_of_opening_x_local[tid] += x_cost - current_cost;
    } else {
      int assign = points.p[i].assign;
      int center_assign = center_table[assign];
      float cost_diff = current_cost - x_cost;
      //lock
      pthread_mutex_lock(&mutexassign);
      lower[center_assign] += cost_diff;
      pthread_mutex_unlock(&mutexassign);
    }
  }
}

void* gl_lower_thread(void* thread_arg) {
  struct thread_arg2_t *my_thread_arg = (struct thread_arg2_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  bool *is_center = my_thread_arg->is_center;
  int* center_table = my_thread_arg->center_table; //index table of centers
  float *low = my_thread_arg->low;
  float *gl_lower = my_thread_arg->gl_lower;
  int* number_of_centers_to_close_local = my_thread_arg->number_of_centers_to_close_local;

  int PP = dataSize/NUM_THREADS;
  for(int i = tid*PP; i < (tid+1)*PP; i++) {
    if( is_center[i] ) {
      gl_lower[center_table[i]] = low[i];
      if ( low[i] > 0 ) {
	++number_of_centers_to_close_local[tid];
      }
    }
  }
}

void* close_thread(void* thread_arg) {
  struct thread_arg3_t *my_thread_arg = (struct thread_arg3_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  Points points = *(my_thread_arg->points);
  bool *is_center = my_thread_arg->is_center;
  int* center_table = my_thread_arg->center_table; //index table of centers
  float *gl_lower = my_thread_arg->gl_lower;
  int *switch_membership = my_thread_arg->switch_membership;
  int x = my_thread_arg->x;

  int PP = dataSize/NUM_THREADS;
  for(int i = tid*PP; i < (tid+1)*PP; i++) {
    bool close_center = gl_lower[center_table[points.p[i].assign]] > 0 ;
    if ( switch_membership[i] || close_center ) {
      points.p[i].cost = points.p[i].weight * dist(points.p[i], points.p[x], points.dim);
      points.p[i].assign = x;
    }
    if( is_center[i] && gl_lower[center_table[i]] > 0 ) {
      is_center[i] = false;
    }
  }
}

void SW_StreamCluster::Run()
{
   int k1 = 0;
   int k2 = dataSize;
   for(int it= 0; it< ITER_COUNT; it++) {
       //sequential, note that center_table[i] increases with i,
       //no problem for parallelization of reading x[center_table[i]]
       int count = 0;
       for( int i = k1; i < k2; i++ ) {
	 if( is_center[i] ) {
	   center_table[i] = count++;
	 }
       }

       //parallel
       pthread_t thread_id[NUM_THREADS];
       struct thread_arg1_t thread1_arg[NUM_THREADS];
       float cost_of_opening_x_local[NUM_THREADS];
       pthread_mutex_init(&mutexassign, NULL);
       for(int p = 0; p < NUM_THREADS; p++) {
	 thread1_arg[p].tid = p;
	 thread1_arg[p].dataSize = dataSize;
	 thread1_arg[p].points = points;
	 thread1_arg[p].center_table = center_table;
	 thread1_arg[p].switch_membership = switch_membership;
	 thread1_arg[p].lower = lower;
	 thread1_arg[p].cost_of_opening_x_local = cost_of_opening_x_local;
	 thread1_arg[p].x = x;
	 pthread_create(&thread_id[p], NULL, cost_thread, (void *)&thread1_arg[p]);
       }
       for(int p = 0; p < NUM_THREADS; p++) {
	 pthread_join(thread_id[p], NULL);
       }
       pthread_mutex_destroy(&mutexassign);

       //reduction
       for(int i = 0; i < NUM_THREADS; i++) {
	 cost_of_opening_x += cost_of_opening_x_local[i];
       }

       //parallel
       struct thread_arg2_t thread2_arg[NUM_THREADS];
       int number_of_centers_to_close_local[NUM_THREADS];
       for(int p = 0; p < NUM_THREADS; p++) {
	 thread2_arg[p].tid = p;
	 thread2_arg[p].dataSize = dataSize;
	 thread2_arg[p].is_center = is_center;
	 thread2_arg[p].center_table = center_table;
	 thread2_arg[p].low = low;
	 thread2_arg[p].gl_lower = gl_lower;
	 thread2_arg[p].number_of_centers_to_close_local = number_of_centers_to_close_local;
	 pthread_create(&thread_id[p], NULL, gl_lower_thread, (void *)&thread2_arg[p]);
       }
       for(int p = 0; p < NUM_THREADS; p++) {
	 pthread_join(thread_id[p], NULL);
       }

       //reduction
       for(int i = 0; i < NUM_THREADS; i++) {
	 number_of_centers_to_close += number_of_centers_to_close_local[i];
       }

       //parallel
       struct thread_arg3_t thread3_arg[NUM_THREADS];
       for(int p = 0; p < NUM_THREADS; p++) {
	 thread3_arg[p].tid = p;
	 thread3_arg[p].dataSize = dataSize;
	 thread3_arg[p].points = points;
	 thread3_arg[p].is_center = is_center;
	 thread3_arg[p].center_table = center_table;
	 thread3_arg[p].gl_lower = gl_lower;
	 thread3_arg[p].switch_membership = switch_membership;
	 thread3_arg[p].x = x;
	 pthread_create(&thread_id[p], NULL, close_thread, (void *)&thread3_arg[p]);
       }
       for(int p = 0; p < NUM_THREADS; p++) {
	 pthread_join(thread_id[p], NULL);
       }
     }
}

/* compute Euclidean distance squared between two points */
float dist(Point p1, Point p2, int dim)
{
  int i;
  float result=0.0;
  for (i=0;i<dim;i++)
    result += (p1.coord[i] - p2.coord[i])*(p1.coord[i] - p2.coord[i]);
  return(result);
}   

