#ifndef LCACC_MODE_DISPMAPINTEGSUM_H
#define LCACC_MODE_DISPMAPINTEGSUM_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
class OperatingMode_DispMapIntegSum : public LCAccOperatingMode
{
  float internal_sum;
public:
  inline OperatingMode_DispMapIntegSum() {}
  inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(0 < argAddrVec.size());
    uint64_t addr_in_val_array = argAddrVec[0];

    if (argActive[0]) {
      outputArgs.push_back(addr_in_val_array);
    }
  }
  inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(1 < argAddrVec.size());
    uint64_t addr_out_sum_array = argAddrVec[1];

    if (argActive[1]) {
      outputArgs.push_back(addr_out_sum_array);
    }
  }
  inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
  {
    assert(LCACC_INTERNAL_argAddrVec.size() == 2);
    assert(0 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_in_val_array = LCACC_INTERNAL_argAddrVec[0];
    assert(1 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_out_sum_array = LCACC_INTERNAL_argAddrVec[1];

    double in_val_array;
    double out_sum_array;
    in_val_array = 0;
    out_sum_array = 0;

    in_val_array = ReadSPMFlt(0, addr_in_val_array, 0);

#define SPMAddressOf(x) (addr_##x)
    out_sum_array = internal_sum + in_val_array;
    internal_sum = out_sum_array;
#undef SPMAddressOf

    WriteSPMFlt(1, addr_out_sum_array, 0, out_sum_array);
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
    return 10;
  }
  inline virtual bool CallAllAtEnd()
  {
    return false;
  }
  inline static std::string GetModeName()
  {
    return "DispMapIntegSum";
  }
  inline virtual int ArgumentCount()
  {
    return 2;
  }
  inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
  {
    assert(regs.size() == 1);
    internal_sum = ConvertTypes<uint64_t, float>(regs[0]);
  }
  inline static int GetOpCode()
  {
    return 831;
  }
};
}

#endif
