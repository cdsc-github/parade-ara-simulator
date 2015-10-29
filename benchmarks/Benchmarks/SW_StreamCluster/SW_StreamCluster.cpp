#include "../../BenchmarkNode.h"

#include <stdint.h>
#include <iostream>
#include <cmath>

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


class SW_StreamCluster : public BenchmarkNode
{
        int dataSize;
	int dim;
        int thread;

	bool* is_center; //whether a point is a center
	int* center_table; //index table of centers
	int* work_mem;

        Points points;
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
	float  dist(Point p1, Point p2, int dim);
	double pgain(long x, Points *points, double z, long int *numcenters, int pid);
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
	work_mem = new int[dataSize];
	
	points.dim = dim;
	points.num = dataSize;
	points.p = (Point *)malloc(dataSize*sizeof(Point));
	for( int i = 0; i < dataSize; i++ ) {
		points.p[i].coord = (float*)malloc( dim*sizeof(float) );
	}
	switch_membership = new int[dataSize];
	lower = new float[dataSize];
        low = new float[dataSize];
        gl_lower = new float[dataSize];
	
	memset(center_table, 0, dataSize*sizeof(int));
	memset(work_mem, 0, dataSize*sizeof(int));
	memset(switch_membership, 0, dataSize*sizeof(int));
	memset(lower, 0, dataSize*sizeof(float));
	memset(low, 0, dataSize*sizeof(float));
	memset(gl_lower, 0, dataSize*sizeof(float));

	for (int i=0;i<dataSize;i++)
	{
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


void SW_StreamCluster::Run()
{
   int k1 = 0;
   int k2 = dataSize;
   for(int it= 0; it< ITER_COUNT; it++)
   {
	int count = 0;
	for( int i = k1; i < k2; i++ ) {
	      if( is_center[i] ) {
		      center_table[i] = count++;
	      }
	}

//this section is omitted because it is only required to normalize the partial counting done in the previous step for parallel code.  Since this code is sequential, this step is unnecessary.  To simplify things, I removed this step in the LCA and CFU versions as well.
/*	for( int i = k1; i < k2; i++ ) {
	      if( is_center[i] ) {
		      center_table[i] += (int)work_mem[thread];
	      }
	}*/

	for (int i = k1; i < k2; i++ ) {
	      float x_cost = dist(points.p[i], points.p[x], points.dim) * points.p[i].weight;
	      float current_cost = points.p[i].cost;
	      if ( x_cost < current_cost ) {
		      switch_membership[i] = 1;
		      cost_of_opening_x += x_cost - current_cost;
	      } else {
		      int assign = points.p[i].assign;
		      lower[center_table[assign]] += current_cost - x_cost;
	      }
	}

	for ( int i = k1; i < k2; i++ ) {
	  if( is_center[i] ) {
	    gl_lower[center_table[i]] = low[i];
	    if ( low[i] > 0 ) {
	      ++number_of_centers_to_close;
	    }
	  }
	}

	for ( int i = k1; i < k2; i++ ) {
	  bool close_center = gl_lower[center_table[points.p[i].assign]] > 0 ;
	  if ( switch_membership[i] || close_center ) {
	    points.p[i].cost = points.p[i].weight * dist(points.p[i], points.p[x], points.dim);
	    points.p[i].assign = x;
	  }
	}

	for( int i = k1; i < k2; i++ ) {
	  if( is_center[i] && gl_lower[center_table[i]] > 0 ) {
	    is_center[i] = false;
	  }
	}
   }
}


/* compute Euclidean distance squared between two points */
float SW_StreamCluster::dist(Point p1, Point p2, int dim)
{
  int i;
  float result=0.0;
  for (i=0;i<dim;i++)
    result += (p1.coord[i] - p2.coord[i])*(p1.coord[i] - p2.coord[i]);
  return(result);
}   

