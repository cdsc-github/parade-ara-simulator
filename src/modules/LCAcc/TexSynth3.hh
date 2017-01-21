#ifndef LCACC_MODE_TEXSYNTH3_H
#define LCACC_MODE_TEXSYNTH3_H

#include "LCAccOperatingMode.hh"
#include "SPMInterface.hh"
#include <vector>

namespace LCAcc
{
class OperatingMode_TexSynth3 : public LCAccOperatingMode
{
  int xBias;
  int yBias;
  intptr_t sourceImageAddr;
  uint32_t sourceImageWidth;
  uint32_t sourceImageHeight;
  uint32_t dstImageWidth;
  uint32_t dstImageHeight;
  int candidateXBias0;
  int candidateYBias0;
  int candidateXBias1;
  int candidateYBias1;
  int candidateXBias2;
  int candidateYBias2;
  int candidateXBias3;
  int candidateYBias3;
  int candidateXBias4;
  int candidateYBias4;
  int candidateXBias5;
  int candidateYBias5;
  int candidateXBias6;
  int candidateYBias6;
  int candidateXBias7;
  int candidateYBias7;
  int candidateXBias8;
  int candidateYBias8;
  uint32_t targetSPMLocation0;
  uint32_t targetSPMLocation1;
  uint32_t targetSPMLocation2;
  uint32_t targetSPMLocation3;
  uint32_t targetSPMLocation4;
  uint32_t targetSPMLocation5;
  uint32_t targetSPMLocation6;
  uint32_t targetSPMLocation7;
  uint32_t targetSPMLocation8;
public:
  inline OperatingMode_TexSynth3() {}
  inline virtual void GetSPMReadIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {
    assert(0 < argAddrVec.size());
    uint64_t addr_in_atlas = argAddrVec[0];

    if (argActive[0]) {
      for (size_t i = 0; i < 2 * GetArgumentWidth(0); i += GetArgumentWidth(0)) {
        outputArgs.push_back(addr_in_atlas + i);
      }
    }
  }
  inline virtual void GetSPMWriteIndexSet(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& argAddrVec, const std::vector<bool>& argActive, std::vector<uint64_t>& outputArgs)
  {

  }
  inline virtual void Compute(int iteration, int maxIteration, int taskID, const std::vector<uint64_t>& LCACC_INTERNAL_argAddrVec, const std::vector<bool>& LCACC_INTERNAL_argActive)
  {
    assert(LCACC_INTERNAL_argAddrVec.size() == 1);
    assert(0 < LCACC_INTERNAL_argAddrVec.size());
    uint64_t addr_in_atlas = LCACC_INTERNAL_argAddrVec[0];

    int64_t in_atlas[2];

    for (int i = 0; i < 2; i++) {
      in_atlas[(i) % (2)] = (int64_t)0;
    }

    for (size_t i = 0; i < 2; i++) {
      in_atlas[(i) % (2)] = ReadSPMInt(0, addr_in_atlas, i, true);
    }

#define SPMAddressOf(x) (addr_##x)
#define pixelCalc(val, bound) (((val) + (bound)) % (bound))
#define pixelX(index) pixelCalc(candidateX + candidateXBias##index, sourceImageWidth)
#define pixelY(index) pixelCalc(candidateY + candidateYBias##index, sourceImageHeight)
#define pixelRead(index) AddRead( \
					sourceImageAddr + imagePitch + elemSize * (pixelX(index) + pixelY(index) * sourceImageWidth), \
					targetSPMLocation##index + spmDstBias, \
					elemSize)

    int32_t candidateX = in_atlas[0] + xBias;
    int32_t candidateY = in_atlas[1] + yBias;
    size_t elemSize = sizeof(float);
    size_t spmDstBias = elemSize * iteration;
    size_t index = iteration + maxIteration * taskID;
    size_t imageIndex = index / (dstImageWidth * dstImageHeight);
    size_t imagePitch = dstImageWidth * dstImageHeight * sizeof(float) * imageIndex;

