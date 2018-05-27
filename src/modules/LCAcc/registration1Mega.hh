#ifndef LCACC_MODE_REGISTRATION1MEGA_H
#define LCACC_MODE_REGISTRATION1MEGA_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
class OperatingMode_registration1Mega : public LCAccOperatingMode
{
  int width;
  int height;
  int depth;
  float dt;
public:
  inline OperatingMode_registration1Mega() {}
  inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(0 < argAddrVec.size());
    uint64_t addr_v1 = argAddrVec[0];
    assert(1 < argAddrVec.size());
    uint64_t addr_v2 = argAddrVec[1];
    assert(2 < argAddrVec.size());
    uint64_t addr_v3 = argAddrVec[2];
    assert(6 < argAddrVec.size());
    uint64_t addr_u1_Center = argAddrVec[6];
    assert(7 < argAddrVec.size());
    uint64_t addr_u2_Center = argAddrVec[7];
    assert(8 < argAddrVec.size());
    uint64_t addr_u3_Center = argAddrVec[8];
    uint64_t addr_u1_Right = addr_u1_Center + 1 * GetArgumentWidth(6);
    uint64_t addr_u1_Left = addr_u1_Center - 1 * GetArgumentWidth(6);
    uint64_t addr_u1_Down = addr_u1_Center + width * GetArgumentWidth(6);
    uint64_t addr_u1_Up = addr_u1_Center - width * GetArgumentWidth(6);
    uint64_t addr_u1_In = addr_u1_Center + width * height * GetArgumentWidth(6);
    uint64_t addr_u1_Out = addr_u1_Center - width * height * GetArgumentWidth(6);
    uint64_t addr_u2_Right = addr_u2_Center + 1 * GetArgumentWidth(7);
    uint64_t addr_u2_Left = addr_u2_Center - 1 * GetArgumentWidth(7);
    uint64_t addr_u2_Down = addr_u2_Center + width * GetArgumentWidth(7);
    uint64_t addr_u2_Up = addr_u2_Center - width * GetArgumentWidth(7);
    uint64_t addr_u2_In = addr_u2_Center + width * height * GetArgumentWidth(7);
    uint64_t addr_u2_Out = addr_u2_Center - width * height * GetArgumentWidth(7);
    uint64_t addr_u3_Right = addr_u3_Center + 1 * GetArgumentWidth(8);
    uint64_t addr_u3_Left = addr_u3_Center - 1 * GetArgumentWidth(8);
    uint64_t addr_u3_Down = addr_u3_Center + width * GetArgumentWidth(8);
    uint64_t addr_u3_Up = addr_u3_Center - width * GetArgumentWidth(8);
    uint64_t addr_u3_In = addr_u3_Center + width * height * GetArgumentWidth(8);
    uint64_t addr_u3_Out = addr_u3_Center - width * height * GetArgumentWidth(8);

    if (argActive[0]) {
      outputArgs.push_back(addr_v1);
    }

    if (argActive[1]) {
      outputArgs.push_back(addr_v2);
    }

    if (argActive[2]) {
      outputArgs.push_back(addr_v3);
    }

    if (argActive[6]) {
      outputArgs.push_back(addr_u1_Center);
    }

    if (argActive[6]) {
      outputArgs.push_back(addr_u1_Right);
    }

    if (argActive[6]) {
      outputArgs.push_back(addr_u1_Left);
    }

    if (argActive[6]) {
      outputArgs.push_back(addr_u1_Down);
    }

    if (argActive[6]) {
      outputArgs.push_back(addr_u1_Up);
    }

    if (argActive[6]) {
      outputArgs.push_back(addr_u1_In);
    }

    if (argActive[6]) {
      outputArgs.push_back(addr_u1_Out);
    }

    if (argActive[7]) {
      outputArgs.push_back(addr_u2_Center);
    }

    if (argActive[7]) {
      outputArgs.push_back(addr_u2_Right);
    }

    if (argActive[7]) {
      outputArgs.push_back(addr_u2_Left);
    }

    if (argActive[7]) {
      outputArgs.push_back(addr_u2_Down);
    }

    if (argActive[7]) {
      outputArgs.push_back(addr_u2_Up);
    }

    if (argActive[7]) {
      outputArgs.push_back(addr_u2_In);
    }

    if (argActive[7]) {
      outputArgs.push_back(addr_u2_Out);
    }

    if (argActive[8]) {
      outputArgs.push_back(addr_u3_Center);
    }

    if (argActive[8]) {
      outputArgs.push_back(addr_u3_Right);
    }

    if (argActive[8]) {
      outputArgs.push_back(addr_u3_Left);
    }

