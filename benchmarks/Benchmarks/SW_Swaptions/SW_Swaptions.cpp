#include "../../BenchmarkNode.h"

#include <stdint.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#define ITER_COUNT 2
#define FTYPE double
#define BLOCK_SIZE 16 // Blocking to allow better caching


class SW_Swaptions : public BenchmarkNode
{
        int dataSize;
	int dim;
        int thread;

	FTYPE **ppdHJMPath;      //Matrix that stores generated HJM path (Output)
        int iN;                  //Number of time-steps
        int iFactors;            //Number of factors in the HJM framework
        FTYPE dYears;            //Number of years
        FTYPE *pdForward;        //t=0 Forward curve
        FTYPE *pdTotalDrift;     //Vector containing total drift corrections for different maturities
        FTYPE ***ppdFactors;      //Factor volatilities
        long lRndSeed;          //Random number seed
        FTYPE **pdZ; //vector to store random normals
        FTYPE **randZ; //vector to store random normals
	FTYPE **factors;

public:
	SW_Swaptions()
	{
		std::cin >> dataSize;
	}
        virtual void Initialize(int threadID, int procID);
        virtual void Run();
        virtual void Shutdown();
	void serialB(FTYPE **pdZ, FTYPE **randZ, int BLOCKSIZE, int iN, int iFactors);
	FTYPE CumNormalInv( FTYPE u );
	FTYPE RanUnif( long *s );		
	FTYPE** dmatrix( long nrl, long nrh, long ncl, long nch );
	void free_dmatrix( FTYPE **m, long nrl, long nrh, long ncl, long nch );
	FTYPE *dvector( long nl, long nh );
	void free_dvector( FTYPE *v, long nl, long nh );
	void nrerror( char error_text[] );
        int  HJM_SimPath_Forward_Blocking(int sw);
};

BENCH_DECL(SW_Swaptions);

void SW_Swaptions::Initialize(int threadID, int procID)
{
        thread = threadID;
	iN = 11;
	dYears = 5.5;
	iFactors = 3; 
	lRndSeed = 100;	
	int nSwaptions = dataSize;
        pdZ   = dmatrix(0, iFactors-1, 0, iN*BLOCK_SIZE -1); //assigning memory
        randZ = dmatrix(0, iFactors-1, 0, iN*BLOCK_SIZE -1); //assigning memory
	ppdHJMPath = dmatrix(0,iN-1,0,iN*BLOCK_SIZE-1);      
        pdForward = dvector(0, iN-1);;        
        pdTotalDrift = dvector(0, iN-2);   

	factors = dmatrix(0, iFactors-1, 0, iN-2); 
	int i, j, k;
	
	ppdFactors = (FTYPE***) malloc(sizeof(FTYPE**)*dataSize);
        for (i = 0; i < nSwaptions; i++) {
          ppdFactors[i] = dmatrix(0, iFactors-1, 0, iN-2);
          for(k=0;k<=iFactors-1;++k)
                 for(j=0;j<=iN-2;++j)
                        ppdFactors[i][k][j] = factors[k][j];
        }
}

void SW_Swaptions::Shutdown()
{
        free_dmatrix(pdZ, 0, iFactors -1, 0, iN*BLOCK_SIZE -1);
        free_dmatrix(randZ, 0, iFactors -1, 0, iN*BLOCK_SIZE -1);

	for (int i = 0; i < dataSize; i++) 
          free_dmatrix(ppdFactors[i], 0, iFactors-1, 0, iN-2);
	free(ppdFactors);

	free_dmatrix(ppdHJMPath, 0,iN-1,0,iN*BLOCK_SIZE-1);
	free_dmatrix(factors, 0, iFactors-1, 0, iN-2);
        free_dvector(pdForward, 0, iN-1);
        free_dvector(pdTotalDrift, 0, iN-2);
        while(true);
}


void SW_Swaptions::Run()
{
   for(int it= 0; it< ITER_COUNT; it++)
   {
	for (int sw=0;sw<dataSize;sw++)
		HJM_SimPath_Forward_Blocking(sw); //ppdHJMPath, iN, iFactors, dYears, pdForward, pdTotalDrift, ppdFactors, lRndSeed, BLOCK_SIZE);
   }
}


