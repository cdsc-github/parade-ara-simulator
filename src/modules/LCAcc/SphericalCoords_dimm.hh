#ifndef LCACC_MODE_SPHERICALCOORDS_DIMM_H
#define LCACC_MODE_SPHERICALCOORDS_DIMM_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
	class OperatingMode_SphericalCoords_dimm : public LCAccOperatingMode
	{
	public:
		inline OperatingMode_SphericalCoords_dimm(){}
		inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(0 < argAddrVec.size());
			uint64_t addr_input_cpose_1 = argAddrVec[0];
			assert(1 < argAddrVec.size());
			uint64_t addr_input_cpose_2 = argAddrVec[1];
			assert(2 < argAddrVec.size());
			uint64_t addr_input_cpose_3 = argAddrVec[2];
			assert(3 < argAddrVec.size());
			uint64_t addr_input_point = argAddrVec[3];

			if(argActive[0])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(0); i += GetArgumentWidth(0))
				{
					outputArgs.push_back(addr_input_cpose_1 + i);
				}
			}
			if(argActive[1])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(1); i += GetArgumentWidth(1))
				{
					outputArgs.push_back(addr_input_cpose_2 + i);
				}
			}
			if(argActive[2])
			{
				outputArgs.push_back(addr_input_cpose_3);
			}
			if(argActive[3])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(3); i += GetArgumentWidth(3))
				{
					outputArgs.push_back(addr_input_point + i);
				}
			}
		}
		inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
		{
			assert(4 < argAddrVec.size());
			uint64_t addr_output_dryp_dpoint_1 = argAddrVec[4];
			assert(5 < argAddrVec.size());
			uint64_t addr_output_dryp_dpoint_2 = argAddrVec[5];
			assert(6 < argAddrVec.size());
			uint64_t addr_output_dryp_dpoint_3 = argAddrVec[6];
			assert(7 < argAddrVec.size());
			uint64_t addr_output_dryp_dpose_1 = argAddrVec[7];
			assert(8 < argAddrVec.size());
			uint64_t addr_output_dryp_dpose_2 = argAddrVec[8];
			assert(9 < argAddrVec.size());
			uint64_t addr_output_dryp_dpose_3 = argAddrVec[9];
			assert(10 < argAddrVec.size());
			uint64_t addr_output_dryp_dpose_4 = argAddrVec[10];
			assert(11 < argAddrVec.size());
			uint64_t addr_output_dryp_dpose_5 = argAddrVec[11];
			assert(12 < argAddrVec.size());
			uint64_t addr_output_dryp_dpose_6 = argAddrVec[12];
			assert(13 < argAddrVec.size());
			uint64_t addr_output_dryp_dpose_7 = argAddrVec[13];

			if(argActive[4])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(4); i += GetArgumentWidth(4))
				{
					outputArgs.push_back(addr_output_dryp_dpoint_1 + i);
				}
			}
			if(argActive[5])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(5); i += GetArgumentWidth(5))
				{
					outputArgs.push_back(addr_output_dryp_dpoint_2 + i);
				}
			}
			if(argActive[6])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(6); i += GetArgumentWidth(6))
				{
					outputArgs.push_back(addr_output_dryp_dpoint_3 + i);
				}
			}
			if(argActive[7])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(7); i += GetArgumentWidth(7))
				{
					outputArgs.push_back(addr_output_dryp_dpose_1 + i);
				}
			}
			if(argActive[8])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(8); i += GetArgumentWidth(8))
				{
					outputArgs.push_back(addr_output_dryp_dpose_2 + i);
				}
			}
			if(argActive[9])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(9); i += GetArgumentWidth(9))
				{
					outputArgs.push_back(addr_output_dryp_dpose_3 + i);
				}
			}
			if(argActive[10])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(10); i += GetArgumentWidth(10))
				{
					outputArgs.push_back(addr_output_dryp_dpose_4 + i);
				}
			}
			if(argActive[11])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(11); i += GetArgumentWidth(11))
				{
					outputArgs.push_back(addr_output_dryp_dpose_5 + i);
				}
			}
			if(argActive[12])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(12); i += GetArgumentWidth(12))
				{
					outputArgs.push_back(addr_output_dryp_dpose_6 + i);
				}
			}
			if(argActive[13])
			{
				for(size_t i = 0; i < ((3)) * GetArgumentWidth(13); i += GetArgumentWidth(13))
				{
					outputArgs.push_back(addr_output_dryp_dpose_7 + i);
				}
			}
		}
		inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
		{
			assert(LCACC_INTERNAL_argAddrVec.size() == 14);
			assert(0 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_input_cpose_1 = LCACC_INTERNAL_argAddrVec[0];
			assert(1 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_input_cpose_2 = LCACC_INTERNAL_argAddrVec[1];
			assert(2 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_input_cpose_3 = LCACC_INTERNAL_argAddrVec[2];
			assert(3 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_input_point = LCACC_INTERNAL_argAddrVec[3];
			assert(4 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output_dryp_dpoint_1 = LCACC_INTERNAL_argAddrVec[4];
			assert(5 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output_dryp_dpoint_2 = LCACC_INTERNAL_argAddrVec[5];
			assert(6 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output_dryp_dpoint_3 = LCACC_INTERNAL_argAddrVec[6];
			assert(7 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output_dryp_dpose_1 = LCACC_INTERNAL_argAddrVec[7];
			assert(8 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output_dryp_dpose_2 = LCACC_INTERNAL_argAddrVec[8];
			assert(9 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output_dryp_dpose_3 = LCACC_INTERNAL_argAddrVec[9];
			assert(10 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output_dryp_dpose_4 = LCACC_INTERNAL_argAddrVec[10];
			assert(11 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output_dryp_dpose_5 = LCACC_INTERNAL_argAddrVec[11];
			assert(12 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output_dryp_dpose_6 = LCACC_INTERNAL_argAddrVec[12];
			assert(13 < LCACC_INTERNAL_argAddrVec.size());
			uint64_t addr_output_dryp_dpose_7 = LCACC_INTERNAL_argAddrVec[13];

			double input_cpose_1[3];
			double input_cpose_2[3];
			double input_cpose_3;
			double input_point[3];
			double output_dryp_dpoint_1[3];
			double output_dryp_dpoint_2[3];
			double output_dryp_dpoint_3[3];
			double output_dryp_dpose_1[3];
			double output_dryp_dpose_2[3];
			double output_dryp_dpose_3[3];
			double output_dryp_dpose_4[3];
			double output_dryp_dpose_5[3];
			double output_dryp_dpose_6[3];
			double output_dryp_dpose_7[3];
			for(int i = 0; i < ((3)); i++)
			{
				input_cpose_1[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < ((3)); i++)
			{
				input_cpose_2[(i) % (3)] = (double)0;
			}
			input_cpose_3 = 0;
			for(int i = 0; i < ((3)); i++)
			{
				input_point[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < ((3)); i++)
			{
				output_dryp_dpoint_1[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < ((3)); i++)
			{
				output_dryp_dpoint_2[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < ((3)); i++)
			{
				output_dryp_dpoint_3[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < ((3)); i++)
			{
				output_dryp_dpose_1[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < ((3)); i++)
			{
				output_dryp_dpose_2[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < ((3)); i++)
			{
				output_dryp_dpose_3[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < ((3)); i++)
			{
				output_dryp_dpose_4[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < ((3)); i++)
			{
				output_dryp_dpose_5[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < ((3)); i++)
			{
				output_dryp_dpose_6[(i) % (3)] = (double)0;
			}
			for(int i = 0; i < ((3)); i++)
			{
				output_dryp_dpose_7[(i) % (3)] = (double)0;
			}

			for(size_t i = 0; i < ((3)); i++)
			{
				input_cpose_1[(i) % (3)] = ReadSPMFlt(0, addr_input_cpose_1, i);
			}
			for(size_t i = 0; i < ((3)); i++)
			{
				input_cpose_2[(i) % (3)] = ReadSPMFlt(1, addr_input_cpose_2, i);
			}
			input_cpose_3 = ReadSPMFlt(2, addr_input_cpose_3, 0);
			for(size_t i = 0; i < ((3)); i++)
			{
				input_point[(i) % (3)] = ReadSPMFlt(3, addr_input_point, i);
			}

			#define SPMAddressOf(x) (addr_##x)
			#define EKF_in_cpose input_cpose
			#define EKF_in_point input_point
			#define EKF_out_jacob_dryp_dpoint(r,c) output_dryp_dpoint[r*3+c]
			#define EKF_out_jacob_dryp_dpose(r,c)  output_dryp_dpose[r*7+c]

			float input_cpose[7] = {input_cpose_1[0], input_cpose_1[2], input_cpose_1[3], input_cpose_1[0], input_cpose_2[2], input_cpose_2[3], input_cpose_3};
			float output_dryp_dpoint[9];
			float output_dryp_dpose[21];

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
			    FLOAT_OUT = FLOAT_OUT * (1.5f - xhalf*FLOAT_OUT*FLOAT_OUT); \
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
			{local.x*_r,                            local.y*_r,                                     local.z*_r              },
			{-local.y/(x2*(y2/x2 + 1)),     1/(local.x*(y2/x2+1)),  0                               },
			{local.x*local.z*_K,            local.y*local.z*_K,                     -(out_range2)/t2} };

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

			#undef EKF_in_cpose
			#undef EKF_in_point
			#undef EKF_out_jacob_dryp_dpoint
			#undef EKF_out_jacob_dryp_dpose
			#undef opt_invSqrt_f_f

			for(int i = 0; i < 3; i++)
			{
				output_dryp_dpoint_1[i] = output_dryp_dpoint[i];
				output_dryp_dpoint_2[i] = output_dryp_dpoint[3 + i];
				output_dryp_dpoint_3[i] = output_dryp_dpoint[6 + i];
				output_dryp_dpose_1[i] = output_dryp_dpose[i];
				output_dryp_dpose_2[i] = output_dryp_dpose[3 + i];
				output_dryp_dpose_3[i] = output_dryp_dpose[6 + i];
				output_dryp_dpose_4[i] = output_dryp_dpose[9 + i];
				output_dryp_dpose_5[i] = output_dryp_dpose[12 + i];
				output_dryp_dpose_6[i] = output_dryp_dpose[15 + i];
				output_dryp_dpose_7[i] = output_dryp_dpose[18 + i];
			}
			#undef SPMAddressOf

			for(size_t i = 0; i < ((3)); i++)
			{
				WriteSPMFlt(4, addr_output_dryp_dpoint_1, i, output_dryp_dpoint_1[(i) % (3)]);
			}
			for(size_t i = 0; i < ((3)); i++)
			{
				WriteSPMFlt(5, addr_output_dryp_dpoint_2, i, output_dryp_dpoint_2[(i) % (3)]);
			}
			for(size_t i = 0; i < ((3)); i++)
			{
				WriteSPMFlt(6, addr_output_dryp_dpoint_3, i, output_dryp_dpoint_3[(i) % (3)]);
			}
			for(size_t i = 0; i < ((3)); i++)
			{
				WriteSPMFlt(7, addr_output_dryp_dpose_1, i, output_dryp_dpose_1[(i) % (3)]);
			}
			for(size_t i = 0; i < ((3)); i++)
			{
				WriteSPMFlt(8, addr_output_dryp_dpose_2, i, output_dryp_dpose_2[(i) % (3)]);
			}
			for(size_t i = 0; i < ((3)); i++)
			{
				WriteSPMFlt(9, addr_output_dryp_dpose_3, i, output_dryp_dpose_3[(i) % (3)]);
			}
			for(size_t i = 0; i < ((3)); i++)
			{
				WriteSPMFlt(10, addr_output_dryp_dpose_4, i, output_dryp_dpose_4[(i) % (3)]);
			}
			for(size_t i = 0; i < ((3)); i++)
			{
				WriteSPMFlt(11, addr_output_dryp_dpose_5, i, output_dryp_dpose_5[(i) % (3)]);
			}
			for(size_t i = 0; i < ((3)); i++)
			{
				WriteSPMFlt(12, addr_output_dryp_dpose_6, i, output_dryp_dpose_6[(i) % (3)]);
			}
			for(size_t i = 0; i < ((3)); i++)
			{
				WriteSPMFlt(13, addr_output_dryp_dpose_7, i, output_dryp_dpose_7[(i) % (3)]);
			}
		}
		inline virtual void BeginComputation(){}
		inline virtual void EndComputation(){}
		inline virtual int CycleTime(){return 1;}
		inline virtual int InitiationInterval(){return 1;}
		inline virtual int PipelineDepth(){return 203;}
		inline virtual bool CallAllAtEnd(){return false;}
		inline static std::string GetModeName(){return "SphericalCoords_dimm";}
		inline virtual int ArgumentCount(){return 14;}
		inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
		{
			assert(regs.size() == 0);
		}
		inline static int GetOpCode(){return 552;}
	};
}

#endif
