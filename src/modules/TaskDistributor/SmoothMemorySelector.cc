#include "SmoothMemorySelector.hh"
#include <map>
#include <vector>
#include <set>
#include <iostream>

std::string SmoothMemorySelector::name = "SmoothMemory";
void SmoothMemorySelector::NotifyDeallocate(int cfuID)
{
	assert(memoryAccessingCfus.find(cfuID) != memoryAccessingCfus.end() || nonMemoryAccessingCfus.find(cfuID) != nonMemoryAccessingCfus.end());
	assert(memoryAccessingCfus.find(cfuID) == memoryAccessingCfus.end() || nonMemoryAccessingCfus.find(cfuID) == nonMemoryAccessingCfus.end());
	if(memoryAccessingCfus.find(cfuID) != memoryAccessingCfus.end())
	{
		assert(memoryPressure.find(memoryAccessingCfus[cfuID]) != memoryPressure.end());
		assert(memoryPressure[memoryAccessingCfus[cfuID]] > 0);
		assert(cfuMemoryLoad.find(cfuID) != cfuMemoryLoad.end());
		memoryPressure[memoryAccessingCfus[cfuID]] -= cfuMemoryLoad[cfuID];
		memoryAccessingCfus.erase(cfuID);
		cfuMemoryLoad.erase(cfuID);
	}
	else
	{
		nonMemoryAccessingCfus.erase(cfuID);
	}
}
const std::string& SmoothMemorySelector::GetSelectorName()
{
	return name;
}
uint32_t SmoothMemorySelector::GetLastCalculationDelay()
{
	return 1;
}
bool SmoothMemorySelector::FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignment, std::vector<int>& requiredModeSet, bool allowPartial)
{
	requiredModeSet.clear();
	std::vector<int> allocatedPerIsland;//everything related to this variable is to implement a limit on CFUS per island
	bool placedAll = true;
	for(size_t i = 0; i < fullSystem.size(); i++)
	{
		while(allocatedPerIsland.size() <= fullSystem[i].cpuID)
		{
			allocatedPerIsland.push_back(0);
		}
		allocatedPerIsland[fullSystem[i].cpuID]++;
	}
	for(size_t i = 0; i < available.size(); i++)
	{
		if(memoryPressure.find(available[i].cpuID) == memoryPressure.end())
		{
			memoryPressure[available[i].cpuID] = 0;
		}
		assert(allocatedPerIsland.size() > available[i].cpuID);
		assert(allocatedPerIsland[available[i].cpuID] > 0);
		allocatedPerIsland[available[i].cpuID]--;
	}
	std::vector<size_t> memoryNodes;
	std::vector<int> memoryDensity;
	std::vector<size_t> nonMemoryNodes;
	std::vector<unsigned int> sortedNodes;
	for(int i = 0; sortedNodes.size() != memoryPressure.size(); i++)
	{//sort the cpu nodes by memory pressure
		for(std::map<int, int>::iterator it = memoryPressure.begin(); it != memoryPressure.end(); it++)
		{
			if(it->second == i)
			{
				sortedNodes.push_back(it->first);
			}
		}
	}
	for(size_t i = 0; i < prog.nodeSet.size(); i++)
	{//accumulate the memory nodes
		int density = 0;
		for(size_t j = 0; j < prog.nodeSet[i].incomingEdges.size(); j++)
		{
			if(prog.edgeSet[prog.nodeSet[i].incomingEdges[j]].from == -1)
			{
				density++;
			}
		}
		for(size_t j = 0; j < prog.nodeSet[i].outgoingEdges.size(); j++)
		{
			if(prog.edgeSet[prog.nodeSet[i].outgoingEdges[j]].to == -1)
			{
				density++;
			}
		}
		if(density > 0)
		{
			memoryNodes.push_back(i);
			memoryDensity.push_back(density);
		}
		else
		{
			nonMemoryNodes.push_back(i);
		}
	}
	//place the memory nodes
	std::map<int, int> mapping;
	std::set<int> usedSet;
	for(size_t i = 0; i < memoryNodes.size(); i++)
	{
		bool placed = false;
		for(size_t cpuIndex = 0; cpuIndex < sortedNodes.size() && !placed; cpuIndex++)
		{
			size_t cpuNode = (cpuIndex + i) % sortedNodes.size();
			for(size_t cfu = 0; cfu < available.size() && !placed; cfu++)
			{
				if(usedSet.find(available[cfu].cfuID) != usedSet.end())
				{
					continue;
				}
				if(available[cfu].cpuID != sortedNodes[cpuNode])
				{
					continue;
				}
				if(prog.nodeSet[memoryNodes[i]].computeDesc.opcode != available[cfu].acceleratorID)
				{
					continue;
				}
				if(allocatedPerIsland[available[cfu].cpuID] > cfusPerIsland)
				{
std::cout << "Allocations per island is exceeded : [" << available[cfu].cpuID << "] " << allocatedPerIsland[available[cfu].cpuID] << " > " << cfusPerIsland << std::endl;
					continue;
				}
				placed = true;
				usedSet.insert(available[cfu].cfuID);
				mapping[memoryNodes[i]] = cfu;
				allocatedPerIsland[available[cfu].cpuID]++;
			}
		}
		if(!placed)
		{
			placedAll = false;
			requiredModeSet.push_back(prog.nodeSet[memoryNodes[i]].computeDesc.opcode);
		}
	}
	//place the other nodes
	for(size_t i = 0; i < nonMemoryNodes.size(); i++)
	{
		bool placed = false;
		for(size_t cfu = 0; cfu < available.size() && !placed; cfu++)
		{
			if(usedSet.find(available[cfu].cfuID) != usedSet.end() || prog.nodeSet[nonMemoryNodes[i]].computeDesc.opcode != available[cfu].acceleratorID)
			{
				continue;
			}
			if(allocatedPerIsland[available[cfu].cpuID] > cfusPerIsland)
			{
				continue;
			}
			placed = true;
			usedSet.insert(available[cfu].cfuID);
			mapping[nonMemoryNodes[i]] = cfu;
			allocatedPerIsland[available[cfu].cpuID]++;
		}
		if(!placed)
		{
			placedAll = false;
			requiredModeSet.push_back(prog.nodeSet[nonMemoryNodes[i]].computeDesc.opcode);
		}
	}
	//package up the assignment
	if(!placedAll && !allowPartial)
	{
		return false;
	}
	for(size_t i = 0; i < prog.nodeSet.size(); i++)
	{
		assignment.push_back(CFUIdentifier());
	}
	for(size_t i = 0; i < memoryNodes.size(); i++)
	{
		if(mapping.find(memoryNodes[i]) != mapping.end())
		{
			assignment[memoryNodes[i]] = available[mapping[memoryNodes[i]]];
			memoryPressure[available[mapping[memoryNodes[i]]].cpuID] += memoryDensity[i];
			memoryAccessingCfus[available[mapping[memoryNodes[i]]].cfuID] = available[mapping[memoryNodes[i]]].cpuID;
			assert(cfuMemoryLoad.find(available[mapping[memoryNodes[i]]].cfuID) == cfuMemoryLoad.end());
			cfuMemoryLoad[available[mapping[memoryNodes[i]]].cfuID] = memoryDensity[i];
		}
		else
		{
			assignment[memoryNodes[i]] = CFUIdentifier();
		}
	}
	for(size_t i = 0; i < nonMemoryNodes.size(); i++)
	{
		if(mapping.find(nonMemoryNodes[i]) != mapping.end())
		{
			assignment[nonMemoryNodes[i]] = available[mapping[nonMemoryNodes[i]]];
			nonMemoryAccessingCfus[available[mapping[nonMemoryNodes[i]]].cfuID] = available[mapping[nonMemoryNodes[i]]].cpuID;
		}
		else
		{
			assignment[nonMemoryNodes[i]] = CFUIdentifier();
		}
	}
	return true;
}