    pixelRead(0);
    pixelRead(1);
    pixelRead(2);
    pixelRead(3);
    pixelRead(4);
    pixelRead(5);
    pixelRead(6);
    pixelRead(7);
    pixelRead(8);

#undef pixelCalc
#undef pixelX
#undef pixelY
#undef pixelRead
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
    return 1;
  }
  inline virtual int PipelineDepth()
  {
    return 12;
  }
  inline virtual bool CallAllAtEnd()
  {
    return false;
  }
  inline static std::string GetModeName()
  {
    return "TexSynth3";
  }
  inline virtual int ArgumentCount()
  {
    return 1;
  }
  inline virtual void SetRegisterValues(const std::vector<uint64_t>& regs)
  {
    assert(regs.size() == 34);
    xBias = ConvertTypes<uint64_t, int>(regs[0]);
    yBias = ConvertTypes<uint64_t, int>(regs[1]);
    sourceImageAddr = ConvertTypes<uint64_t, intptr_t>(regs[2]);
    sourceImageWidth = ConvertTypes<uint64_t, uint32_t>(regs[3]);
    sourceImageHeight = ConvertTypes<uint64_t, uint32_t>(regs[4]);
    dstImageWidth = ConvertTypes<uint64_t, uint32_t>(regs[5]);
    dstImageHeight = ConvertTypes<uint64_t, uint32_t>(regs[6]);
    candidateXBias0 = ConvertTypes<uint64_t, int>(regs[7]);
    candidateYBias0 = ConvertTypes<uint64_t, int>(regs[8]);
    candidateXBias1 = ConvertTypes<uint64_t, int>(regs[9]);
    candidateYBias1 = ConvertTypes<uint64_t, int>(regs[10]);
    candidateXBias2 = ConvertTypes<uint64_t, int>(regs[11]);
    candidateYBias2 = ConvertTypes<uint64_t, int>(regs[12]);
    candidateXBias3 = ConvertTypes<uint64_t, int>(regs[13]);
    candidateYBias3 = ConvertTypes<uint64_t, int>(regs[14]);
    candidateXBias4 = ConvertTypes<uint64_t, int>(regs[15]);
    candidateYBias4 = ConvertTypes<uint64_t, int>(regs[16]);
    candidateXBias5 = ConvertTypes<uint64_t, int>(regs[17]);
    candidateYBias5 = ConvertTypes<uint64_t, int>(regs[18]);
    candidateXBias6 = ConvertTypes<uint64_t, int>(regs[19]);
    candidateYBias6 = ConvertTypes<uint64_t, int>(regs[20]);
    candidateXBias7 = ConvertTypes<uint64_t, int>(regs[21]);
    candidateYBias7 = ConvertTypes<uint64_t, int>(regs[22]);
    candidateXBias8 = ConvertTypes<uint64_t, int>(regs[23]);
    candidateYBias8 = ConvertTypes<uint64_t, int>(regs[24]);
    targetSPMLocation0 = ConvertTypes<uint64_t, uint32_t>(regs[25]);
    targetSPMLocation1 = ConvertTypes<uint64_t, uint32_t>(regs[26]);
    targetSPMLocation2 = ConvertTypes<uint64_t, uint32_t>(regs[27]);
    targetSPMLocation3 = ConvertTypes<uint64_t, uint32_t>(regs[28]);
    targetSPMLocation4 = ConvertTypes<uint64_t, uint32_t>(regs[29]);
    targetSPMLocation5 = ConvertTypes<uint64_t, uint32_t>(regs[30]);
    targetSPMLocation6 = ConvertTypes<uint64_t, uint32_t>(regs[31]);
    targetSPMLocation7 = ConvertTypes<uint64_t, uint32_t>(regs[32]);
    targetSPMLocation8 = ConvertTypes<uint64_t, uint32_t>(regs[33]);
  }
  inline static int GetOpCode()
  {
    return 912;
  }
};
}

#endif
