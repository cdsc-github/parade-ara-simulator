#include <vector>
#include <set>
#include <iostream>
#include <climits>
#include "MinChainSelector.hh"

#define INVALID_CHOICE 0x80000
MinChainSelector::Node::Node(unsigned int op)
{
	assigned = false;
	memoryLoad = 0;
	accIndex = INVALID_CHOICE;
	opcode = op;
}
void MinChainSelector::Node::Assign(int acc)
{
	assert(!assigned);
	assert(accIndex == INVALID_CHOICE);
	assigned = true;
	accIndex = acc;
}
bool MinChainSelector::Node::IsValidAssignment() const
{
	return assigned && (accIndex != INVALID_CHOICE);
}
int MinChainSelector::CalculateChoiceValue(int internalConnectivity, int externalConnectivity, int unknownConnectivity, int nodeMemoryLoad, int sysMemoryLoad, int cfusFreeInNode)
{//edit this function to implement different selection methodology
	return cfusFreeInNode + (internalConnectivity - externalConnectivity) * 10 - ((nodeMemoryLoad > 0) ? sysMemoryLoad : 0);
}
void MinChainSelector::ConstructGraph(const TDProgram& prog, std::vector<Node>& graph)
{
	graph.clear();
	for(size_t x = 0; x < prog.nodeSet.size(); x++)
	{
		Node n(prog.nodeSet[x].computeDesc.opcode);
		for(size_t y = 0; y < prog.nodeSet[x].outgoingEdges.size(); y++)
		{
			assert(prog.edgeSet[prog.nodeSet[x].outgoingEdges[y]].from == (int)x);
			if(prog.edgeSet[prog.nodeSet[x].outgoingEdges[y]].to == -1)
			{
				n.memoryLoad++;
			}
			else
			{
				n.connectivity.push_back(prog.edgeSet[prog.nodeSet[x].outgoingEdges[y]].to);
			}
		}
		for(size_t y = 0; y < prog.nodeSet[x].incomingEdges.size(); y++)
		{
			assert(prog.edgeSet[prog.nodeSet[x].incomingEdges[y]].to == (int)x);
			if(prog.edgeSet[prog.nodeSet[x].incomingEdges[y]].from == -1)
			{
				n.memoryLoad++;
			}
			else
			{
				n.connectivity.push_back(prog.edgeSet[prog.nodeSet[x].incomingEdges[y]].from);
			}
		}
		graph.push_back(n);
	}
}
void MinChainSelector::CalculateSystemMemoryLoad(const std::vector<CFUIdentifier>& fullSystem, std::vector<int>& memoryLoad)
{
	memoryLoad.clear();
	for(size_t x = 0; x < fullSystem.size(); x++)
	{
		while(fullSystem[x].cpuID >= memoryLoad.size())
		{
			memoryLoad.push_back(0);
		}
		if(allocatedMemoryLoad.find(fullSystem[x].cfuID) != allocatedMemoryLoad.end())
		{
			memoryLoad[fullSystem[x].cpuID] += allocatedMemoryLoad[fullSystem[x].cfuID];
		}
	}
}
size_t MinChainSelector::SelectNodeToPlace(const std::vector<Node>& graph)
{
	for(size_t i = 0; i < graph.size(); i++)
	{
		if(!graph[i].assigned)
		{
			return i;
		}
	}
	assert(0);
	return 0;
}
size_t MinChainSelector::SelectLcaMapping(int x, const std::vector<Node>& graph, const std::vector<CFUIdentifier>& available, const std::vector<bool>& takenFilter, const std::vector<int>& memoryLoad, int cfusPerIsland)
{
	//allocate & set defaults
	std::vector<int> nodeQuality;
	std::vector<bool> nodePossible;
	std::vector<int> accFree;
	std::vector<int> adjacentAccs;
	std::set<size_t> alreadyChosenNodes;
	assert(x >= 0);
	assert((unsigned int)x < graph.size());
	for(size_t i = 0; i < graph.size(); i++)
	{
		if(graph[i].IsValidAssignment())
		{
			assert(alreadyChosenNodes.find(graph[i].accIndex) == alreadyChosenNodes.end());
			alreadyChosenNodes.insert(graph[i].accIndex);
		}
	}
	for(size_t i = 0; i < available.size(); i++)
	{
		while(available[i].cpuID >= nodeQuality.size())
		{
			nodeQuality.push_back(INT_MIN);
			nodePossible.push_back(false);
			accFree.push_back(0);
			adjacentAccs.push_back(0);
		}
		if(alreadyChosenNodes.find(i) != alreadyChosenNodes.end())
		{
			continue;
		}
		if(available[i].acceleratorID == graph[x].opcode)
		{
			nodePossible[available[i].cpuID] = true;
		}
		accFree[available[i].cpuID]++;
	}
	//Gather connectivity info
	for(size_t i = 0; i < graph[x].connectivity.size(); i++)
	{
		assert(graph[x].connectivity[i] >= 0 && (unsigned int)(graph[x].connectivity[i]) < graph.size());
		if(graph[graph[x].connectivity[i]].IsValidAssignment())
		{
			assert(graph[graph[x].connectivity[i]].accIndex >= 0 && graph[graph[x].connectivity[i]].accIndex < available.size());
			assert(available[graph[graph[x].connectivity[i]].accIndex].cpuID >= 0 && available[graph[graph[x].connectivity[i]].accIndex].cpuID < adjacentAccs.size());
			adjacentAccs[available[graph[graph[x].connectivity[i]].accIndex].cpuID]++;
		}
	}
	//calculate qualities
	for(size_t i = 0; i < nodeQuality.size(); i++)
	{
		if(!nodePossible[i])
		{
			continue;
		}
		int localAccs = adjacentAccs[i];
		int remoteAccs = 0;
		for(size_t y = 0; y < adjacentAccs.size(); y++)
		{
			if(i != y)
			{
				remoteAccs += adjacentAccs[y];
			}
		}
		nodeQuality[i] = CalculateChoiceValue(localAccs, remoteAccs, graph[x].connectivity.size() - (localAccs + remoteAccs), graph[x].memoryLoad, memoryLoad[i], accFree[i]);
	}
	//select max quality
	int maxQuality = INT_MIN;
	size_t selectedNode = INVALID_CHOICE;
	for(size_t i = 0; i < nodeQuality.size(); i++)
	{
		if(nodePossible[i] && nodeQuality[i] > maxQuality)
		{
			selectedNode = i;
			maxQuality = nodeQuality[i];
		}
	}
	if(selectedNode == INVALID_CHOICE)
	{
		return INVALID_CHOICE;
	}
	size_t chosenAccelerator = INVALID_CHOICE;
	for(size_t i = 0; i < available.size() && chosenAccelerator == INVALID_CHOICE; i++)
	{
		if(available[i].cpuID == selectedNode && available[i].acceleratorID == graph[x].opcode && alreadyChosenNodes.find(i) == alreadyChosenNodes.end())
		{
			chosenAccelerator = i;
		}
	}
	assert(chosenAccelerator != INVALID_CHOICE);
	return chosenAccelerator;
}
std::string MinChainSelector::selectorName = "MinChain";
void MinChainSelector::NotifyDeallocate(int cfuID)
{
	assert(allocatedMemoryLoad.find(cfuID) != allocatedMemoryLoad.end());
	allocatedMemoryLoad.erase(cfuID);
}
const std::string& MinChainSelector::GetSelectorName()
{
	return selectorName;
}
uint32_t MinChainSelector::GetLastCalculationDelay()
{
	return 1;
}
bool MinChainSelector::FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignment, std::vector<int>& missingModeSet, bool allowPartial)
{
	missingModeSet.clear();
	std::vector<Node> graph;
	std::vector<int> systemMemoryLoad;
	std::vector<bool> accTaken;
	for(size_t i = 0; i < available.size(); i++)
	{
		accTaken.push_back(false);
	}
	ConstructGraph(prog, graph);
	CalculateSystemMemoryLoad(fullSystem, systemMemoryLoad);
	size_t placed = 0;
	size_t placedInFpga = 0;
	while(placed < graph.size())
	{
		size_t x = SelectNodeToPlace(graph);
		assert(graph.size() > x);
		assert(!graph[x].assigned);
		size_t acc = SelectLcaMapping(x, graph, available, accTaken, systemMemoryLoad, cfusPerIsland);
		if(acc == INVALID_CHOICE)
		{
			placedInFpga++;
			missingModeSet.push_back(graph[x].opcode);
		}
		else
		{
			assert(available.size() > acc);
			assert(available[acc].acceleratorID == graph[x].opcode);
			assert(accTaken.size() > acc);
			assert(!accTaken[acc]);
			accTaken[acc] = true;
		}
		graph[x].Assign(acc);
		placed++;
	}
	assert(placedInFpga == 0 || !missingModeSet.empty());
	if(placedInFpga == 0 || allowPartial)
	{
		assignment.clear();
		for(size_t i = 0; i < graph.size(); i++)
		{
			assert(allocatedMemoryLoad.find(available[graph[i].accIndex].cfuID) == allocatedMemoryLoad.end());
			if(graph[i].IsValidAssignment())
			{
				assert(available.size() > graph[i].accIndex);
				assert(graph[i].accIndex >= 0);
				assignment.push_back(available[graph[i].accIndex]);
				allocatedMemoryLoad[available[graph[i].accIndex].cfuID] = graph[i].memoryLoad;
			}
			else
			{
				assignment.push_back(CFUIdentifier());
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}