    if (argActive[8]) {
      outputArgs.push_back(addr_u3_Down);
    }

    if (argActive[8]) {
      outputArgs.push_back(addr_u3_Up);
    }

    if (argActive[8]) {
      outputArgs.push_back(addr_u3_In);
    }

    if (argActive[8]) {
      outputArgs.push_back(addr_u3_Out);
    }
  }
  inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(3 < argAddrVec.size());
    uint64_t addr_u1_result = argAddrVec[3];
    assert(4 < argAddrVec.size());
    uint64_t addr_u2_result = argAddrVec[4];
    assert(5 < argAddrVec.size());
    uint64_t addr_u3_result = argAddrVec[5];

    if (argActive[3]) {
      outputArgs.push_back(addr_u1_result);
    }

    if (argActive[4]) {
      outputArgs.push_back(addr_u2_result);
    }

    if (argActive[5]) {
      outputArgs.push_back(addr_u3_result);
    }
  }
  inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
  {
    assert(LCACC_INTERNAL_argAddrVec.size() == 9);
    assert(0 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_v1 = LCACC_INTERNAL_argAddrVec[0];
    assert(1 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_v2 = LCACC_INTERNAL_argAddrVec[1];
    assert(2 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_v3 = LCACC_INTERNAL_argAddrVec[2];
    assert(3 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_u1_result = LCACC_INTERNAL_argAddrVec[3];
    assert(4 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_u2_result = LCACC_INTERNAL_argAddrVec[4];
    assert(5 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_u3_result = LCACC_INTERNAL_argAddrVec[5];
    assert(6 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_u1_Center = LCACC_INTERNAL_argAddrVec[6];
    assert(7 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_u2_Center = LCACC_INTERNAL_argAddrVec[7];
    assert(8 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_u3_Center = LCACC_INTERNAL_argAddrVec[8];
    uint64_t addr_u1_Right = addr_u1_Center + 1 * GetArgumentWidth(6);
    uint64_t addr_u1_Left = addr_u1_Center - 1 * GetArgumentWidth(6);
    uint64_t addr_u1_Down = addr_u1_Center + width * GetArgumentWidth(6);
    uint64_t addr_u1_Up = addr_u1_Center - width * GetArgumentWidth(6);
    uint64_t addr_u1_In = addr_u1_Center + width * height * GetArgumentWidth(6);
    uint64_t addr_u1_Out = addr_u1_Center - width * height * GetArgumentWidth(6);
    uint64_t addr_u2_Right = addr_u2_Center + 1 * GetArgumentWidth(7);
    uint64_t addr_u2_Left = addr_u2_Center - 1 * GetArgumentWidth(7);
    uint64_t addr_u2_Down = addr_u2_Center + width * GetArgumentWidth(7);
    uint64_t addr_u2_Up = addr_u2_Center - width * GetArgumentWidth(7);
    uint64_t addr_u2_In = addr_u2_Center + width * height * GetArgumentWidth(7);
    uint64_t addr_u2_Out = addr_u2_Center - width * height * GetArgumentWidth(7);
    uint64_t addr_u3_Right = addr_u3_Center + 1 * GetArgumentWidth(8);
    uint64_t addr_u3_Left = addr_u3_Center - 1 * GetArgumentWidth(8);
    uint64_t addr_u3_Down = addr_u3_Center + width * GetArgumentWidth(8);
    uint64_t addr_u3_Up = addr_u3_Center - width * GetArgumentWidth(8);
    uint64_t addr_u3_In = addr_u3_Center + width * height * GetArgumentWidth(8);
    uint64_t addr_u3_Out = addr_u3_Center - width * height * GetArgumentWidth(8);

    double v1;
    double v2;
    double v3;
    double u1_result;
    double u2_result;
    double u3_result;
    double u1_Center;
    double u1_Right;
    double u1_Left;
    double u1_Down;
    double u1_Up;
    double u1_In;
    double u1_Out;
    double u2_Center;
    double u2_Right;
    double u2_Left;
    double u2_Down;
    double u2_Up;
    double u2_In;
    double u2_Out;
    double u3_Center;
    double u3_Right;
    double u3_Left;
    double u3_Down;
    double u3_Up;
    double u3_In;
    double u3_Out;
    v1 = 0;
    v2 = 0;
    v3 = 0;
    u1_result = 0;
    u2_result = 0;
    u3_result = 0;
    u1_Center = 0;
    u1_Right = 0;
    u1_Left = 0;
    u1_Down = 0;
    u1_Up = 0;
    u1_In = 0;
    u1_Out = 0;
    u2_Center = 0;
    u2_Right = 0;
    u2_Left = 0;
    u2_Down = 0;
    u2_Up = 0;
    u2_In = 0;
    u2_Out = 0;
    u3_Center = 0;
    u3_Right = 0;
    u3_Left = 0;
    u3_Down = 0;
    u3_Up = 0;
    u3_In = 0;
    u3_Out = 0;

    v1 = ReadSPMFlt(0, addr_v1, 0);
    v2 = ReadSPMFlt(1, addr_v2, 0);
    v3 = ReadSPMFlt(2, addr_v3, 0);
    u1_Center = ReadSPMFlt(6, addr_u1_Center, 0);
    u1_Right = ReadSPMFlt(6, addr_u1_Right, 0);
    u1_Left = ReadSPMFlt(6, addr_u1_Left, 0);
    u1_Down = ReadSPMFlt(6, addr_u1_Down, 0);
    u1_Up = ReadSPMFlt(6, addr_u1_Up, 0);
    u1_In = ReadSPMFlt(6, addr_u1_In, 0);
    u1_Out = ReadSPMFlt(6, addr_u1_Out, 0);
    u2_Center = ReadSPMFlt(7, addr_u2_Center, 0);
    u2_Right = ReadSPMFlt(7, addr_u2_Right, 0);
    u2_Left = ReadSPMFlt(7, addr_u2_Left, 0);
    u2_Down = ReadSPMFlt(7, addr_u2_Down, 0);
    u2_Up = ReadSPMFlt(7, addr_u2_Up, 0);
    u2_In = ReadSPMFlt(7, addr_u2_In, 0);
    u2_Out = ReadSPMFlt(7, addr_u2_Out, 0);
    u3_Center = ReadSPMFlt(8, addr_u3_Center, 0);
    u3_Right = ReadSPMFlt(8, addr_u3_Right, 0);
    u3_Left = ReadSPMFlt(8, addr_u3_Left, 0);
    u3_Down = ReadSPMFlt(8, addr_u3_Down, 0);
    u3_Up = ReadSPMFlt(8, addr_u3_Up, 0);
    u3_In = ReadSPMFlt(8, addr_u3_In, 0);
    u3_Out = ReadSPMFlt(8, addr_u3_Out, 0);

#define SPMAddressOf(x) (addr_##x)
    float du1_dx = (u1_Right - u1_Left) * 0.5f;
    float du2_dx = (u2_Right - u2_Left) * 0.5f;
    float du3_dx = (u3_Right - u3_Left) * 0.5f;

    float du1_dy = (u1_Up - u1_Down) * 0.5f;
    float du2_dy = (u2_Up - u2_Down) * 0.5f;
    float du3_dy = (u3_Up - u3_Down) * 0.5f;

    float du1_dz = (u1_In - u1_Out) * 0.5f;
    float du2_dz = (u1_In - u1_Out) * 0.5f;
    float du3_dz = (u1_In - u1_Out) * 0.5f;

    float r1 = v1 - v1 * du1_dx - v2 * du1_dy - v3 * du1_dz;
    float r2 = v2 - v1 * du2_dx - v2 * du2_dy - v3 * du2_dz;
    float r3 = v3 - v1 * du3_dx - v2 * du3_dy - v3 * du3_dz;

    u1_result = r1 * dt;
    u2_result = r2 * dt;
    u3_result = r3 * dt;
#undef SPMAddressOf

    WriteSPMFlt(3, addr_u1_result, 0, u1_result);
    WriteSPMFlt(4, addr_u2_result, 0, u2_result);
    WriteSPMFlt(5, addr_u3_result, 0, u3_result);
  }
  inline virtual void BeginComputation() {}
  inline virtual void EndComputation() {}
  inline virtual int CycleTime()
  {
    return 2;
  }
  inline virtual int InitiationInterval()
  {
    return 2;
  }
  inline virtual int PipelineDepth()
  {
    return 1;
  }
  inline virtual bool CallAllAtEnd()
  {
    return false;
  }
  inline static std::string GetModeName()
  {
    return "registration1Mega";
  }
  inline virtual int ArgumentCount()
  {
    return 9;
  }
  inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
  {
    assert(regs.size() == 4);
    width = ConvertTypes<uint64_t, int>(regs[0]);
    height = ConvertTypes<uint64_t, int>(regs[1]);
    depth = ConvertTypes<uint64_t, int>(regs[2]);
    dt = ConvertTypes<uint64_t, float>(regs[3]);
  }
  inline static int GetOpCode()
  {
    return 401;
  }
};
}

#endif
