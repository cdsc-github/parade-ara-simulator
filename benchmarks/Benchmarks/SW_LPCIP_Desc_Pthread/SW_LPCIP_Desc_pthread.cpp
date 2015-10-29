#include "../../BenchmarkNode.h"
#include <stdint.h>
#include <iostream>
#include <cstring>

#define IMGW                    640
#define IMGH                    480
#define PATCH_H                 10
#define PATCH_W                 10


#define NUM_THREADS 4

void* LPCIP_Desc_thread(void* thread_arg);


class SW_LPCIP_Desc : public BenchmarkNode
{
	int thread;
	int dataSize;

	unsigned char (*input_img);
	float (*input_center_x);
	float (*input_center_y);
	float (*input_rho_scale);
	float (*output_lpcip)[PATCH_H * PATCH_W];
public:
	void computeLPCIP(unsigned char input_img[IMGW * IMGH], float input_center_x, float input_center_y, float input_rho_scale, float output_lpcip[PATCH_H * PATCH_W]);

//public:
	SW_LPCIP_Desc()
	{
		std::cin >> dataSize;
	}
	virtual void Initialize(int threadID, int procID);
	virtual void Run();
	virtual void Shutdown();
};

BENCH_DECL(SW_LPCIP_Desc);

struct thread_arg1_t {

int tid;
  int dataSize;

 unsigned char (*input_img);
        float (*input_center_x);
        float (*input_center_y);
        float (*input_rho_scale);
        float (*output_lpcip)[PATCH_H * PATCH_W];

SW_LPCIP_Desc* benchmark;
};




