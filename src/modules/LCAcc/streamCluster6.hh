#ifndef LCACC_MODE_STREAMCLUSTER6_H
#define LCACC_MODE_STREAMCLUSTER6_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
class OperatingMode_streamCluster6 : public LCAccOperatingMode
{
  uint64_t lowerAddr;
  uint64_t spmTarget;
public:
  inline OperatingMode_streamCluster6() {}
  inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(0 < argAddrVec.size());
    uint64_t addr_inIsCenter = argAddrVec[0];
    assert(1 < argAddrVec.size());
    uint64_t addr_table = argAddrVec[1];
    assert(2 < argAddrVec.size());
    uint64_t addr_lower = argAddrVec[2];

    if (argActive[0]) {
      outputArgs.push_back(addr_inIsCenter);
    }

    if (argActive[1]) {
      outputArgs.push_back(addr_table);
    }

    if (argActive[2]) {
      outputArgs.push_back(addr_lower);
    }
  }
  inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(3 < argAddrVec.size());
    uint64_t addr_outIsCenter = argAddrVec[3];

    if (argActive[3]) {
      outputArgs.push_back(addr_outIsCenter);
    }
  }
  inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
  {
    assert(LCACC_INTERNAL_argAddrVec.size() == 4);
    assert(0 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_inIsCenter = LCACC_INTERNAL_argAddrVec[0];
    assert(1 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_table = LCACC_INTERNAL_argAddrVec[1];
    assert(2 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_lower = LCACC_INTERNAL_argAddrVec[2];
    assert(3 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_outIsCenter = LCACC_INTERNAL_argAddrVec[3];

    uint64_t inIsCenter;
    uint64_t table;
    double lower;
    double outIsCenter;
    inIsCenter = 0;
    table = 0;
    lower = 0;
    outIsCenter = 0;

    inIsCenter = ReadSPMInt(0, addr_inIsCenter, 0, true);
    table = ReadSPMInt(1, addr_table, 0, true);
    lower = ReadSPMFlt(2, addr_lower, 0);

#define SPMAddressOf(x) (addr_##x)

    if (inIsCenter) {
      AddRead(lowerAddr + table * sizeof(float), spmTarget + iteration * sizeof(float), sizeof(float));
    }

#undef SPMAddressOf

    WriteSPMFlt(3, addr_outIsCenter, 0, outIsCenter);
  }
  inline virtual void MemoryAccessComplete(int iteration, int maxIteration, int taskID, uint64_t spmAddr, uint64_t memAddr, int accessType, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec)
  {
    assert(LCACC_INTERNAL_argAddrVec.size() == 4);
    uint64_t addr_inIsCenter = LCACC_INTERNAL_argAddrVec[0];
    uint64_t addr_table = LCACC_INTERNAL_argAddrVec[1];
    uint64_t addr_lower = LCACC_INTERNAL_argAddrVec[2];
    uint64_t addr_outIsCenter = LCACC_INTERNAL_argAddrVec[3];

    uint64_t inIsCenter;
    uint64_t table;
    double lower;
    double outIsCenter;

    inIsCenter = ReadSPMInt(0, addr_inIsCenter, 0, true);
    table = ReadSPMInt(1, addr_table, 0, true);
    lower = ReadSPMFlt(2, addr_lower, 0);

    if (lower > 0) {
      outIsCenter = false;
    }

    WriteSPMFlt(3, addr_outIsCenter, 0, outIsCenter);
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
    return 1;
  }
  inline virtual bool CallAllAtEnd()
  {
    return false;
  }
  inline static std::string GetModeName()
  {
    return "streamCluster6";
  }
  inline virtual int ArgumentCount()
  {
    return 4;
  }
  inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
  {
    assert(regs.size() == 2);
    lowerAddr = ConvertTypes<uint64_t, uint64_t>(regs[0]);
    spmTarget = ConvertTypes<uint64_t, uint64_t>(regs[1]);
  }
  inline static int GetOpCode()
  {
    return 1206;
  }
};
}

#endif
