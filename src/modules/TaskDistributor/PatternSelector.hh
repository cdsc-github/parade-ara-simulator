#ifndef PATTERN_SELECTOR_H
#define PATTERN_SELECTOR_H

#include <vector>
#include <string>
#include "TDProgram.hh"
#include "CFUIdentifier.hh"

class PatternSelector
{
public:
  virtual ~PatternSelector() {}
  virtual void NotifyDeallocate(int cfuID) {}
  virtual const std::string& GetSelectorName() = 0;
  virtual uint32_t GetLastCalculationDelay() = 0;
  virtual bool FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignment, std::vector<int>& missingModeSet, bool allowPartial) = 0;
};

#endif
