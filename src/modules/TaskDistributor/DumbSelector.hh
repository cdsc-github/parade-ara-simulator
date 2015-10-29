#ifndef DUMB_SELECTOR_H
#define DUMB_SELECTOR_H

#include "PatternSelector.hh"

class DumbSelector : public PatternSelector
{
	static std::string name;
public:
        virtual const std::string& GetSelectorName();
        virtual uint32_t GetLastCalculationDelay();
        virtual bool FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignment, std::vector<int>& requiredModeSet, bool allowPartial);
};

#endif
