#include "../../BenchmarkNode.h"
#include <ctime>
#include <stdint.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

#define WIDTHin         32
#define HEIGHTin        32
#define WIDTHout        512
#define HEIGHTout       32

inline float selectff(float input0, float input1, float input2, float input3)
{
	return (input0 < input1) ? input2 : input3;
}
inline int selectfi(float input0, float input1, int input2, int input3)
{
	return (input0 < input1) ? input2 : input3;
}
inline int selectii(int input0, int input1, int input2, int input3)
{
	return (input0 < input1) ? input2 : input3;
}
class SW_Texture_Synthesis : public BenchmarkNode
{	
	int dataSize;
	int thread;

	float (*image)[HEIGHTin][WIDTHin];
	float (*target)[HEIGHTout][WIDTHout];
	float (*result)[HEIGHTout][WIDTHout][1];
	unsigned int (*rand_seeds)[100];
	int (*atlas)[HEIGHTout][WIDTHout][2];
	int (*candlistx)[HEIGHTout-1][WIDTHout][4];
	int (*candlisty)[HEIGHTout-1][WIDTHout][4];
	int (*ii)[HEIGHTout];
	int (*jj)[WIDTHout];
	
	void TexSynInitVals(float in_image[HEIGHTin][WIDTHin], unsigned int in_rand_seeds[100],
						int in_j, int in_i, int out_atlas[2], float out_result[1]);
	void TexSynCreateCand(int in_atlas[HEIGHTout][WIDTHout][2],
						  unsigned int in_rand_seeds[100], int in_j, int in_i,
						  int out_candlistx[4], int out_candlisty[4]);
	void TexSynCompareAll(float in_image[HEIGHTin][WIDTHin], int in_x[4], int in_y[4],
						  float in_target[HEIGHTout][WIDTHout], int in_j, int in_i,
						  int out_best[2], float out_result[HEIGHTout][WIDTHout][1]);

public:
	SW_Texture_Synthesis()
	{
		std::cin >> dataSize;
	}
	virtual void Initialize(int threadID, int procID);
	virtual void Run();
	virtual void Shutdown();
};

BENCH_DECL(SW_Texture_Synthesis);

