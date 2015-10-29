#include "../../BenchmarkNode.h"
#include <stdint.h>
#include <iostream>
#include <cmath>

class SW_EKF_SLAM : public BenchmarkNode
{
	int thread;
	int dataSize;

	float (*X)[7];
	float (*U)[7];
	float (*out_X_oplus_U)[7];
	float (*out_df_dx)[7*4];
	float (*cpose)[7];
	float (*point)[3];
	float (*out_jacob_dryp_dpoint)[3*3];
	float (*out_jacob_dryp_dpose)[3*7];

	void jacobiansPoseComposition(float input_X[7], float input_U[7], float output_df_dx[7*4], float output_X_oplus_U[7]);
	void sphericalCoordinates(float input_cpose[7], float input_point[3], float output_jacob_dryp_dpoint[3*3], float output_jacob_dryp_dpose[3*7]);

public:
	SW_EKF_SLAM()
	{
		std::cin >> dataSize;
	}
	virtual void Initialize(int threadID, int procID);
	virtual void Run();
	virtual void Shutdown();
};

BENCH_DECL(SW_EKF_SLAM);

void SW_EKF_SLAM::jacobiansPoseComposition(float input_X[7], float input_U[7], float output_df_dx[7*4], float output_X_oplus_U[7])
{
	// Note: First 3 columns of output_df_dx (e.g. [][0], [][1], and [][2])
	//       are const vals, so only 7x4 is needed instead of 7x7

	#define EKF_in_X input_X
	#define EKF_in_U input_U
	#define EKF_out_df_dx(r,c) output_df_dx[r*4+c-3]
	#define EKF_out_X_oplus_U  output_X_oplus_U

	typedef struct CPose3DQuat_type {
		float x, y, z, qr, qx, qy, qz;
	} CPose3DQuat;

	typedef struct _f_i_converter {
		union {
			float f;
			int i;
		};
	} f_i_converter;

	#ifndef opt_invSqrt_f_f
	#define opt_invSqrt_f_f(FLOAT_IN, FLOAT_OUT) { \
		f_i_converter c; \
		c.f = FLOAT_IN; \
		float xhalf = 0.5f * FLOAT_IN; \
		int i = c.i; \
		c.i = 0x5f3759d5 - (i >> 1); \
		FLOAT_OUT = c.f; \
		FLOAT_OUT = FLOAT_OUT*(1.5f - xhalf*FLOAT_OUT*FLOAT_OUT); \
	}
	#endif

	// Reconstruct CPose3DQuat input(s) to this function
	const CPose3DQuat X = {EKF_in_X[0], EKF_in_X[1], EKF_in_X[2], EKF_in_X[3], EKF_in_X[4], EKF_in_X[5], EKF_in_X[6]};
	const CPose3DQuat U = {EKF_in_U[0], EKF_in_U[1], EKF_in_U[2], EKF_in_U[3], EKF_in_U[4], EKF_in_U[5], EKF_in_U[6]};

	CPose3DQuat X_plus_U = {X.x+U.x, X.y+U.y, X.z+U.z, X.qr+U.qr, X.qx+U.qx, X.qy+U.qy, X.qz+U.qz};
	float norm_jacob[4][4];
	float X_plus_U_normSqr = X_plus_U.qr*X_plus_U.qr + X_plus_U.qx*X_plus_U.qx + X_plus_U.qy*X_plus_U.qy + X_plus_U.qz*X_plus_U.qz;
	float X_plus_U_normSqr_power3 = X_plus_U_normSqr * X_plus_U_normSqr * X_plus_U_normSqr;
	float X_plus_U_n;
	opt_invSqrt_f_f(X_plus_U_normSqr_power3, X_plus_U_n);
	norm_jacob[0][0] = X_plus_U_n * (X_plus_U.qx*X_plus_U.qx + X_plus_U.qy*X_plus_U.qy + X_plus_U.qz*X_plus_U.qz);
	norm_jacob[0][1] = X_plus_U_n * (-X_plus_U.qr * X_plus_U.qx);
	norm_jacob[0][2] = X_plus_U_n * (-X_plus_U.qr * X_plus_U.qy);
	norm_jacob[0][3] = X_plus_U_n * (-X_plus_U.qr * X_plus_U.qz);

	norm_jacob[1][0] = X_plus_U_n * (-X_plus_U.qx * X_plus_U.qr);
	norm_jacob[1][1] = X_plus_U_n * (X_plus_U.qr*X_plus_U.qr + X_plus_U.qy*X_plus_U.qy + X_plus_U.qz*X_plus_U.qz);
	norm_jacob[1][2] = X_plus_U_n * (-X_plus_U.qx * X_plus_U.qy);
	norm_jacob[1][3] = X_plus_U_n * (-X_plus_U.qx * X_plus_U.qz);

	norm_jacob[2][0] = X_plus_U_n * (-X_plus_U.qy * X_plus_U.qr);
	norm_jacob[2][1] = X_plus_U_n * (-X_plus_U.qy * X_plus_U.qx);
	norm_jacob[2][2] = X_plus_U_n * (X_plus_U.qr*X_plus_U.qr + X_plus_U.qx*X_plus_U.qx + X_plus_U.qz*X_plus_U.qz);
	norm_jacob[2][3] = X_plus_U_n * (-X_plus_U.qy * X_plus_U.qz);

	norm_jacob[3][0] = X_plus_U_n * (-X_plus_U.qz * X_plus_U.qr);
	norm_jacob[3][1] = X_plus_U_n * (-X_plus_U.qz * X_plus_U.qx);
	norm_jacob[3][2] = X_plus_U_n * (-X_plus_U.qz * X_plus_U.qy);
	norm_jacob[3][3] = X_plus_U_n * (X_plus_U.qr*X_plus_U.qr + X_plus_U.qx*X_plus_U.qx + X_plus_U.qy*X_plus_U.qy);

	float norm_jacob_X[4][4];
	float X_normSqr = X.qr*X.qr + X.qx*X.qx + X.qy*X.qy + X.qz*X.qz;
	float X_normSqr_power3 = X_normSqr * X_normSqr * X_normSqr;
	float X_n;
	opt_invSqrt_f_f(X_normSqr_power3, X_n);
	norm_jacob_X[0][0] = X_n * (X.qx*X.qx + X.qy*X.qy + X.qz*X.qz);
	norm_jacob_X[0][1] = X_n * (-X.qr * X.qx);
	norm_jacob_X[0][2] = X_n * (-X.qr * X.qy);
	norm_jacob_X[0][3] = X_n * (-X.qr * X.qz);

	norm_jacob_X[1][0] = X_n * (-X.qx * X.qr);
	norm_jacob_X[1][1] = X_n * (X.qr*X.qr + X.qy*X.qy + X.qz*X.qz);
	norm_jacob_X[1][2] = X_n * (-X.qx * X.qy);
	norm_jacob_X[1][3] = X_n * (-X.qx * X.qz);

	norm_jacob_X[2][0] = X_n * (-X.qy * X.qr);
	norm_jacob_X[2][1] = X_n * (-X.qy * X.qx);
	norm_jacob_X[2][2] = X_n * (X.qr*X.qr + X.qx*X.qx + X.qz*X.qz);
	norm_jacob_X[2][3] = X_n * (-X.qy * X.qz);

	norm_jacob_X[3][0] = X_n * (-X.qz * X.qr);
	norm_jacob_X[3][1] = X_n * (-X.qz * X.qx);
	norm_jacob_X[3][2] = X_n * (-X.qz * X.qy);
	norm_jacob_X[3][3] = X_n * (X.qr*X.qr + X.qx*X.qx + X.qy*X.qy);

	// EKF_out_df_dx ===================================================

	const float valsX[3][4] = {
		{ 2 * (-X.qz*U.y + X.qy*U.z ),
		  2 * ( X.qy*U.y + X.qz*U.z ),
		  2 * (-2*X.qy*U.x + X.qx*U.y + X.qr*U.z ),
		  2 * (-2*X.qz*U.x - X.qr*U.y + X.qx*U.z )  },

		{ 2 * ( X.qz*U.x - X.qx*U.z ),
		  2 * ( X.qy*U.x - 2*X.qx*U.y - X.qr*U.z ),
		  2 * ( X.qx*U.x + X.qz*U.z ),
		  2 * ( X.qr*U.x - 2*X.qz*U.y + X.qy*U.z )  },

		{ 2 * (-X.qy*U.x + X.qx*U.y ),
		  2 * ( X.qz*U.x + X.qr*U.y - 2*X.qx*U.z ),
		  2 * (-X.qr*U.x + X.qz*U.y - 2*X.qy*U.z ),
		  2 * ( X.qx*U.x + X.qy*U.y )               }
	};

	const float aux44_dataX[4][4] = {
		{ U.qr, -U.qx, -U.qy, -U.qz },
		{ U.qx,  U.qr,  U.qz, -U.qy },
		{ U.qy, -U.qz,  U.qr,  U.qx },
		{ U.qz,  U.qy, -U.qx,  U.qr }
	};

	//EKF_out_df_dx(0,0) = 1;
	//EKF_out_df_dx(0,1) = 0;
	//EKF_out_df_dx(0,2) = 0;
	EKF_out_df_dx(0,3) = valsX[0][0]*norm_jacob_X[0][0] + valsX[0][1]*norm_jacob_X[1][0] + valsX[0][2]*norm_jacob_X[2][0] + valsX[0][3]*norm_jacob_X[3][0];
	EKF_out_df_dx(0,4) = valsX[0][0]*norm_jacob_X[0][1] + valsX[0][1]*norm_jacob_X[1][1] + valsX[0][2]*norm_jacob_X[2][1] + valsX[0][3]*norm_jacob_X[3][1];
	EKF_out_df_dx(0,5) = valsX[0][0]*norm_jacob_X[0][2] + valsX[0][1]*norm_jacob_X[1][2] + valsX[0][2]*norm_jacob_X[2][2] + valsX[0][3]*norm_jacob_X[3][2];
	EKF_out_df_dx(0,6) = valsX[0][0]*norm_jacob_X[0][3] + valsX[0][1]*norm_jacob_X[1][3] + valsX[0][2]*norm_jacob_X[2][3] + valsX[0][3]*norm_jacob_X[3][3];

	//EKF_out_df_dx(1,0) = 0;
	//EKF_out_df_dx(1,1) = 1;
	//EKF_out_df_dx(1,2) = 0;
	EKF_out_df_dx(1,3) = valsX[1][0]*norm_jacob_X[0][0] + valsX[1][1]*norm_jacob_X[1][0] + valsX[1][2]*norm_jacob_X[2][0] + valsX[1][3]*norm_jacob_X[3][0];
	EKF_out_df_dx(1,4) = valsX[1][0]*norm_jacob_X[0][1] + valsX[1][1]*norm_jacob_X[1][1] + valsX[1][2]*norm_jacob_X[2][1] + valsX[1][3]*norm_jacob_X[3][1];
	EKF_out_df_dx(1,5) = valsX[1][0]*norm_jacob_X[0][2] + valsX[1][1]*norm_jacob_X[1][2] + valsX[1][2]*norm_jacob_X[2][2] + valsX[1][3]*norm_jacob_X[3][2];
	EKF_out_df_dx(1,6) = valsX[1][0]*norm_jacob_X[0][3] + valsX[1][1]*norm_jacob_X[1][3] + valsX[1][2]*norm_jacob_X[2][3] + valsX[1][3]*norm_jacob_X[3][3];

	//EKF_out_df_dx(2,0) = 0;
	//EKF_out_df_dx(2,1) = 0;
	//EKF_out_df_dx(2,2) = 1;
	EKF_out_df_dx(2,3) = valsX[2][0]*norm_jacob_X[0][0] + valsX[2][1]*norm_jacob_X[1][0] + valsX[2][2]*norm_jacob_X[2][0] + valsX[2][3]*norm_jacob_X[3][0];
	EKF_out_df_dx(2,4) = valsX[2][0]*norm_jacob_X[0][1] + valsX[2][1]*norm_jacob_X[1][1] + valsX[2][2]*norm_jacob_X[2][1] + valsX[2][3]*norm_jacob_X[3][1];
	EKF_out_df_dx(2,5) = valsX[2][0]*norm_jacob_X[0][2] + valsX[2][1]*norm_jacob_X[1][2] + valsX[2][2]*norm_jacob_X[2][2] + valsX[2][3]*norm_jacob_X[3][2];
	EKF_out_df_dx(2,6) = valsX[2][0]*norm_jacob_X[0][3] + valsX[2][1]*norm_jacob_X[1][3] + valsX[2][2]*norm_jacob_X[2][3] + valsX[2][3]*norm_jacob_X[3][3];

	//EKF_out_df_dx(3,0) = 0;
	//EKF_out_df_dx(3,1) = 0;
	//EKF_out_df_dx(3,2) = 0;
	EKF_out_df_dx(3,3) = norm_jacob[0][0]*aux44_dataX[0][0] + norm_jacob[0][1]*aux44_dataX[1][0] + norm_jacob[0][2]*aux44_dataX[2][0] + norm_jacob[0][3]*aux44_dataX[3][0];
	EKF_out_df_dx(3,4) = norm_jacob[0][0]*aux44_dataX[0][1] + norm_jacob[0][1]*aux44_dataX[1][1] + norm_jacob[0][2]*aux44_dataX[2][1] + norm_jacob[0][3]*aux44_dataX[3][1];
	EKF_out_df_dx(3,5) = norm_jacob[0][0]*aux44_dataX[0][2] + norm_jacob[0][1]*aux44_dataX[1][2] + norm_jacob[0][2]*aux44_dataX[2][2] + norm_jacob[0][3]*aux44_dataX[3][2];
	EKF_out_df_dx(3,6) = norm_jacob[0][0]*aux44_dataX[0][3] + norm_jacob[0][1]*aux44_dataX[1][3] + norm_jacob[0][2]*aux44_dataX[2][3] + norm_jacob[0][3]*aux44_dataX[3][3];

	//EKF_out_df_dx(4,0) = 0;
	//EKF_out_df_dx(4,1) = 0;
	//EKF_out_df_dx(4,2) = 0;
	EKF_out_df_dx(4,3) = norm_jacob[1][0]*aux44_dataX[0][0] + norm_jacob[1][1]*aux44_dataX[1][0] + norm_jacob[1][2]*aux44_dataX[2][0] + norm_jacob[1][3]*aux44_dataX[3][0];
	EKF_out_df_dx(4,4) = norm_jacob[1][0]*aux44_dataX[0][1] + norm_jacob[1][1]*aux44_dataX[1][1] + norm_jacob[1][2]*aux44_dataX[2][1] + norm_jacob[1][3]*aux44_dataX[3][1];
	EKF_out_df_dx(4,5) = norm_jacob[1][0]*aux44_dataX[0][2] + norm_jacob[1][1]*aux44_dataX[1][2] + norm_jacob[1][2]*aux44_dataX[2][2] + norm_jacob[1][3]*aux44_dataX[3][2];
	EKF_out_df_dx(4,6) = norm_jacob[1][0]*aux44_dataX[0][3] + norm_jacob[1][1]*aux44_dataX[1][3] + norm_jacob[1][2]*aux44_dataX[2][3] + norm_jacob[1][3]*aux44_dataX[3][3];

	//EKF_out_df_dx(5,0) = 0;
	//EKF_out_df_dx(5,1) = 0;
	//EKF_out_df_dx(5,2) = 0;
	EKF_out_df_dx(5,3) = norm_jacob[2][0]*aux44_dataX[0][0] + norm_jacob[2][1]*aux44_dataX[1][0] + norm_jacob[2][2]*aux44_dataX[2][0] + norm_jacob[2][3]*aux44_dataX[3][0];
	EKF_out_df_dx(5,4) = norm_jacob[2][0]*aux44_dataX[0][1] + norm_jacob[2][1]*aux44_dataX[1][1] + norm_jacob[2][2]*aux44_dataX[2][1] + norm_jacob[2][3]*aux44_dataX[3][1];
	EKF_out_df_dx(5,5) = norm_jacob[2][0]*aux44_dataX[0][2] + norm_jacob[2][1]*aux44_dataX[1][2] + norm_jacob[2][2]*aux44_dataX[2][2] + norm_jacob[2][3]*aux44_dataX[3][2];
	EKF_out_df_dx(5,6) = norm_jacob[2][0]*aux44_dataX[0][3] + norm_jacob[2][1]*aux44_dataX[1][3] + norm_jacob[2][2]*aux44_dataX[2][3] + norm_jacob[2][3]*aux44_dataX[3][3];

	//EKF_out_df_dx(6,0) = 0;
	//EKF_out_df_dx(6,1) = 0;
	//EKF_out_df_dx(6,2) = 0;
	EKF_out_df_dx(6,3) = norm_jacob[3][0]*aux44_dataX[0][0] + norm_jacob[3][1]*aux44_dataX[1][0] + norm_jacob[3][2]*aux44_dataX[2][0] + norm_jacob[3][3]*aux44_dataX[3][0];
	EKF_out_df_dx(6,4) = norm_jacob[3][0]*aux44_dataX[0][1] + norm_jacob[3][1]*aux44_dataX[1][1] + norm_jacob[3][2]*aux44_dataX[2][1] + norm_jacob[3][3]*aux44_dataX[3][1];
	EKF_out_df_dx(6,5) = norm_jacob[3][0]*aux44_dataX[0][2] + norm_jacob[3][1]*aux44_dataX[1][2] + norm_jacob[3][2]*aux44_dataX[2][2] + norm_jacob[3][3]*aux44_dataX[3][2];
	EKF_out_df_dx(6,6) = norm_jacob[3][0]*aux44_dataX[0][3] + norm_jacob[3][1]*aux44_dataX[1][3] + norm_jacob[3][2]*aux44_dataX[2][3] + norm_jacob[3][3]*aux44_dataX[3][3];

	EKF_out_X_oplus_U[0] = X_plus_U.x;
	EKF_out_X_oplus_U[1] = X_plus_U.y;
	EKF_out_X_oplus_U[2] = X_plus_U.z;
	EKF_out_X_oplus_U[3] = X_plus_U.qr;
	EKF_out_X_oplus_U[4] = X_plus_U.qx;
	EKF_out_X_oplus_U[5] = X_plus_U.qy;
	EKF_out_X_oplus_U[6] = X_plus_U.qz;
}
void SW_EKF_SLAM::sphericalCoordinates(float input_cpose[7], float input_point[3], float output_jacob_dryp_dpoint[3*3], float output_jacob_dryp_dpose[3*7])
{
	#define EKF_in_cpose input_cpose
	#define EKF_in_point input_point
	#define EKF_out_jacob_dryp_dpoint(r,c) output_jacob_dryp_dpoint[r*3+c]
	#define EKF_out_jacob_dryp_dpose(r,c)  output_jacob_dryp_dpose[r*7+c]

	typedef struct TPoint3D_type {
		float x, y, z;
	} TPoint3D;

	typedef struct _f_i_converter {
		union {
			float f;
			int i;
		};
	} f_i_converter;

	#ifndef opt_invSqrt_f_f
	#define opt_invSqrt_f_f(FLOAT_IN, FLOAT_OUT) { \
		f_i_converter c; \
		c.f = FLOAT_IN; \
		float xhalf = 0.5f * FLOAT_IN; \
		int i = c.i; \
		c.i = 0x5f3759d5 - (i >> 1); \
		FLOAT_OUT = c.f; \
		FLOAT_OUT = FLOAT_OUT*(1.5f - xhalf*FLOAT_OUT*FLOAT_OUT); \
	}
	#endif

	// Reconstruct TPoint3D input(s) to this function
	const TPoint3D point = {EKF_in_point[0], EKF_in_point[1], EKF_in_point[2]};

	const float m_coord[3] = {EKF_in_cpose[0], EKF_in_cpose[1], EKF_in_cpose[2]};
	const float m_quat[4] = {EKF_in_cpose[3], EKF_in_cpose[4], EKF_in_cpose[5], EKF_in_cpose[6]};

	// Pass to coordinates as seen from this 6D pose:
	float jacob_dinv_dpoint[3][3];
	float jacob_dinv_dpose[3][7];

	// NOTE: Array represents a matrix as Matrix[row][column]
	TPoint3D local;

	// inverseComposePoint();
	{
		const float qr = m_quat[0];
		const float qx = m_quat[1];
		const float qy = m_quat[2];
		const float qz = m_quat[3];
		const float qx2 = qx * qx;
		const float qy2 = qy * qy;
		const float qz2 = qz * qz;

		const float Ax = 2 * (point.x - m_coord[0]);
		const float Ay = 2 * (point.y - m_coord[1]);
		const float Az = 2 * (point.z - m_coord[2]);

		jacob_dinv_dpoint[0][0] = 1 - 2 * (qy2 + qz2);
		jacob_dinv_dpoint[0][1] = 2 * (-qr * qz + qx * qy);
		jacob_dinv_dpoint[0][2] = 2 * (qx * qz + qr * qy);

		jacob_dinv_dpoint[1][0] = 2 * (qx * qy + qr * qz);
		jacob_dinv_dpoint[1][1] = 1 - 2 * (qx2 + qz2);
		jacob_dinv_dpoint[1][2] = 2 * (-qr * qx + qy * qz);

		jacob_dinv_dpoint[2][0] = 2 * (-qr * qy + qx * qz);
		jacob_dinv_dpoint[2][1] = 2 * (qy * qz + qr * qx);
		jacob_dinv_dpoint[2][2] = 1 - 2 * (qx2 + qy2);


		float normSqr = qr*qr + qx*qx + qy*qy + qz*qz;
		float normSqr_power3 = normSqr * normSqr * normSqr;
		float n;
		opt_invSqrt_f_f(normSqr_power3, n);

		jacob_dinv_dpose[0][0] = 2*qy2 + 2*qz2 - 1;
		jacob_dinv_dpose[0][1] = -2*qr*qz - 2*qx*qy;
		jacob_dinv_dpose[0][2] = 2*qr*qy - 2*qx*qz;
		jacob_dinv_dpose[0][3] = n * ( (-qy*Az + qz*Ay)*(qx*qx + qy*qy + qz*qz) + (qy*Ay + qz*Az)*(-qx * qr) + (qx*Ay - 2*qy*Ax - qr*Az)*(-qy * qr) + (qx*Az + qr*Ay - 2*qz*Ax)*(-qz * qr) );
		jacob_dinv_dpose[0][4] = n * ( (-qy*Az + qz*Ay)*(-qr * qx) + (qy*Ay + qz*Az)*(qr*qr + qy*qy + qz*qz) + (qx*Ay - 2*qy*Ax - qr*Az)*(-qy * qx) + (qx*Az + qr*Ay - 2*qz*Ax)*(-qz * qx) );
		jacob_dinv_dpose[0][5] = n * ( (-qy*Az + qz*Ay)*(-qr * qy) + (qy*Ay + qz*Az)*(-qx * qy) + (qx*Ay - 2*qy*Ax - qr*Az)*(qr*qr + qx*qx + qz*qz) + (qx*Az + qr*Ay - 2*qz*Ax)*(-qz * qy) );
		jacob_dinv_dpose[0][6] = n * ( (-qy*Az + qz*Ay)*(-qr * qz) + (qy*Ay + qz*Az)*(-qx * qz) + (qx*Ay - 2*qy*Ax - qr*Az)*(-qy * qz) + (qx*Az + qr*Ay - 2*qz*Ax)*(qr*qr + qx*qx + qy*qy) );

		jacob_dinv_dpose[1][0] = 2*qr*qz - 2*qx*qy;
		jacob_dinv_dpose[1][1] = 2*qx2 + 2*qz2 - 1;
		jacob_dinv_dpose[1][2] = -2*qr*qx - 2*qy*qz;
		jacob_dinv_dpose[1][3] = n * ( (qx*Az - qz*Ax)*(qx*qx + qy*qy + qz*qz) + (qy*Ax - 2*qx*Ay + qr*Az)*(-qx * qr) + (qx*Ax + qz*Az)*(-qy * qr) + (qy*Az - 2*qz*Ay - qr*Ax)*(-qz * qr) );
		jacob_dinv_dpose[1][4] = n * ( (qx*Az - qz*Ax)*(-qr * qx) + (qy*Ax - 2*qx*Ay + qr*Az)*(qr*qr + qy*qy + qz*qz) + (qx*Ax + qz*Az)*(-qy * qx) + (qy*Az - 2*qz*Ay - qr*Ax)*(-qz * qx) );
		jacob_dinv_dpose[1][5] = n * ( (qx*Az - qz*Ax)*(-qr * qy) + (qy*Ax - 2*qx*Ay + qr*Az)*(-qx * qy) + (qx*Ax + qz*Az)*(qr*qr + qx*qx + qz*qz) + (qy*Az - 2*qz*Ay - qr*Ax)*(-qz * qy) );
		jacob_dinv_dpose[1][6] = n * ( (qx*Az - qz*Ax)*(-qr * qz) + (qy*Ax - 2*qx*Ay + qr*Az)*(-qx * qz) + (qx*Ax + qz*Az)*(-qy * qz) + (qy*Az - 2*qz*Ay - qr*Ax)*(qr*qr + qx*qx + qy*qy) );

		jacob_dinv_dpose[2][0] = -2*qr*qy - 2*qx*qz;
		jacob_dinv_dpose[2][1] = 2*qr*qx - 2*qy*qz;
		jacob_dinv_dpose[2][2] = 2*qx2 + 2*qy2 - 1;
		jacob_dinv_dpose[2][3] = n * ( (qy*Ax - qx*Ay)*(qx*qx + qy*qy + qz*qz) + (qz*Ax - qr*Ay - 2*qx*Az)*(-qx * qr) + (qr*Ax + qz*Ay - 2*qy*Az)*(-qy * qr) + (qx*Ax + qy*Ay)*(-qz * qr) );
		jacob_dinv_dpose[2][4] = n * ( (qy*Ax - qx*Ay)*(-qr * qx) + (qz*Ax - qr*Ay - 2*qx*Az)*(qr*qr + qy*qy + qz*qz) + (qr*Ax + qz*Ay - 2*qy*Az)*(-qy * qx) + (qx*Ax + qy*Ay)*(-qz * qx) );
		jacob_dinv_dpose[2][5] = n * ( (qy*Ax - qx*Ay)*(-qr * qy) + (qz*Ax - qr*Ay - 2*qx*Az)*(-qx * qy) + (qr*Ax + qz*Ay - 2*qy*Az)*(qr*qr + qx*qx + qz*qz) + (qx*Ax + qy*Ay)*(-qz * qy) );
		jacob_dinv_dpose[2][6] = n * ( (qy*Ax - qx*Ay)*(-qr * qz) + (qz*Ax - qr*Ay - 2*qx*Az)*(-qx * qz) + (qr*Ax + qz*Ay - 2*qy*Az)*(-qy * qz) + (qx*Ax + qy*Ay)*(qr*qr + qx*qx + qy*qy) );

		// Function itself:
		const float t2 = -qr*qx; const float t3 = -qr*qy; const float t4 = -qr*qz;
		const float t5 = -qx*qx; const float t6 =  qx*qy; const float t7 =  qx*qz;
		const float t8 = -qy*qy; const float t9 =  qy*qz; const float t10= -qz*qz;
		local.x = 2 * ( (t8+t10)*(point.x-m_coord[0]) + (t6-t4)*(point.y-m_coord[1])  + (t3+t7)*(point.z-m_coord[2]) ) + (point.x-m_coord[0]);
		local.y = 2 * ( (t4+t6)*(point.x-m_coord[0])  + (t5+t10)*(point.y-m_coord[1]) + (t9-t2)*(point.z-m_coord[2]) ) + (point.y-m_coord[1]);
		local.z = 2 * ( (t7-t3)*(point.x-m_coord[0])  + (t2+t9)*(point.y-m_coord[1])  + (t5+t8)*(point.z-m_coord[2]) ) + (point.z-m_coord[2]);
	}

	// Range:
	float dist_squared = local.x*local.x + local.y*local.y + local.z*local.z;
	float out_range;
	opt_invSqrt_f_f(dist_squared, out_range);

	// Jacobians are: dryp_dpoint = dryp_dlocalpoint * dinv_dpoint    and    dryp_dpose = dryp_dlocalpoint * dinv_dpose
	// NOTE: Jacobians are undefined if range is 0
	if (local.x != 0 && local.y != 0 && local.z != 0) {
		const float out_range2 = out_range * out_range;
		const float _r = out_range;
		const float x2 = local.x * local.x;
		const float y2 = local.y * local.y;
		float x2_plus_y2 = x2 + y2;
		float t2;
		opt_invSqrt_f_f(x2_plus_y2, t2);
		const float _K = t2 * out_range2;

		float dryp_dlocalpoint[3][3]= {
			{ local.x*_r,                local.y*_r,            local.z*_r       },
			{ -local.y/(x2*(y2/x2 + 1)), 1/(local.x*(y2/x2+1)), 0                },
			{ local.x*local.z*_K,        local.y*local.z*_K,    -(out_range2)/t2 }
		};

		//EKF_out_jacob_dryp_dpoint->multiply(dryp_dlocalpoint,jacob_dinv_dpoint);
		EKF_out_jacob_dryp_dpoint(0,0) = dryp_dlocalpoint[0][0]*jacob_dinv_dpoint[0][0] + dryp_dlocalpoint[0][1]*jacob_dinv_dpoint[1][0] + dryp_dlocalpoint[0][2]*jacob_dinv_dpoint[2][0];
		EKF_out_jacob_dryp_dpoint(0,1) = dryp_dlocalpoint[0][0]*jacob_dinv_dpoint[0][1] + dryp_dlocalpoint[0][1]*jacob_dinv_dpoint[1][1] + dryp_dlocalpoint[0][2]*jacob_dinv_dpoint[2][1];
		EKF_out_jacob_dryp_dpoint(0,2) = dryp_dlocalpoint[0][0]*jacob_dinv_dpoint[0][2] + dryp_dlocalpoint[0][1]*jacob_dinv_dpoint[1][2] + dryp_dlocalpoint[0][2]*jacob_dinv_dpoint[2][2];

		EKF_out_jacob_dryp_dpoint(1,0) = dryp_dlocalpoint[1][0]*jacob_dinv_dpoint[0][0] + dryp_dlocalpoint[1][1]*jacob_dinv_dpoint[1][0] + dryp_dlocalpoint[1][2]*jacob_dinv_dpoint[2][0];
		EKF_out_jacob_dryp_dpoint(1,1) = dryp_dlocalpoint[1][0]*jacob_dinv_dpoint[0][1] + dryp_dlocalpoint[1][1]*jacob_dinv_dpoint[1][1] + dryp_dlocalpoint[1][2]*jacob_dinv_dpoint[2][1];
		EKF_out_jacob_dryp_dpoint(1,2) = dryp_dlocalpoint[1][0]*jacob_dinv_dpoint[0][2] + dryp_dlocalpoint[1][1]*jacob_dinv_dpoint[1][2] + dryp_dlocalpoint[1][2]*jacob_dinv_dpoint[2][2];

		EKF_out_jacob_dryp_dpoint(2,0) = dryp_dlocalpoint[2][0]*jacob_dinv_dpoint[0][0] + dryp_dlocalpoint[2][1]*jacob_dinv_dpoint[1][0] + dryp_dlocalpoint[2][2]*jacob_dinv_dpoint[2][0];
		EKF_out_jacob_dryp_dpoint(2,1) = dryp_dlocalpoint[2][0]*jacob_dinv_dpoint[0][1] + dryp_dlocalpoint[2][1]*jacob_dinv_dpoint[1][1] + dryp_dlocalpoint[2][2]*jacob_dinv_dpoint[2][1];
		EKF_out_jacob_dryp_dpoint(2,2) = dryp_dlocalpoint[2][0]*jacob_dinv_dpoint[0][2] + dryp_dlocalpoint[2][1]*jacob_dinv_dpoint[1][2] + dryp_dlocalpoint[2][2]*jacob_dinv_dpoint[2][2];

		//EKF_out_jacob_dryp_dpose->multiply(dryp_dlocalpoint,jacob_dinv_dpose);
		EKF_out_jacob_dryp_dpose(0,0) = dryp_dlocalpoint[0][0]*jacob_dinv_dpose[0][0] + dryp_dlocalpoint[0][1]*jacob_dinv_dpose[1][0] + dryp_dlocalpoint[0][2]*jacob_dinv_dpose[2][0];
		EKF_out_jacob_dryp_dpose(0,1) = dryp_dlocalpoint[0][0]*jacob_dinv_dpose[0][1] + dryp_dlocalpoint[0][1]*jacob_dinv_dpose[1][1] + dryp_dlocalpoint[0][2]*jacob_dinv_dpose[2][1];
		EKF_out_jacob_dryp_dpose(0,2) = dryp_dlocalpoint[0][0]*jacob_dinv_dpose[0][2] + dryp_dlocalpoint[0][1]*jacob_dinv_dpose[1][2] + dryp_dlocalpoint[0][2]*jacob_dinv_dpose[2][2];
		EKF_out_jacob_dryp_dpose(0,3) = dryp_dlocalpoint[0][0]*jacob_dinv_dpose[0][3] + dryp_dlocalpoint[0][1]*jacob_dinv_dpose[1][3] + dryp_dlocalpoint[0][2]*jacob_dinv_dpose[2][3];
		EKF_out_jacob_dryp_dpose(0,4) = dryp_dlocalpoint[0][0]*jacob_dinv_dpose[0][4] + dryp_dlocalpoint[0][1]*jacob_dinv_dpose[1][4] + dryp_dlocalpoint[0][2]*jacob_dinv_dpose[2][4];
		EKF_out_jacob_dryp_dpose(0,5) = dryp_dlocalpoint[0][0]*jacob_dinv_dpose[0][5] + dryp_dlocalpoint[0][1]*jacob_dinv_dpose[1][5] + dryp_dlocalpoint[0][2]*jacob_dinv_dpose[2][5];
		EKF_out_jacob_dryp_dpose(0,6) = dryp_dlocalpoint[0][0]*jacob_dinv_dpose[0][6] + dryp_dlocalpoint[0][1]*jacob_dinv_dpose[1][6] + dryp_dlocalpoint[0][2]*jacob_dinv_dpose[2][6];

		EKF_out_jacob_dryp_dpose(1,0) = dryp_dlocalpoint[1][0]*jacob_dinv_dpose[0][0] + dryp_dlocalpoint[1][1]*jacob_dinv_dpose[1][0] + dryp_dlocalpoint[1][2]*jacob_dinv_dpose[2][0];
		EKF_out_jacob_dryp_dpose(1,1) = dryp_dlocalpoint[1][0]*jacob_dinv_dpose[0][1] + dryp_dlocalpoint[1][1]*jacob_dinv_dpose[1][1] + dryp_dlocalpoint[1][2]*jacob_dinv_dpose[2][1];
		EKF_out_jacob_dryp_dpose(1,2) = dryp_dlocalpoint[1][0]*jacob_dinv_dpose[0][2] + dryp_dlocalpoint[1][1]*jacob_dinv_dpose[1][2] + dryp_dlocalpoint[1][2]*jacob_dinv_dpose[2][2];
		EKF_out_jacob_dryp_dpose(1,3) = dryp_dlocalpoint[1][0]*jacob_dinv_dpose[0][3] + dryp_dlocalpoint[1][1]*jacob_dinv_dpose[1][3] + dryp_dlocalpoint[1][2]*jacob_dinv_dpose[2][3];
		EKF_out_jacob_dryp_dpose(1,4) = dryp_dlocalpoint[1][0]*jacob_dinv_dpose[0][4] + dryp_dlocalpoint[1][1]*jacob_dinv_dpose[1][4] + dryp_dlocalpoint[1][2]*jacob_dinv_dpose[2][4];
		EKF_out_jacob_dryp_dpose(1,5) = dryp_dlocalpoint[1][0]*jacob_dinv_dpose[0][5] + dryp_dlocalpoint[1][1]*jacob_dinv_dpose[1][5] + dryp_dlocalpoint[1][2]*jacob_dinv_dpose[2][5];
		EKF_out_jacob_dryp_dpose(1,6) = dryp_dlocalpoint[1][0]*jacob_dinv_dpose[0][6] + dryp_dlocalpoint[1][1]*jacob_dinv_dpose[1][6] + dryp_dlocalpoint[1][2]*jacob_dinv_dpose[2][6];

		EKF_out_jacob_dryp_dpose(2,0) = dryp_dlocalpoint[2][0]*jacob_dinv_dpose[0][0] + dryp_dlocalpoint[2][1]*jacob_dinv_dpose[1][0] + dryp_dlocalpoint[2][2]*jacob_dinv_dpose[2][0];
		EKF_out_jacob_dryp_dpose(2,1) = dryp_dlocalpoint[2][0]*jacob_dinv_dpose[0][1] + dryp_dlocalpoint[2][1]*jacob_dinv_dpose[1][1] + dryp_dlocalpoint[2][2]*jacob_dinv_dpose[2][1];
		EKF_out_jacob_dryp_dpose(2,2) = dryp_dlocalpoint[2][0]*jacob_dinv_dpose[0][2] + dryp_dlocalpoint[2][1]*jacob_dinv_dpose[1][2] + dryp_dlocalpoint[2][2]*jacob_dinv_dpose[2][2];
		EKF_out_jacob_dryp_dpose(2,3) = dryp_dlocalpoint[2][0]*jacob_dinv_dpose[0][3] + dryp_dlocalpoint[2][1]*jacob_dinv_dpose[1][3] + dryp_dlocalpoint[2][2]*jacob_dinv_dpose[2][3];
		EKF_out_jacob_dryp_dpose(2,4) = dryp_dlocalpoint[2][0]*jacob_dinv_dpose[0][4] + dryp_dlocalpoint[2][1]*jacob_dinv_dpose[1][4] + dryp_dlocalpoint[2][2]*jacob_dinv_dpose[2][4];
		EKF_out_jacob_dryp_dpose(2,5) = dryp_dlocalpoint[2][0]*jacob_dinv_dpose[0][5] + dryp_dlocalpoint[2][1]*jacob_dinv_dpose[1][5] + dryp_dlocalpoint[2][2]*jacob_dinv_dpose[2][5];
		EKF_out_jacob_dryp_dpose(2,6) = dryp_dlocalpoint[2][0]*jacob_dinv_dpose[0][6] + dryp_dlocalpoint[2][1]*jacob_dinv_dpose[1][6] + dryp_dlocalpoint[2][2]*jacob_dinv_dpose[2][6];
	}
}
void SW_EKF_SLAM::Initialize(int threadID, int procID)
{
	thread = threadID;

	X = new float[dataSize][7];
	U = new float[dataSize][7];
	out_df_dx = new float[dataSize][7*4];
	out_X_oplus_U = new float[dataSize][7];
	cpose = new float[dataSize][7];
	point = new float[dataSize][3];
	out_jacob_dryp_dpoint = new float[dataSize][3*3];
	out_jacob_dryp_dpose = new float[dataSize][3*7];

	for (int i = 0; i < dataSize; i++) {
		for (int x = 0; x < 7; x++) {
			for (int y = 0; y < 4; y++) {
				out_df_dx[i][x * 4 + y] = 0;
			}
			for (int y = 0; y < 3; y++) {
				out_jacob_dryp_dpose[i][x * 3 + y] = 0;
			}
			X[i][x] = 0;
			U[i][x] = 0;
			cpose[i][x] = 0;
			out_X_oplus_U[i][x] = 0;
		}
		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				out_jacob_dryp_dpoint[i][x * 3 + y] = 0;
			}
			point[i][x] = 0;
		}
	}
}
void SW_EKF_SLAM::Run()
{
	for (int i = 0; i < dataSize; i++) {
		jacobiansPoseComposition(X[i], U[i], out_df_dx[i], out_X_oplus_U[i]);
		sphericalCoordinates(cpose[i], point[i], out_jacob_dryp_dpoint[i], out_jacob_dryp_dpose[i]);
	}
}
void SW_EKF_SLAM::Shutdown()
{
	delete [] X;
	delete [] U;
	delete [] out_df_dx;
	delete [] out_X_oplus_U;
	delete [] cpose;
	delete [] point;
	delete [] out_jacob_dryp_dpoint;
	delete [] out_jacob_dryp_dpose;

	while(true);
}
