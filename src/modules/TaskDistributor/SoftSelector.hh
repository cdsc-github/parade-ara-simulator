#ifndef SOFT_SELECTOR_H
#define SOFT_SELECTOR_H

#include "DumbSelector.hh"

class SoftSelector : public DumbSelector
{
  static std::string name;
  uint32_t lastComputeTime;
public:
  virtual const std::string& GetSelectorName();
  virtual uint32_t GetLastCalculationDelay();
  virtual bool FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignment, std::vector<int>& requiredModeSet, bool allowPartial);
};

#endif