void SW_Swaptions::serialB(FTYPE **pdZ, FTYPE **randZ, int BLOCKSIZE, int iN, int iFactors)
{
  for(int l=0;l<=iFactors-1;++l){
    for(int b=0; b<BLOCKSIZE; b++){
      for (int j=1;j<=iN-1;++j){
	pdZ[l][BLOCKSIZE*j + b]= CumNormalInv(randZ[l][BLOCKSIZE*j + b]);  /* 18% of the total executition time */
      }
    }
  }
}


/**********************************************************************/
static FTYPE a[4] = {
  2.50662823884,
    -18.61500062529,
    41.39119773534,
    -25.44106049637
};

static FTYPE b[4] = {
  -8.47351093090,
    23.08336743743,
    -21.06224101826,
    3.13082909833
};

static FTYPE c[9] = {
  0.3374754822726147,
    0.9761690190917186,
    0.1607979714918209,
    0.0276438810333863,
    0.0038405729373609,
    0.0003951896511919,
    0.0000321767881768,
    0.0000002888167364,
    0.0000003960315187
};

/**********************************************************************/
FTYPE SW_Swaptions::CumNormalInv( FTYPE u )
{
  // Returns the inverse of cumulative normal distribution function.
  // Reference: Moro, B., 1995, "The Full Monte," RISK (February), 57-58.

  FTYPE x, r;

  x = u - 0.5;
  if( fabs (x) < 0.42 )
  {
    r = x * x;
    r = x * ((( a[3]*r + a[2]) * r + a[1]) * r + a[0])/
          ((((b[3] * r+ b[2]) * r + b[1]) * r + b[0]) * r + 1.0);
    return (r);
  }

  r = u;
  if( x > 0.0 ) r = 1.0 - u;
  r = log(-log(r));
  r = c[0] + r * (c[1] + r *
       (c[2] + r * (c[3] + r *
       (c[4] + r * (c[5] + r * (c[6] + r * (c[7] + r*c[8])))))));
  if( x < 0.0 ) r = -r;

  return (r);

} // end of CumNormalInv


FTYPE SW_Swaptions::RanUnif( long *s )
{
  // uniform random number generator

  long   ix, k1;
  FTYPE dRes;

  ix = *s;
  k1 = ix/127773L;
  ix = 16807L*( ix - k1*127773L ) - k1 * 2836L;
  if (ix < 0) ix = ix + 2147483647L;
  *s   = ix;
  dRes = (ix * 4.656612875e-10);
  return (dRes);

} // end of RanUnif

int SW_Swaptions::HJM_SimPath_Forward_Blocking(int sw)
/*
				FTYPE **ppdHJMPath,	//Matrix that stores generated HJM path (Output)
				 int iN,					//Number of time-steps
				 int iFactors,			//Number of factors in the HJM framework
				 FTYPE dYears,			//Number of years
				 FTYPE *pdForward,		//t=0 Forward curve
				 FTYPE *pdTotalDrift,	//Vector containing total drift corrections for different maturities
				 FTYPE **ppdFactors,	//Factor volatilities
				 long *lRndSeed,			//Random number seed
				 int BLOCKSIZE)*/
{
//This function computes and stores an HJM Path for given inputs

	int iSuccess = 0;
	int i,j,l; //looping variables
	int BLOCKSIZE = BLOCK_SIZE;
//	FTYPE **pdZ; //vector to store random normals
//	FTYPE **randZ; //vector to store random normals
	FTYPE dTotalShock; //total shock by which the forward curve is hit at (t, T-t)
	FTYPE ddelt, sqrt_ddelt; //length of time steps

	ddelt = (FTYPE)(dYears/iN);
	sqrt_ddelt = sqrt(ddelt);

//	pdZ   = dmatrix(0, iFactors-1, 0, iN*BLOCKSIZE -1); //assigning memory
//	randZ = dmatrix(0, iFactors-1, 0, iN*BLOCKSIZE -1); //assigning memory

	// =====================================================
	// t=0 forward curve stored iN first row of ppdHJMPath
	// At time step 0: insert expected drift
	// rest reset to 0
	for(int b=0; b<BLOCKSIZE; b++){
	  for(j=0;j<=iN-1;j++){
	    ppdHJMPath[0][BLOCKSIZE*j + b] = pdForward[j];

	    for(i=1;i<=iN-1;++i)
	      { ppdHJMPath[i][BLOCKSIZE*j + b]=0; } //initializing HJMPath to zero
	  }
	}
	// -----------------------------------------------------

        // =====================================================
        // sequentially generating random numbers

        for(int b=0; b<BLOCKSIZE; b++){
          for(int s=0; s<1; s++){
            for (j=1;j<=iN-1;++j){
              for (l=0;l<=iFactors-1;++l){
                //compute random number in exact same sequence
                randZ[l][BLOCKSIZE*j + b + s] = RanUnif(&lRndSeed);  /* 10% of the total executition time */
              }
            }
          }
        }

	// =====================================================
	// shocks to hit various factors for forward curve at t

	/* 18% of the total executition time */
	serialB(pdZ, randZ, BLOCKSIZE, iN, iFactors);

	// =====================================================
	// Generation of HJM Path1
	for(int b=0; b<BLOCKSIZE; b++){ // b is the blocks
	  for (j=1;j<=iN-1;++j) {// j is the timestep

	    for (l=0;l<=iN-(j+1);++l){ // l is the future steps
	      dTotalShock = 0;

	      for (i=0;i<=iFactors-1;++i){// i steps through the stochastic factors
		dTotalShock += ppdFactors[sw][i][l]* pdZ[i][BLOCKSIZE*j + b];
	      }

	      ppdHJMPath[j][BLOCKSIZE*l+b] = ppdHJMPath[j-1][BLOCKSIZE*(l+1)+b]+ pdTotalDrift[l]*ddelt + sqrt_ddelt*dTotalShock;
	      //as per formula
	    }
	  }
	} // end Blocks
	// -----------------------------------------------------

	//free_dmatrix(pdZ, 0, iFactors -1, 0, iN*BLOCKSIZE -1);
	//free_dmatrix(randZ, 0, iFactors -1, 0, iN*BLOCKSIZE -1);
	iSuccess = 1;
	return iSuccess;
}