void SW_LPCIP_Desc::computeLPCIP(unsigned char input_img[IMGW * IMGH], float input_center_x, float input_center_y, float input_rho_scale, float output_lpcip[PATCH_H * PATCH_W])
{
	// *** Log-Polar Transform (Forward) ***
	// (NOTE: This function is called for each feature)

	#define LPCIP_img(r,c)          input_img[r*640+c]
	#define LPCIP_center_x          (input_center_x)
	#define LPCIP_center_y          (input_center_y)
	#define LPCIP_rho_scale         (input_rho_scale)
	#define LPCIP_lpcip(r,c)        output_lpcip[r*10+c]

	#ifndef CV_PI
	#define CV_PI           3.1415926535897932384626433832795f
	#endif

	#define LPCIP_IMGW                    IMGW
	#define LPCIP_IMGH                    IMGH
	#define LPCIP_PATCH_H                 PATCH_H
	#define LPCIP_PATCH_W                 PATCH_W

	#ifndef opt_sin_f_f     // Functionality: sin x = x - (x^3)/6 + (x^5)/120 - (x^7)/5040
	#define opt_sin_f_f(FLOAT_IN, FLOAT_OUT) { \
		float x2, x3, x5, x7; \
		x2 = FLOAT_IN * FLOAT_IN; \
		x3 = x2*FLOAT_IN; \
		x5 = x3*x2; \
		x7 = x5*x2; \
		FLOAT_OUT = (FLOAT_IN - x3/6 + x5/120 - x7/5040); \
	}
	#endif

	#ifndef opt_cos_f_f     // Functionality: --> cos x = 1 - (x^2)/2 + (x^4)/24  - (x^6)/720
	#define opt_cos_f_f(FLOAT_IN, FLOAT_OUT) { \
		float x2, x4, x6; \
		x2 = FLOAT_IN * FLOAT_IN; \
		x4 = x2*x2; \
		x6 = x4*x2; \
		FLOAT_OUT = (1 - x2/2 + x4/24  - x6/720); \
	}
	#endif
		
	#ifndef opt_exp_f_f
	#define opt_exp_f_f(FLOAT_IN, FLOAT_OUT) { \
		if (FLOAT_IN <= -4.f) \
				FLOAT_OUT = 0.f; \
		else { \
				float x1  = FLOAT_IN + 2.f; \
				float x2 = x1*x1; \
				float x3 = x2*x1; \
				float x4 = x2*x2; \
				float x5 = x2*x3; \
				FLOAT_OUT = 0.1353352832366127f * (1.f + x1 + x2/2.f + x3/6.f + x4/24.f + x5/120.f); \
		} \
	}
	#endif

	int mapx[LPCIP_PATCH_H][LPCIP_PATCH_W];
	int mapy[LPCIP_PATCH_H][LPCIP_PATCH_W];

	int phi, rho, h, w;
	float _exp_tab[LPCIP_PATCH_W];
	float* exp_tab = _exp_tab;

	for (rho = 0; rho < LPCIP_PATCH_W; rho++) {
	// loop_tripcount min=10 max=10 avg=10
		float exp_in = rho / LPCIP_rho_scale;
		float exp_out;
		opt_exp_f_f(exp_in, exp_out);
		exp_tab[rho] = exp_out;
	}

	for (phi = 0; phi < LPCIP_PATCH_H; phi++) {
	// loop_tripcount min=10 max=10 avg=10
		float trig_in, cp, sp;
		trig_in = (float) (phi*2*CV_PI/LPCIP_PATCH_H);
		opt_cos_f_f(trig_in, cp);
		opt_sin_f_f(trig_in, sp);

		for (rho = 0; rho < LPCIP_PATCH_W; rho++) {
		// loop_tripcount min=10 max=10 avg=10
			float r = exp_tab[rho];
			float x = r*cp + LPCIP_center_x;
			float y = r*sp + LPCIP_center_y;
			mapx[phi][rho] = (int) x;
			mapy[phi][rho] = (int) y;
		}
	}

	for (h = 0; h < LPCIP_PATCH_H; h++) {
	// loop_tripcount min=10 max=10 avg=10
		for (w = 0; w < LPCIP_PATCH_W; w++) {
		// loop_tripcount min=10 max=10 avg=10
			int y_mapping = mapy[h][w];
			if (y_mapping < 0) y_mapping = 0;
			else if (y_mapping >= LPCIP_IMGH) y_mapping = LPCIP_IMGH-1;

			int x_mapping = mapx[h][w];
			if (x_mapping < 0) x_mapping = 0;
			else if (x_mapping >= LPCIP_IMGW) x_mapping = LPCIP_IMGW-1;

			LPCIP_lpcip(h,w) = LPCIP_img(y_mapping, x_mapping);
		}
	}
}
void SW_LPCIP_Desc::Initialize(int threadID, int procID)
{
	thread = threadID;

	input_img = new unsigned char[IMGH * IMGW];
	input_center_x = new float[dataSize];
	input_center_y = new float[dataSize];
	input_rho_scale = new float[dataSize];
	output_lpcip = new float[dataSize][PATCH_H * PATCH_W];

	for (int i = 0; i < dataSize; i++) {
		input_center_x[i] = 100;
		input_center_y[i] = 100;
		input_rho_scale[i] = 1;
		for (int r = 0; r < PATCH_H; r++) {
			for (int c = 0; c < PATCH_W; c++) {
				output_lpcip[i][r * PATCH_W + c] = 0;
			}
		}
	}
	for (int h = 0; h < IMGH; h++)
		for (int w = 0; w < IMGW; w++)
			input_img[h * IMGW + w] = 0;
}
void SW_LPCIP_Desc::Run()
{
  pthread_t thread_id[NUM_THREADS];
  struct thread_arg1_t thread1_arg[NUM_THREADS];

  for(int p = 0; p < NUM_THREADS; p++) {
    thread1_arg[p].tid = p;
    thread1_arg[p].dataSize = dataSize;
    thread1_arg[p].input_img = input_img;
    thread1_arg[p].input_center_x = input_center_x;
    thread1_arg[p].input_center_y = input_center_y;
    thread1_arg[p].input_rho_scale = input_rho_scale;
    thread1_arg[p].output_lpcip = output_lpcip;
    thread1_arg[p].benchmark = this;
    pthread_create(&thread_id[p], NULL, LPCIP_Desc_thread, (void *)&thread1_arg[p]);
  }
  for(int p = 0; p < NUM_THREADS; p++) {
    pthread_join(thread_id[p], NULL);
  }

}

void* LPCIP_Desc_thread(void* thread_arg) {
 
   struct thread_arg1_t *my_thread_arg = (struct thread_arg1_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  unsigned char (*input_img) = my_thread_arg->input_img;
  float (*input_center_x) = my_thread_arg->input_center_x;
  float (*input_center_y) = my_thread_arg->input_center_y;
   float (*input_rho_scale) = my_thread_arg->input_rho_scale;
   float (*output_lpcip)[PATCH_H * PATCH_W] = my_thread_arg->output_lpcip;
  SW_LPCIP_Desc* benchmark = my_thread_arg->benchmark;

  int PP = dataSize/NUM_THREADS;
  for(int i = tid*PP; i < (tid+1)*PP; i++) {

		benchmark->computeLPCIP(input_img, input_center_x[i], input_center_y[i], input_rho_scale[i], output_lpcip[i]);
	}
}
void SW_LPCIP_Desc::Shutdown()
{
	delete [] input_img;
	delete [] input_center_x;
	delete [] input_center_y;
	delete [] input_rho_scale;
	delete [] output_lpcip;

	while(true);
}