void SW_Texture_Synthesis::TexSynInitVals(float in_image[HEIGHTin][WIDTHin], unsigned int in_rand_seeds[100],
										  int in_j, int in_i, int out_atlas[2], float out_result[1])
{
#ifndef vrfinishx
#define vrfinishx	(WIDTHin - 1)
#endif
#ifndef vrfinishy
#define	vrfinishy	(HEIGHTin - 1)
#endif

#ifndef _RANDF_ // returns (32-bit) float value and updates unsigned int seeds
#define _RANDF_(seed_z_uint, seed_w_uint, rand_f) { \
    seed_z_uint = (unsigned int) (36969 * (seed_z_uint & 65535) + (seed_z_uint >> 16)); \
    seed_w_uint = (unsigned int) (18000 * (seed_w_uint & 65535) + (seed_w_uint >> 16)); \
    rand_f = (float) ( (unsigned int) ((seed_z_uint << 16) + seed_w_uint) / 0xFFFFFFFF ); \
}
#endif

	int tmpx, tmpy;
	float rand_val1, rand_val2;
	unsigned int seed_z, seed_w;

	seed_z = in_rand_seeds[in_j % 100];
	seed_w = in_rand_seeds[in_i % 100];
	_RANDF_(seed_z, seed_w, rand_val1);
	_RANDF_(seed_z, seed_w, rand_val2);
	tmpx = 1 + (int)(rand_val1 * (vrfinishx - 1));
	tmpy = 1 + (int)(rand_val2 * (vrfinishy - 1));
	out_atlas[0] = tmpx;
	out_atlas[1] = tmpy;
	out_result[0] = in_image[tmpy][tmpx];
}
void SW_Texture_Synthesis::TexSynCreateCand(int in_atlas[HEIGHTout][WIDTHout][2], unsigned int in_rand_seeds[100], int in_j, int in_i, int out_candlistx[4], int out_candlisty[4])
{
	// Creates a list of valid candidates for given pixel using only L-shaped causal area
	// NOTE: vrstartx = vrstarty = 1 for all data sizes except fullhd and wuxga

#ifndef vrfinishx
#define vrfinishx	(WIDTHin - 1)
#endif
#ifndef vrfinishy
#define	vrfinishy	(HEIGHTin - 1)
#endif

#ifndef _RANDF_ // returns (32-bit) float value and updates unsigned int seeds
#define _RANDF_(seed_z_uint, seed_w_uint, rand_f) { \
    seed_z_uint = (unsigned int) (36969 * (seed_z_uint & 65535) + (seed_z_uint >> 16)); \
    seed_w_uint = (unsigned int) (18000 * (seed_w_uint & 65535) + (seed_w_uint >> 16)); \
    rand_f = (float) ( (unsigned int) ((seed_z_uint << 16) + seed_w_uint) / 0xFFFFFFFF ); \
}
#endif
	
	float rand_val1, rand_val2;
	unsigned int seed_z, seed_w;
	unsigned int bool1, bool2, bool3, or_bools;
	int xloopout_0, xloopout_1, xloopout_2;
	int yloopout_0, yloopout_y;

	seed_z = in_rand_seeds[in_j % 100];
	seed_w = in_rand_seeds[in_i % 100];

	xloopout_0 = (WIDTHout + in_j - 1) % WIDTHout;
	xloopout_1 = (WIDTHout + in_j) % WIDTHout;
	xloopout_2 = (WIDTHout + in_j + 1) % WIDTHout;
	yloopout_0 = (HEIGHTout + in_i - 1) % HEIGHTout;
	yloopout_y = (HEIGHTout + in_i - 2) % HEIGHTout;

	//for (i = 0; i <= vrstarty; i++) { for (j = -vrstartx; j <= vrstartx; j++) {
	// i = 0, j = -1
	out_candlistx[0] = in_atlas[yloopout_y][xloopout_1][0] + 1;
	out_candlisty[0] = in_atlas[yloopout_y][xloopout_1][1];
	_RANDF_(seed_z, seed_w, rand_val1);
	_RANDF_(seed_z, seed_w, rand_val2);
	//if (out_candlistx[0] >= vrfinishx || out_candlistx[0] < 1 || out_candlisty[0] >= vrfinishy)
	bool1 = selectii(out_candlistx[0], vrfinishx, 1, 0);
	bool2 = selectii(1, out_candlistx[0], 1, 0);
	bool3 = selectii(out_candlisty[0], vrfinishy, 1, 0);
	or_bools = selectii(0, bool1 + bool2 + bool3, 1, 0);
	out_candlistx[0] = selectii(0, or_bools, 1 + (int)(rand_val1*(vrfinishx-1)), out_candlistx[0]);
	out_candlisty[0] = selectii(0, or_bools, 1 + (int)(rand_val2*(vrfinishy-1)), out_candlisty[0]);
	// i = 1, j = -1
	out_candlistx[1] = in_atlas[yloopout_0][xloopout_0][0] + 1;
	out_candlisty[1] = in_atlas[yloopout_0][xloopout_0][1] + 1;
	_RANDF_(seed_z, seed_w, rand_val1);
	_RANDF_(seed_z, seed_w, rand_val2);
	//if (out_candlistx[1] >= vrfinishx || out_candlistx[1] < 1 || out_candlisty[1] >= vrfinishy)
	bool1 = selectii(out_candlistx[1], vrfinishx, 1, 0);
	bool2 = selectii(1, out_candlistx[1], 1, 0);
	bool3 = selectii(out_candlisty[1], vrfinishy, 1, 0);
	or_bools = selectii(0, bool1 + bool2 + bool3, 1, 0);
	out_candlistx[1] = selectii(0, or_bools, 1 + (int)(rand_val1*(vrfinishx-1)), out_candlistx[1]);
	out_candlisty[1] = selectii(0, or_bools, 1 + (int)(rand_val2*(vrfinishy-1)), out_candlisty[1]);
	// i = 1, j = 0
	out_candlistx[2] = in_atlas[yloopout_0][xloopout_1][0];
	out_candlisty[2] = in_atlas[yloopout_0][xloopout_1][1] + 1;
	_RANDF_(seed_z, seed_w, rand_val1);
	_RANDF_(seed_z, seed_w, rand_val2);
	//if (out_candlistx[2] >= vrfinishx || out_candlistx[2] < 1 || out_candlisty[2] >= vrfinishy)
	bool1 = selectii(out_candlistx[2], vrfinishx, 1, 0);
	bool2 = selectii(1, out_candlistx[2], 1, 0);
	bool3 = selectii(out_candlisty[2], vrfinishy, 1, 0);
	or_bools = selectii(0, bool1 + bool2 + bool3, 1, 0);
	out_candlistx[2] = selectii(0, or_bools, 1 + (int)(rand_val1*(vrfinishx-1)), out_candlistx[2]);
	out_candlisty[2] = selectii(0, or_bools, 1 + (int)(rand_val2*(vrfinishy-1)), out_candlisty[2]);
	// i = 1, j = 1
	out_candlistx[3] = in_atlas[yloopout_0][xloopout_2][0] - 1;
	out_candlisty[3] = in_atlas[yloopout_0][xloopout_2][1] + 1;
	_RANDF_(seed_z, seed_w, rand_val1);
	_RANDF_(seed_z, seed_w, rand_val2);
	//if (out_candlistx[3] >= vrfinishx || out_candlistx[3] < 1 || out_candlisty[3] >= vrfinishy)
	bool1 = selectii(out_candlistx[3], vrfinishx, 1, 0);
	bool2 = selectii(1, out_candlistx[3], 1, 0);
	bool3 = selectii(out_candlisty[3], vrfinishy, 1, 0);
	or_bools = selectii(0, bool1 + bool2 + bool3, 1, 0);
	out_candlistx[3] = selectii(0, or_bools, 1 + (int)(rand_val1*(vrfinishx-1)), out_candlistx[3]);
	out_candlisty[3] = selectii(0, or_bools, 1 + (int)(rand_val2*(vrfinishy-1)), out_candlisty[3]);
}
void SW_Texture_Synthesis::TexSynCompareAll(float in_image[HEIGHTin][WIDTHin], int in_x[4], int in_y[4],
											float in_target[HEIGHTout][WIDTHout], int in_j, int in_i,
											int out_best[2], float out_result[HEIGHTout][WIDTHout][1])
{
	float tmp, diff, comp_diff;
	int cand_x, cand_y;
	int xloopout_0, xloopout_1, xloopout_2;
	int yloopout_0, yloopout_2, yloopout_y;

	xloopout_0 = (WIDTHout + in_j - 1) % WIDTHout;
	xloopout_1 = (WIDTHout + in_j) % WIDTHout;
	xloopout_2 = (WIDTHout + in_j + 1) % WIDTHout;
	yloopout_0 = (HEIGHTout + in_i - 1) % HEIGHTout;
	yloopout_2 = (HEIGHTout + in_i + 1) % HEIGHTout;
	yloopout_y = (HEIGHTout + in_i - 2) % HEIGHTout;

	comp_diff = 1e10;

	// *** COMPARE NEIGHB -- 0 ***
	
	diff = 0;
	cand_x = in_x[0];
	cand_y = in_y[0];
	tmp = in_image[cand_y-1][cand_x-1] - out_result[yloopout_0][xloopout_0][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y-1][cand_x] - out_result[yloopout_0][xloopout_1][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y-1][cand_x+1] - out_result[yloopout_0][xloopout_2][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y][cand_x-1] - out_result[yloopout_y][xloopout_1][0];
	diff += tmp * tmp;

	// *** COMPARE REST -- 0 ***

	diff = diff + selectff(in_target[yloopout_2][xloopout_0], 1.0, (in_image[cand_y+1][cand_x-1] - in_target[yloopout_2][xloopout_0]) * (in_image[cand_y+1][cand_x-1] - in_target[yloopout_2][xloopout_0]), 0);
	diff = diff + selectff(in_target[yloopout_2][xloopout_1], 1.0, (in_image[cand_y+1][cand_x] - in_target[yloopout_2][xloopout_1]) * (in_image[cand_y+1][cand_x] - in_target[yloopout_2][xloopout_1]), 0);
	diff = diff + selectff(in_target[yloopout_2][xloopout_2], 1.0, (in_image[cand_y+1][cand_x+1] - in_target[yloopout_2][xloopout_2]) * (in_image[cand_y+1][cand_x+1] - in_target[yloopout_2][xloopout_2]), 0);
	diff = diff + selectff(in_target[in_i][xloopout_2], 1.0, (in_image[cand_y][cand_x+1] - in_target[in_i][xloopout_2]) * (in_image[cand_y][cand_x+1] - in_target[in_i][xloopout_2]), 0);
	out_best[0] = selectfi(diff, comp_diff, cand_x, out_best[0]);
	out_best[1] = selectfi(diff, comp_diff, cand_y, out_best[1]);
	comp_diff = selectff(diff, comp_diff, diff, comp_diff);

	// *** COMPARE NEIGHB -- 1 ***
	
	diff = 0;
	cand_x = in_x[1];
	cand_y = in_y[1];
	tmp = in_image[cand_y-1][cand_x-1] - out_result[yloopout_0][xloopout_0][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y-1][cand_x] - out_result[yloopout_0][xloopout_1][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y-1][cand_x+1] - out_result[yloopout_0][xloopout_2][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y][cand_x-1] - out_result[yloopout_y][xloopout_1][0];
	diff += tmp * tmp;

	// *** COMPARE REST -- 1 ***

	diff = diff + selectff(in_target[yloopout_2][xloopout_0], 1.0, (in_image[cand_y+1][cand_x-1] - in_target[yloopout_2][xloopout_0]) * (in_image[cand_y+1][cand_x-1] - in_target[yloopout_2][xloopout_0]), 0);
	diff = diff + selectff(in_target[yloopout_2][xloopout_1], 1.0, (in_image[cand_y+1][cand_x] - in_target[yloopout_2][xloopout_1]) * (in_image[cand_y+1][cand_x] - in_target[yloopout_2][xloopout_1]), 0);
	diff = diff + selectff(in_target[yloopout_2][xloopout_2], 1.0, (in_image[cand_y+1][cand_x+1] - in_target[yloopout_2][xloopout_2]) * (in_image[cand_y+1][cand_x+1] - in_target[yloopout_2][xloopout_2]), 0);
	diff = diff + selectff(in_target[in_i][xloopout_2], 1.0, (in_image[cand_y][cand_x+1] - in_target[in_i][xloopout_2]) * (in_image[cand_y][cand_x+1] - in_target[in_i][xloopout_2]), 0);
	out_best[0] = selectfi(diff, comp_diff, cand_x, out_best[0]);
	out_best[1] = selectfi(diff, comp_diff, cand_y, out_best[1]);
	comp_diff = selectff(diff, comp_diff, diff, comp_diff);

	// *** COMPARE NEIGHB -- 2 ***
	
	diff = 0;
	cand_x = in_x[2];
	cand_y = in_y[2];
	tmp = in_image[cand_y-1][cand_x-1] - out_result[yloopout_0][xloopout_0][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y-1][cand_x] - out_result[yloopout_0][xloopout_1][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y-1][cand_x+1] - out_result[yloopout_0][xloopout_2][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y][cand_x-1] - out_result[yloopout_y][xloopout_1][0];
	diff += tmp * tmp;

	// *** COMPARE REST -- 2 ***

	diff = diff + selectff(in_target[yloopout_2][xloopout_0], 1.0, (in_image[cand_y+1][cand_x-1] - in_target[yloopout_2][xloopout_0]) * (in_image[cand_y+1][cand_x-1] - in_target[yloopout_2][xloopout_0]), 0);
	diff = diff + selectff(in_target[yloopout_2][xloopout_1], 1.0, (in_image[cand_y+1][cand_x] - in_target[yloopout_2][xloopout_1]) * (in_image[cand_y+1][cand_x] - in_target[yloopout_2][xloopout_1]), 0);
	diff = diff + selectff(in_target[yloopout_2][xloopout_2], 1.0, (in_image[cand_y+1][cand_x+1] - in_target[yloopout_2][xloopout_2]) * (in_image[cand_y+1][cand_x+1] - in_target[yloopout_2][xloopout_2]), 0);
	diff = diff + selectff(in_target[in_i][xloopout_2], 1.0, (in_image[cand_y][cand_x+1] - in_target[in_i][xloopout_2]) * (in_image[cand_y][cand_x+1] - in_target[in_i][xloopout_2]), 0);
	out_best[0] = selectfi(diff, comp_diff, cand_x, out_best[0]);
	out_best[1] = selectfi(diff, comp_diff, cand_y, out_best[1]);
	comp_diff = selectff(diff, comp_diff, diff, comp_diff);

	// *** COMPARE NEIGHB -- 3 ***
	
	diff = 0;
	cand_x = in_x[3];
	cand_y = in_y[3];
	tmp = in_image[cand_y-1][cand_x-1] - out_result[yloopout_0][xloopout_0][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y-1][cand_x] - out_result[yloopout_0][xloopout_1][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y-1][cand_x+1] - out_result[yloopout_0][xloopout_2][0];
	diff += tmp * tmp;
	tmp = in_image[cand_y][cand_x-1] - out_result[yloopout_y][xloopout_1][0];
	diff += tmp * tmp;

	// *** COMPARE REST -- 3 ***

	diff = diff + selectff(in_target[yloopout_2][xloopout_0], 1.0, (in_image[cand_y+1][cand_x-1] - in_target[yloopout_2][xloopout_0]) * (in_image[cand_y+1][cand_x-1] - in_target[yloopout_2][xloopout_0]), 0);
	diff = diff + selectff(in_target[yloopout_2][xloopout_1], 1.0, (in_image[cand_y+1][cand_x] - in_target[yloopout_2][xloopout_1]) * (in_image[cand_y+1][cand_x] - in_target[yloopout_2][xloopout_1]), 0);
	diff = diff + selectff(in_target[yloopout_2][xloopout_2], 1.0, (in_image[cand_y+1][cand_x+1] - in_target[yloopout_2][xloopout_2]) * (in_image[cand_y+1][cand_x+1] - in_target[yloopout_2][xloopout_2]), 0);
	diff = diff + selectff(in_target[in_i][xloopout_2], 1.0, (in_image[cand_y][cand_x+1] - in_target[in_i][xloopout_2]) * (in_image[cand_y][cand_x+1] - in_target[in_i][xloopout_2]), 0);
	out_best[0] = selectfi(diff, comp_diff, cand_x, out_best[0]);
	out_best[1] = selectfi(diff, comp_diff, cand_y, out_best[1]);
	comp_diff = selectff(diff, comp_diff, diff, comp_diff);

	out_result[in_i][in_j][0] = in_image[out_best[1]][out_best[0]];
}
void SW_Texture_Synthesis::Initialize(int threadID, int procID)
{
	thread = threadID;
	
	srand((unsigned int) time(NULL));
	
	image = new float[dataSize][HEIGHTin][WIDTHin];
	target = new float[dataSize][HEIGHTout][WIDTHout];
	result = new float[dataSize][HEIGHTout][WIDTHout][1];
	rand_seeds = new unsigned int[dataSize][100];
	atlas = new int[dataSize][HEIGHTout][WIDTHout][2];
	candlistx = new int[dataSize][HEIGHTout-1][WIDTHout][4];
	candlisty = new int[dataSize][HEIGHTout-1][WIDTHout][4];
	ii = new int[dataSize][HEIGHTout];
	jj = new int[dataSize][WIDTHout];
	
	for (int d = 0; d < dataSize; d++) {
		for (int i = 0; i < HEIGHTin; i++)
			for (int j = 0; j < WIDTHin; j++)
				image[d][i][j] = 0.0;
		for (int i = 0; i < HEIGHTout; i++)
			for (int j = 0; j < WIDTHout; j++)
				target[d][i][j] = result[d][i][j][0] = 1.0;
		for (int i = 0; i < 100; i++)
			rand_seeds[d][i] = (unsigned int) rand();
		for (int i = 0; i < HEIGHTout; i++)
			for (int j = 0; j < WIDTHout; j++)
				for (int k = 0; k < 2; k++)
					atlas[d][i][j][k] = 0;
		for (int i = 0; i < (HEIGHTout-1); i++)
			for (int j = 0; j < WIDTHout; j++)
				for (int k = 0; k < 4; k++)
					candlistx[d][i][j][k] = candlisty[d][i][j][k] = 0;
		for (int i = 0; i < HEIGHTout; i++)
			ii[d][i] = i;
		for (int j = 0; j < WIDTHout; j++)
			jj[d][j] = j;
	}
}
void SW_Texture_Synthesis::Run()
{
	for (int d = 0; d < dataSize; d++) {
		for (int i = 0; i < HEIGHTout; i++) {
			for (int j = 0; j < WIDTHout; j++) {
				TexSynInitVals(image[d], rand_seeds[d], jj[d][j], ii[d][i],
							   atlas[d][i][j], result[d][i][j]);
			}
		}
	}

	for (int d = 0; d < dataSize; d++) {
		for (int i = 0; i < (HEIGHTout - 1); i++) {
			for (int j = 0; j < WIDTHout; j++) {
				TexSynCreateCand(atlas[d], rand_seeds[d], jj[d][j], ii[d][i],
								 candlistx[d][i][j], candlisty[d][i][j]);
			}
		}
	}
	
	for (int d = 0; d < dataSize; d++) {
		for (int i = 0; i < (HEIGHTout - 1); i++) {
			for (int j = 0; j < WIDTHout; j++) {
				TexSynCompareAll(image[d], candlistx[d][i][j], candlisty[d][i][j],
								 target[d], jj[d][j], ii[d][i],
								 atlas[d][i][j], result[d]);
			}
		}
	}
}
void SW_Texture_Synthesis::Shutdown()
{
	delete [] image;
	delete [] target;
	delete [] result;
	delete [] rand_seeds;
	delete [] atlas;
	delete [] candlistx;
	delete [] candlisty;
	delete [] ii;
	delete [] jj;

	while(true);
}

