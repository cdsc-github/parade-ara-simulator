#ifndef LCACC_MODE_STREAMCLUSTER1_H
#define LCACC_MODE_STREAMCLUSTER1_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
class OperatingMode_streamCluster1 : public LCAccOperatingMode
{
  uint32_t counter;
public:
  inline OperatingMode_streamCluster1() {}
  inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(0 < argAddrVec.size());
    uint64_t addr_predicate = argAddrVec[0];

    if (argActive[0]) {
      outputArgs.push_back(addr_predicate);
    }
  }
  inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(1 < argAddrVec.size());
    uint64_t addr_count = argAddrVec[1];

    if (argActive[1]) {
      outputArgs.push_back(addr_count);
    }
  }
  inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
  {
    assert(LCACC_INTERNAL_argAddrVec.size() == 2);
    assert(0 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_predicate = LCACC_INTERNAL_argAddrVec[0];
    assert(1 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_count = LCACC_INTERNAL_argAddrVec[1];

    uint64_t predicate;
    uint64_t count;
    predicate = 0;
    count = 0;

    predicate = ReadSPMInt(0, addr_predicate, 0, true);

#define SPMAddressOf(x) (addr_##x)

    if (predicate)
      count = counter++;

#undef SPMAddressOf

    WriteSPMInt(1, addr_count, 0, true, count);
  }
  inline virtual void BeginComputation() {}
  inline virtual void EndComputation() {}
  inline virtual int CycleTime()
  {
    return 1;
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
    return "streamCluster1";
  }
  inline virtual int ArgumentCount()
  {
    return 2;
  }
  inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
  {
    assert(regs.size() == 1);
    counter = ConvertTypes<uint64_t, uint32_t>(regs[0]);
  }
  inline static int GetOpCode()
  {
    return 1201;
  }
};
}

#endif
