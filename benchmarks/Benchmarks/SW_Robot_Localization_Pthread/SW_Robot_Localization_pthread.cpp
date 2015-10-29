#include "../../BenchmarkNode.h"
#include <stdint.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

#define N_VAL               3


#define NUM_THREADS 4

void* Robot_Localization_thread(void* thread_arg);



class SW_Robot_Localization : public BenchmarkNode
{
	int dataSize;
	int thread;
	
	float (*ran1)[N_VAL][3];		// input
	float (*ran2)[N_VAL][3];		// input
	float (*quat)[N_VAL][4];		// input
	float (*accl)[N_VAL][3];		// input
	float (*pos_in)[N_VAL][3];		// input and output
	float (*vel_in)[N_VAL][3];		// input and output
	float (*pos_out)[N_VAL][3];		// input and output
	float (*vel_out)[N_VAL][3];		// input and output

public:	
	void RobLoc(float in_ran1[3], float in_ran2[3],
				float in_quat[4], float in_accl[3],
				float in_pos[3], float in_vel[3],
				float out_pos[3], float out_vel[3]);

//public:
	SW_Robot_Localization()
	{
		std::cin >> dataSize;
	}
	virtual void Initialize(int threadID, int procID);
	virtual void Run();
	virtual void Shutdown();
};


BENCH_DECL(SW_Robot_Localization);


struct thread_arg1_t {
  int tid;
  int dataSize;
 
 float (*ran1)[N_VAL][3];                // input
        float (*ran2)[N_VAL][3];                // input
        float (*quat)[N_VAL][4];                // input
        float (*accl)[N_VAL][3];                // input
        float (*pos_in)[N_VAL][3];              // input and output
        float (*vel_in)[N_VAL][3];              // input and output
        float (*pos_out)[N_VAL][3];             // input and output
        float (*vel_out)[N_VAL][3];             // input and output


 SW_Robot_Localization* benchmark;
};





void SW_Robot_Localization::RobLoc(float in_ran1[3], float in_ran2[3],
								   float in_quat[4], float in_accl[3],
								   float in_pos[3], float in_vel[3],
								   float out_pos[3], float out_vel[3])
{
	// Observation

	float q0 = in_quat[0];
	float q1 = in_quat[1];
	float q2 = in_quat[2];
	float q3 = in_quat[3];

	float g0 = q2 * 9.8;
	float g1 = q3 * 9.8;
	float g2 = q0 * (-9.8);
	float g3 = q1 * (-9.8);

	float gravity0 = g1*q0 - g0*q1 - g2*q3 + g3*q2;
	float gravity1 = g1*q3 - g0*q2 + g2*q0 - g3*q1;
	float gravity2 = g2*q1 - g0*q3 - g1*q2 + g3*q0;

	// Motion Model

	float accl0 = in_accl[0] - gravity0;
	float accl1 = in_accl[1] - gravity1;
	float accl2 = in_accl[2] - gravity2;

	float a0 = q0*in_vel[0] + q1*in_vel[1] + q2*in_vel[2];
	float a1 = q0*in_vel[1] - q1*in_vel[0] + q3*in_vel[2];
	float a2 = q0*in_vel[2] - q2*in_vel[0] - q3*in_vel[1];
	float a3 = q2*in_vel[1] - q1*in_vel[2] - q3*in_vel[0];

	float c0 = q0*accl0 + q1*accl1 + q2*accl2;
	float c1 = q0*accl1 - q1*accl0 + q3*accl2;
	float c2 = q0*accl2 - q2*accl0 - q3*accl1;
	float c3 = q2*accl1 - q1*accl2 - q3*accl0;

	out_pos[0] = in_pos[0] + 0.01 * (a0*q1 + a1*q0 + a2*q3 - a3*q2) + in_ran1[0] + 0.00005 * (c0*q1 + c1*q0 + c2*q3 - c3*q2);
	out_pos[1] = in_pos[1] + 0.01 * (a0*q2 - a1*q3 + a2*q0 + a3*q1) + in_ran1[1] + 0.00005 * (c0*q2 - c1*q3 + c2*q0 + c3*q1);
	out_pos[2] = in_pos[2] + 0.01 * (a0*q3 + a1*q2 - a2*q1 + a3*q0) + in_ran1[2] + 0.00005 * (c0*q3 + c1*q2 - c2*q1 + c3*q0);

	out_vel[0] = in_vel[0] + accl0 * 0.01 + in_ran2[0];
	out_vel[1] = in_vel[1] + accl1 * 0.01 + in_ran2[1];
	out_vel[2] = in_vel[2] + accl2 * 0.01 + in_ran2[2];
}
void SW_Robot_Localization::Initialize(int threadID, int procID)
{
	thread = threadID;
	
	ran1 = new float[dataSize][N_VAL][3];
	ran2 = new float[dataSize][N_VAL][3];
	quat = new float[dataSize][N_VAL][4];
	accl = new float[dataSize][N_VAL][3];
	pos_in = new float[dataSize][N_VAL][3];
	vel_in = new float[dataSize][N_VAL][3];
	pos_out = new float[dataSize][N_VAL][3];
	vel_out = new float[dataSize][N_VAL][3];
	
	for (int z = 0; z < 1; z++) {
		ran1[z][0][0] = (-2.0 * log(0.9)) / 9;
		ran1[z][0][1] = (-2.0 * log(0.45)) / 4.5;
		ran1[z][0][2] = (-2.0 * log(0.3)) / 3;
		ran1[z][1][0] = (-2.0 * log(0.45)) / 4.5;
		ran1[z][1][1] = (-2.0 * log(0.9)) / 9;
		ran1[z][1][2] = (-2.0 * log(0.6)) / 6;
		ran1[z][2][0] = (-2.0 * log(0.3)) / 3;
		ran1[z][2][1] = (-2.0 * log(0.6)) / 6;
		ran1[z][2][2] = (-2.0 * log(0.9)) / 9;
		for (int i = 3; i < N_VAL; i++) {
			float x;
			x = 0.09 / (i+1);
			ran1[z][i][0] = (-2.0 * log(x)) / x;
			x = 0.18 / (i+1);
			ran1[z][i][1] = (-2.0 * log(x)) / x;
			x = 0.27 / (i+1);
			ran1[z][i][2] = (-2.0 * log(x)) / x;
		}
		
		ran2[z][0][0] = (-4.0 * log(0.9)) / 9;
		ran2[z][0][1] = (-4.0 * log(0.45)) / 4.5;
		ran2[z][0][2] = (-4.0 * log(0.3)) / 3;
		ran2[z][1][0] = (-4.0 * log(0.45)) / 4.5;
		ran2[z][1][1] = (-4.0 * log(0.9)) / 9;
		ran2[z][1][2] = (-4.0 * log(0.6)) / 6;
		ran2[z][2][0] = (-4.0 * log(0.3)) / 3;
		ran2[z][2][1] = (-4.0 * log(0.6)) / 6;
		ran2[z][2][2] = (-4.0 * log(0.9)) / 9;
		for (int i = 3; i < N_VAL; i++) {
			float x;
			x = 0.09 / (i+1);
			ran2[z][i][0] = (-4.0 * log(x)) / x;
			x = 0.18 / (i+1);
			ran2[z][i][1] = (-4.0 * log(x)) / x;
			x = 0.27 / (i+1);
			ran2[z][i][2] = (-4.0 * log(x)) / x;
		}
		
		for (int i = 0; i < N_VAL; i++) {
			for (int j = 0; j < 4; j++) {
				quat[z][i][j] = 0.0;
			}
			for (int j = 0; j < 3; j++) {
				accl[z][i][j] = 0.1;
				pos_in[z][i][j] = vel_in[z][i][j] = 0.0;
		   		pos_out[z][i][j] = vel_out[z][i][j] = 0.0;
			}
		}
	}
	for (int z = 1; z < dataSize; z++) {
	  for(int i = 0; i < N_VAL; i++) {
	    for(int j = 0; j < 3; j++) {
	      ran1[z][i][j] = ran1[0][i][j];
	      ran2[z][i][j] = ran2[0][i][j];
	      accl[z][i][j] = accl[0][i][j];
	      pos_in[z][i][j] = pos_in[0][i][j];
	      vel_in[z][i][j] = vel_in[0][i][j];
	      pos_out[z][i][j] = pos_out[0][i][j];
	      vel_out[z][i][j] = vel_out[0][i][j];
	    }
	    for(int j = 0; j < 4; j++) {
	      quat[z][i][j] = quat[0][i][j];
	    }
	  }
	}
}






