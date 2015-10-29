#ifndef GREEDY_MOHAMMAD_H
#define GREEDY_MOHAMMAD_H

#include <set>
#include <map>
#include <cassert>
#include <iostream>
#include "PatternSelector.hh"

class GreedySelector : public PatternSelector
{
	static const int NOF_CORE = 32;
	static const int NOF_CFU = 32;
	struct node_t
	{
		int func;
		bool visited;
		inline node_t()
		{
			func = -1;
			visited = false;
		}
	};
	class edge_t
	{
	public:
		node_t *src;
		node_t *des;
		inline edge_t(node_t *s, node_t *d) { src=s; des=d; };
	};
	class dfg_t
	{
	public:
		unsigned int core_id;
		std::set <node_t*> vertices;
		std::set <edge_t*> edges;
		void addVertix(node_t *n) {vertices.insert(n);};
		void addEdge(edge_t *e) {edges.insert(e);};
		inline dfg_t(std::set <node_t*> v, std::set <edge_t*> e, unsigned int c)
		{
			for (std::set<node_t*>::iterator n = v.begin(); n != v.end(); n++)
				addVertix(*n);
			for (std::set<edge_t*>::iterator n = e.begin(); n != e.end(); n++)
				addEdge(*n);
			core_id = c;
		}
		inline dfg_t()
		{
			core_id = 0;
		}
	};
	struct overlappedNodeInfo_t
	{
		node_t* node;
		dfg_t* oldP;
		dfg_t* newP;
		inline overlappedNodeInfo_t(node_t* n, dfg_t* s, dfg_t* d)
		{
			node = n; oldP = s; newP = d;
		}
	};
	class td_t
	{
	public:
		td_t();
		void assignNewAllocation(int core, int cfu);
		void clearAllocations();
		void allocateDFG(dfg_t *dfg, std::set<dfg_t*> &selectedPatterns);
		bool checkFeasibility(dfg_t *dfg);
	private:
		unsigned int allocTable[NOF_CORE][NOF_CFU];

		void selectPatterns(dfg_t *dfg, std::map< int, std::set<dfg_t*> > &patterns, std::set<dfg_t*> &selectedPatterns);
		void findEssentialPatterns(dfg_t* dfg, std::map<int, std::set<dfg_t*> > &patterns, std::set<dfg_t*> &selectedPatterns, std::map<node_t*, std::set<dfg_t*> > &coveringPatterns );
		node_t* checkPatternCoverage(dfg_t* dfg, std::set<dfg_t*> &selectedPatterns, std::map<node_t*, std::set<dfg_t*> > &coveringPatterns);
		int findCommunicationOverHead(dfg_t* dfg, dfg_t* pattern, std::set<dfg_t*> selectedPatterns, std::set<overlappedNodeInfo_t *> &overlappedNodesInfo);
		int findCommunicationOverHead(node_t* node, dfg_t* pattern);

		void generatePatterns(dfg_t *dfg, std::map<int, std::set<dfg_t*> > &patterns);
		void enumeratePatterns (dfg_t *dfg, int coreNo, std::set<dfg_t*> &patterns);
		void permuteCFUs(dfg_t *dfg, int coreNo, std::set<node_t*> &removeNodes);
		void cluster(std::set<dfg_t*> &patterns, std::set<node_t*> &nodes, std::set<edge_t*> &edges, int core_id);
		void recursiveCluster(dfg_t* d, std::set<node_t*> &nodes, std::set<edge_t*> &edges, node_t* n);
	};
	
	td_t td;
	std::string selectorName;
public:
	inline GreedySelector()
	{
		selectorName = "Greedy";
		td.clearAllocations();
	}
	virtual const std::string& GetSelectorName();
	virtual uint32_t GetLastCalculationDelay();
	virtual bool FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignment, std::vector<int>& requiredModeSet, bool allowPartial);
};

#endif
