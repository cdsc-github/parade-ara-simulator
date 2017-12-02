#ifndef LCACC_MODE_LPCIP_H
#define LCACC_MODE_LPCIP_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
class OperatingMode_LPCIP : public LCAccOperatingMode
{
  int LPCIP_IMGH;
  int LPCIP_IMGW;
  int LPCIP_PATCH_H;
  int LPCIP_PATCH_W;
  int spmSampleTarget;
public:
  inline OperatingMode_LPCIP() {}
  inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(0 < argAddrVec.size());
    uint64_t addr_input_img_Start = argAddrVec[0];
    assert(1 < argAddrVec.size());
    uint64_t addr_input_center_x = argAddrVec[1];
    assert(2 < argAddrVec.size());
    uint64_t addr_input_center_y = argAddrVec[2];
    assert(3 < argAddrVec.size());
    uint64_t addr_input_rho = argAddrVec[3];

    if (argActive[0]) {
      outputArgs.push_back(addr_input_img_Start);
    }

    if (argActive[1]) {
      outputArgs.push_back(addr_input_center_x);
    }

    if (argActive[2]) {
      outputArgs.push_back(addr_input_center_y);
    }

    if (argActive[3]) {
      outputArgs.push_back(addr_input_rho);
    }
  }
  inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {

  }
  inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
  {
    assert(LCACC_INTERNAL_argAddrVec.size() == 4);
    assert(0 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_input_img_Start = LCACC_INTERNAL_argAddrVec[0];
    assert(1 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_input_center_x = LCACC_INTERNAL_argAddrVec[1];
    assert(2 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_input_center_y = LCACC_INTERNAL_argAddrVec[2];
    assert(3 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_input_rho = LCACC_INTERNAL_argAddrVec[3];

    uint64_t input_img_Start;
    double input_center_x;
    double input_center_y;
    double input_rho;
    input_img_Start = 0;
    input_center_x = 0;
    input_center_y = 0;
    input_rho = 0;

    input_img_Start = ReadSPMInt(0, addr_input_img_Start, 0, true);
    input_center_x = ReadSPMFlt(1, addr_input_center_x, 0);
    input_center_y = ReadSPMFlt(2, addr_input_center_y, 0);
    input_rho = ReadSPMFlt(3, addr_input_rho, 0);

#define SPMAddressOf(x) (addr_##x)
#define LPCIP_img(r,c)     ((input_img_Start) + ((r)*LPCIP_IMGW) + (c))
#define LPCIP_center_x     (input_center_x)
#define LPCIP_center_y     (input_center_y)
#define LPCIP_rho_scale    (input_rho)
#define CV_PI              3.1415926535897932384626433832795f

#ifndef opt_sin_f_f     // Functionality: sin x = x - (x^3)/6 + (x^5)/120 - (x^7)/5040
#define opt_sin_f_f(FLOAT_IN, FLOAT_OUT) { \
			    float x2, x3, x5, x7; \
			    x2 = FLOAT_IN * FLOAT_IN; \
			    x3 = x2 * FLOAT_IN; \
			    x5 = x3 * x2; \
			    x7 = x5 * x2; \
			    FLOAT_OUT = (FLOAT_IN - x3/6 + x5/120 - x7/5040); \
			}
#endif

#ifndef opt_cos_f_f     // Functionality: --> cos x = 1 - (x^2)/2 + (x^4)/24  - (x^6)/720
#define opt_cos_f_f(FLOAT_IN, FLOAT_OUT) { \
			    float x2, x4, x6; \
			    x2 = FLOAT_IN * FLOAT_IN; \
			    x4 = x2 * x2; \
			    x6 = x4 * x2; \
			    FLOAT_OUT = (1 - x2/2 + x4/24  - x6/720); \
			}
#endif

#ifndef opt_exp_f_f
#define opt_exp_f_f(FLOAT_IN, FLOAT_OUT) { \
			    if (FLOAT_IN <= -4.f) \
			        FLOAT_OUT = 0.f; \
			    else { \
			        float x1  = FLOAT_IN + 2.f; \
			        float x2 = x1 * x1; \
			        float x3 = x2 * x1; \
			        float x4 = x2 * x2; \
			        float x5 = x2 * x3; \
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
      trig_in = (float) (phi * 2 * CV_PI / LPCIP_PATCH_H);
      opt_cos_f_f(trig_in, cp);
      opt_sin_f_f(trig_in, sp);

      for (rho = 0; rho < LPCIP_PATCH_W; rho++) {
        // loop_tripcount min=10 max=10 avg=10
        float r = exp_tab[rho];
        float x = r * cp + LPCIP_center_x;
        float y = r * sp + LPCIP_center_y;
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
        else if (y_mapping >= LPCIP_IMGH) y_mapping = LPCIP_IMGH - 1;

        int x_mapping = mapx[h][w];

        if (x_mapping < 0) x_mapping = 0;
        else if (x_mapping >= LPCIP_IMGW) x_mapping = LPCIP_IMGW - 1;

        AddRead((uint64_t)LPCIP_img(y_mapping, x_mapping), spmSampleTarget + iteration * LPCIP_PATCH_W * LPCIP_PATCH_H + (h * LPCIP_PATCH_W + w), sizeof(uint8_t));
      }
    }

#undef LPCIP_img
#undef LPCIP_center_x
#undef LPCIP_center_y
#undef LPCIP_rho_scale
#undef CV_PI
#undef opt_sin_f_f
#undef opt_cos_f_f
#undef opt_exp_f_f
#undef SPMAddressOf

  }
  inline virtual void BeginComputation() {}
  inline virtual void EndComputation() {}
  inline virtual int CycleTime()
  {
    return 1;
  }
  inline virtual int InitiationInterval()
  {
    return 100;
  }
  inline virtual int PipelineDepth()
  {
    return 267;
  }
  inline virtual bool CallAllAtEnd()
  {
    return false;
  }
  inline static std::string GetModeName()
  {
    return "LPCIP";
  }
  inline virtual int ArgumentCount()
  {
    return 4;
  }
  inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
  {
    assert(regs.size() == 5);
    LPCIP_IMGH = ConvertTypes<uint64_t, int>(regs[0]);
    LPCIP_IMGW = ConvertTypes<uint64_t, int>(regs[1]);
    LPCIP_PATCH_H = ConvertTypes<uint64_t, int>(regs[2]);
    LPCIP_PATCH_W = ConvertTypes<uint64_t, int>(regs[3]);
    spmSampleTarget = ConvertTypes<uint64_t, int>(regs[4]);
  }
  inline static int GetOpCode()
  {
    return 806;
  }
};
}

#endif
