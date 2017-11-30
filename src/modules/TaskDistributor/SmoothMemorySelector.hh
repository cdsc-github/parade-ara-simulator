#ifndef SMOOTH_MEMORY_SELECTOR_H
#define SMOOTH_MEMORY_SELECTOR_H

#include <map>
#include "PatternSelector.hh"

class SmoothMemorySelector : public PatternSelector
{
  static std::string name;
  std::map<int, int> cfuMemoryLoad;//key cfuID, value memory load contributed by this cfu
  std::map<int, int> memoryAccessingCfus; //key cfuID, value nodeID
  std::map<int, int> nonMemoryAccessingCfus; //key cfuID, value nodeID
  std::map<int, int> memoryPressure; // key nodeID, int memory pressure;
public:
  virtual void NotifyDeallocate(int cfuID);
  virtual const std::string& GetSelectorName();
  virtual uint32_t GetLastCalculationDelay();
  virtual bool FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignmen, std::vector<int>& requiredModeSet, bool allowPartial);
};

#endif