FTYPE** SW_Swaptions::dmatrix( long nrl, long nrh, long ncl, long nch )
{
  // allocate a FTYPE matrix with subscript range m[nrl..nrh][ncl..nch]

        long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
        FTYPE **m;

  // allocate pointers to rows
        m=(FTYPE **) malloc((size_t)((nrow+1)*sizeof(FTYPE*)));
	Touch(thread, (void *)m, (size_t)((nrow+1)*sizeof(FTYPE*)));	
        if (!m) nrerror("allocation failure 1 in dmatrix()");
        m += 1;
        m -= nrl;

  // allocate rows and set pointers to them
        m[nrl]=(FTYPE *) malloc((size_t)((nrow*ncol+1)*sizeof(FTYPE)));
	Touch(thread, (void *)m[nrl], (size_t)((nrow*ncol+1)*sizeof(FTYPE)));
        if (!m[nrl]) nrerror("allocation failure 2 in dmatrix()");
        m[nrl] += 1;
        m[nrl] -= ncl;

        for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  // return pointer to array of pointers to rows
        return m;

} // end of dmatrix 

void SW_Swaptions::free_dmatrix( FTYPE **m, long nrl, long nrh, long ncl, long nch )
{
  // free a FTYPE matrix allocated by dmatrix()

        free((char*) (m[nrl]+ncl-1));
        free((char*) (m+nrl-1));

} // end of free_dmatrix

void SW_Swaptions::nrerror( char error_text[] )
{
  // Numerical Recipes standard error handler
        fprintf( stderr,"Numerical Recipes run-time error...\n" );
        fprintf( stderr,"%s\n",error_text );
        fprintf( stderr,"...now exiting to system...\n" );
        exit(1);

} // end of nrerror

FTYPE *SW_Swaptions::dvector( long nl, long nh )
{
  // allocate a FTYPE vector with subscript range v[nl..nh] 

        FTYPE *v;

        v=(FTYPE *)malloc((size_t) ((nh-nl+2)*sizeof(FTYPE)));
	Touch(thread, v, (size_t) ((nh-nl+2)*sizeof(FTYPE)));
        if (!v) nrerror("allocation failure in dvector()");
        return v-nl+1;

} // end of dvector

/**********************************************************************/
void SW_Swaptions::free_dvector( FTYPE *v, long nl, long nh )
{
  // free a FTYPE vector allocated with dvector()

        free((char*) (v+nl-1));

} // end of free_dvector

