#ifndef MIN_CHAIN_SELECTOR_H
#define MIN_CHAIN_SELECTOR_H

#include <string>
#include "PatternSelector.hh"

class MinChainSelector : public PatternSelector
{
  class Node
  {
  public:
    unsigned int memoryLoad;
    unsigned int opcode;
    std::vector<int> connectivity;
    bool assigned;
    size_t accIndex;
    Node(unsigned int op);
    void Assign(int acc);
    bool IsValidAssignment() const;
  };
  int CalculateChoiceValue(int internalConnectivity, int externalConnectivity, int unknownConnectivity, int nodeMemoryLoad, int sysMemoryLoad, int cfusFreeInNode);
  void ConstructGraph(const TDProgram& prog, std::vector<Node>& graph);
  void CalculateSystemMemoryLoad(const std::vector<CFUIdentifier>& fullSystem, std::vector<int>& memoryLoad);
  size_t SelectNodeToPlace(const std::vector<Node>& graph);
  size_t SelectLcaMapping(int x, const std::vector<Node>& graph, const std::vector<CFUIdentifier>& available, const std::vector<bool>& takenFilter, const std::vector<int>& memoryLoad, int cfusPerIsland);
  static std::string selectorName;
  std::map<int, int> allocatedMemoryLoad;//key cfuID, value memory load
public:
  virtual void NotifyDeallocate(int cfuID);
  virtual const std::string& GetSelectorName();
  virtual uint32_t GetLastCalculationDelay();
  virtual bool FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignmen, std::vector<int>& requiredModeSet, bool allowPartial);
};

#endif
