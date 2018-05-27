#include "DumbSelector.hh"
#include <set>

std::string DumbSelector::name = "Dumb";
const std::string& DumbSelector::GetSelectorName()
{
  return name;
}
uint32_t DumbSelector::GetLastCalculationDelay()
{
  return 1;
}
bool DumbSelector::FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignment, std::vector<int>& requiredModeSet, bool allowPartial)
{
  assert(!allowPartial);
  std::set<int> selectedSet;

  for (size_t node = 0; node < prog.nodeSet.size(); node++) {
    bool found = false;

    for (size_t cfu = 0; cfu < available.size(); cfu++) {
      if (selectedSet.find(available[cfu].cfuID) != selectedSet.end()) {
        continue;
      }

      if (available[cfu].acceleratorID == prog.nodeSet[node].computeDesc.opcode && available[cfu].cpuID == (unsigned int)core) {
        assignment.push_back(available[cfu]);
        selectedSet.insert(available[cfu].cfuID);
        found = true;
        break;
      }
    }

    if (!found) {
      for (size_t cfu = 0; cfu < available.size(); cfu++) {
        if (selectedSet.find(available[cfu].cfuID) != selectedSet.end()) {
          continue;
        }

        if (available[cfu].acceleratorID == prog.nodeSet[node].computeDesc.opcode) {
          assignment.push_back(available[cfu]);
          selectedSet.insert(available[cfu].cfuID);
          found = true;
          break;
        }
      }
    }

    if (!found) {
      return false;
    }
  }

  return true;
}
