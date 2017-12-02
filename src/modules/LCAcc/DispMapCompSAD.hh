#ifndef LCACC_MODE_DISPMAPCOMPSAD_H
#define LCACC_MODE_DISPMAPCOMPSAD_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
class OperatingMode_DispMapCompSAD : public LCAccOperatingMode
{
public:
  inline OperatingMode_DispMapCompSAD() {}
  inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(0 < argAddrVec.size());
    uint64_t addr_in_ILeft = argAddrVec[0];
    assert(1 < argAddrVec.size());
    uint64_t addr_in_IRight_mov = argAddrVec[1];

    if (argActive[0]) {
      outputArgs.push_back(addr_in_ILeft);
    }

    if (argActive[1]) {
      outputArgs.push_back(addr_in_IRight_mov);
    }
  }
  inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(2 < argAddrVec.size());
    uint64_t addr_out_SAD = argAddrVec[2];

    if (argActive[2]) {
      outputArgs.push_back(addr_out_SAD);
    }
  }
  inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
  {
    assert(LCACC_INTERNAL_argAddrVec.size() == 3);
    assert(0 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_in_ILeft = LCACC_INTERNAL_argAddrVec[0];
    assert(1 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_in_IRight_mov = LCACC_INTERNAL_argAddrVec[1];
    assert(2 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_out_SAD = LCACC_INTERNAL_argAddrVec[2];

    double in_ILeft;
    double in_IRight_mov;
    double out_SAD;
    in_ILeft = 0;
    in_IRight_mov = 0;
    out_SAD = 0;

    in_ILeft = ReadSPMFlt(0, addr_in_ILeft, 0);
    in_IRight_mov = ReadSPMFlt(1, addr_in_IRight_mov, 0);

#define SPMAddressOf(x) (addr_##x)
    out_SAD = in_ILeft * in_ILeft - 2 * in_ILeft * in_IRight_mov + in_IRight_mov * in_IRight_mov;
#undef SPMAddressOf

    WriteSPMFlt(2, addr_out_SAD, 0, out_SAD);
  }
  inline virtual void BeginComputation() {}
  inline virtual void EndComputation() {}
  inline virtual int CycleTime()
  {
    return 1;
  }
  inline virtual int InitiationInterval()
  {
    return 1;
  }
  inline virtual int PipelineDepth()
  {
    return 33;
  }
  inline virtual bool CallAllAtEnd()
  {
    return false;
  }
  inline static std::string GetModeName()
  {
    return "DispMapCompSAD";
  }
  inline virtual int ArgumentCount()
  {
    return 3;
  }
  inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
  {
    assert(regs.size() == 0);
  }
  inline static int GetOpCode()
  {
    return 830;
  }
};
}

#endif