void SW_Robot_Localization::Run()
{
  pthread_t thread_id[NUM_THREADS];
  struct thread_arg1_t thread1_arg[NUM_THREADS];
  for(int p = 0; p < NUM_THREADS; p++) {
    thread1_arg[p].tid = p;
    thread1_arg[p].dataSize = dataSize;
    thread1_arg[p].ran1 = ran1;
    thread1_arg[p].ran2 = ran2;
    thread1_arg[p].quat = quat;
    thread1_arg[p].accl = accl;
    thread1_arg[p].pos_in = pos_in;
    thread1_arg[p].vel_in = vel_in;
    thread1_arg[p].pos_out = pos_out;
     thread1_arg[p].vel_out = vel_out;
    thread1_arg[p].benchmark = this;
    pthread_create(&thread_id[p], NULL, Robot_Localization_thread, (void *)&thread1_arg[p]);
  }
  for(int p = 0; p < NUM_THREADS; p++) {
    pthread_join(thread_id[p], NULL);
  }

}           


void* Robot_Localization_thread(void* thread_arg) {

   struct thread_arg1_t *my_thread_arg = (struct thread_arg1_t *)thread_arg;
  int tid = my_thread_arg->tid;
  set_cpu(tid);
  int dataSize = my_thread_arg->dataSize;
  float (*ran1)[N_VAL][3] = my_thread_arg->ran1;
   float (*ran2)[N_VAL][3] = my_thread_arg->ran2;
  float (*quat)[N_VAL][4] = my_thread_arg->quat;
    float (*accl)[N_VAL][3] = my_thread_arg->accl;
    float (*pos_in)[N_VAL][3] = my_thread_arg->pos_in;
   float (*vel_in)[N_VAL][3]=my_thread_arg->vel_in;
   float (*pos_out)[N_VAL][3]=my_thread_arg->pos_out;
   float (*vel_out)[N_VAL][3]=my_thread_arg->vel_out;
  SW_Robot_Localization* benchmark = my_thread_arg->benchmark;

  int PP = dataSize/NUM_THREADS;
  for(int i = tid*PP; i < (tid+1)*PP; i++) {
		for (int j = 0; j < N_VAL; j++) {
			benchmark->RobLoc(ran1[i][j], ran2[i][j], quat[i][j], accl[i][j], pos_in[i][j], vel_in[i][j], pos_out[i][j], vel_out[i][j]);
		}
	}
}
void SW_Robot_Localization::Shutdown()
{
	delete [] ran1;
	delete [] ran2;
	delete [] quat;
	delete [] accl;
	delete [] pos_in;
	delete [] vel_in;
	delete [] pos_out;
	delete [] vel_out;

	while(true);
}
