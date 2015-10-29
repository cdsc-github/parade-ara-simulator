#include "ExclusiveDumbSelector.hh"
#include <set>

std::string ExclusiveDumbSelector::name = "ExclusiveDumb";
const std::string& ExclusiveDumbSelector::GetSelectorName()
{
        return name;
}
uint32_t ExclusiveDumbSelector::GetLastCalculationDelay()
{
        return 1;
}
bool ExclusiveDumbSelector::FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignment, std::vector<int>& requiredModeSet, bool allowPartial)
{
	assert(!allowPartial);
        std::set<int> selectedSet;
        for(size_t node = 0; node < prog.nodeSet.size(); node++)
        {
                bool found = false;
                for(size_t cfu = 0; cfu < available.size(); cfu++)
                {
                        if(selectedSet.find(available[cfu].cfuID) != selectedSet.end())
                        {
                                continue;
                        }
                        if(available[cfu].acceleratorID == prog.nodeSet[node].computeDesc.opcode && available[cfu].cpuID == (unsigned int)core)
                        {
                                assignment.push_back(available[cfu]);
                                selectedSet.insert(available[cfu].cfuID);
                                found = true;
                                break;
                        }
                }
                if(!found)
                {
                        return false;
                }
        }
        return true;
}

