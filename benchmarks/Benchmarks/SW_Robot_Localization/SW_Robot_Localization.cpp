#include "../../BenchmarkNode.h"
#include <stdint.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

#define N_VAL               3


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
	
	void RobLoc(float in_ran1[3], float in_ran2[3],
				float in_quat[4], float in_accl[3],
				float in_pos[3], float in_vel[3],
				float out_pos[3], float out_vel[3]);

public:
	SW_Robot_Localization()
	{
		std::cin >> dataSize;
	}
	virtual void Initialize(int threadID, int procID);
	virtual void Run();
	virtual void Shutdown();
};

BENCH_DECL(SW_Robot_Localization);

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
	for (int i = 0; i < dataSize; i++) {
		for (int j = 0; j < N_VAL; j++) {
			RobLoc(ran1[i][j], ran2[i][j], quat[i][j], accl[i][j], pos_in[i][j], vel_in[i][j], pos_out[i][j], vel_out[i][j]);
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
