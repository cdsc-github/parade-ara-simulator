#include "SoftSelector.hh"

std::string SoftSelector::name = "Soft";
const std::string& SoftSelector::GetSelectorName()
{
  return name;
}
uint32_t SoftSelector::GetLastCalculationDelay()
{
  return lastComputeTime;
}
bool SoftSelector::FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignment, std::vector<int>& requiredModeSet, bool allowPartial)
{
  lastComputeTime = 50000;
  lastComputeTime += 10000 * prog.nodeSet.size();
  return DumbSelector::FindAssignment(core, prog, cfusPerIsland, fullSystem, available, assignment, requiredModeSet, allowPartial);
}
