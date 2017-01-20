#include <vector>
#include "GreedySelector.hh"

using namespace std;

GreedySelector::td_t::td_t()
{
	int i, j;
	for (i=0;i<GreedySelector::NOF_CORE;i++)
		for (j=0;j<GreedySelector::NOF_CFU;j++)
			allocTable[i][j]=0;
}

void GreedySelector::td_t::recursiveCluster(dfg_t* d, set<node_t*> &nodes, set<edge_t*> &edges, node_t* n)
{
	d->addVertix(n); nodes.erase(n);
	for (set<edge_t*>::iterator et=edges.begin(); et!=edges.end();et++)
	{
		if ((*et)->des == n || (*et)->src == n)
		{
			d->addEdge(*et);
			if (d->vertices.find((*et)->des)==d->vertices.end() || d->vertices.find((*et)->src)==d->vertices.end())
				recursiveCluster(d, nodes, edges, ((*et)->src == n)? (*et)->des : (*et)->src);
		}
	}
}


void GreedySelector::td_t::cluster(set<dfg_t*> &patterns, set<node_t*> &nodes, set<edge_t*> &edges, int core_id)
{
	while (!nodes.empty())
	{
		set<node_t*>::iterator n=nodes.begin();
		dfg_t*d = new dfg_t(); d->core_id = core_id;
		recursiveCluster(d, nodes, edges, *n);
		patterns.insert(d);
	}
}
void GreedySelector::td_t::permuteCFUs(dfg_t *dfg, int coreNo, set<node_t*> &removeNodes)
{
	set<node_t*>::iterator ni;
	map <int, set<node_t*> > neededCFU;
	// enumerate what is needed
	for (ni=dfg->vertices.begin();ni!=dfg->vertices.end();ni++)
		neededCFU[(*ni)->func].insert(*ni);

	map <int, set<node_t*> >::iterator it;
	for (it=neededCFU.begin();it!=neededCFU.end();it++)
	{
		// if available == 0 remove all
		if (allocTable[coreNo][(*it).first] == 0)
		{
			set<node_t*>::iterator jt;
			for (jt=(*it).second.begin();jt!=(*it).second.end();jt++)
				removeNodes.insert(*jt);
		}
		// if (available < needed) select the best subset in dfg
		else if ( allocTable[coreNo][(*it).first]  <  (*it).second.size() )
		{
			// criteria: select those which have the highest number of available neighbors
			map <node_t*, int> count;
			set<node_t*>::iterator jt;
			// for all the nodes of type (*it).first
			for (jt=(*it).second.begin();jt!=(*it).second.end();jt++)
			{
				// for all the edges if they match (*jt) and if their other end is available, inc count
				set<edge_t*>::iterator ei;
				if (count.find(*jt)==count.end()) count[*jt] = 0;
				if ((*jt)->visited == true) count[*jt] = (-1)*(dfg->vertices.size() + dfg->edges.size());
				for (ei=dfg->edges.begin();ei!=dfg->edges.end();ei++)
				{
					if ( ( (*ei)->des == (*jt) && (allocTable[coreNo][(*ei)->src->func]>0) ) || ( (*ei)->src == (*jt) && (allocTable[coreNo][(*ei)->des->func]>0) ) )
						count[*jt]++;
				}
			}
			// remove (needed - available)# of CFUs which has the lowest count value from the dfg
			for (unsigned int i=0;i<	(*it).second.size() - allocTable[coreNo][(*it).first] ; i++)					
			{
				// find the node with minimum count and remove it from "count" and add it to "removeNodes" set
				map <node_t*,int>::iterator ci; 
				int min=dfg->vertices.size(); node_t *min_cnt_node = NULL;
				for (ci=count.begin();ci!=count.end();ci++)
					if ((*ci).second <= min) {
						min_cnt_node = (*ci).first;
						min = (*ci).second;
					}
				removeNodes.insert(min_cnt_node);
				count.erase(min_cnt_node);
			}
		}
	}
}
void GreedySelector::td_t::enumeratePatterns (dfg_t *dfg, int coreNo, set<dfg_t*> &patterns)
{
	set<node_t*>removeNodes;

	permuteCFUs(dfg, coreNo, removeNodes);

	set<edge_t*>removeEdges;
	set<node_t*>::iterator ni;
	set<edge_t*>::iterator et;
	for (ni=removeNodes.begin(); ni!=removeNodes.end();ni++)
		for (et=dfg->edges.begin();et!=dfg->edges.end();et++)
			if ((*et)->des == (*ni) || (*et)->src == (*ni))
				removeEdges.insert(*et);;
	for (ni=removeNodes.begin();ni!=removeNodes.end();ni++)
		dfg->vertices.erase(*ni);
	for (et=removeEdges.begin();et!=removeEdges.end();et++)
		dfg->edges.erase(*et);

	for (ni=dfg->vertices.begin();ni!=dfg->vertices.end();ni++)
		(*ni)->visited = true;

	// cluster the remaining nodes
	cluster(patterns, dfg->vertices, dfg->edges, dfg->core_id);
}
void GreedySelector::td_t::generatePatterns(dfg_t *dfg, map<int, set<dfg_t*> > &patterns)
{
	int c;
	for (c=0;c<GreedySelector::NOF_CORE;c++)
	{
		dfg_t* newdfg = new dfg_t(dfg->vertices, dfg->edges, c);
		//traverseDFG(newdfg, c, patterns[c]);
		enumeratePatterns(newdfg, c, patterns[c]);
	}
}
void GreedySelector::td_t::findEssentialPatterns(dfg_t* dfg, map<int, set<dfg_t*> > &patterns, set<dfg_t*> &selectedPatterns, map<node_t*, set<dfg_t*> > &coveringPatterns )
{
	int c;
	set<node_t*>::iterator it;
	for (it=dfg->vertices.begin();it!=dfg->vertices.end();it++)
	{
		for (c=0;c<GreedySelector::NOF_CORE;c++)
		{
			set<dfg_t*>::iterator jt;
			for (jt=patterns[c].begin();jt!=patterns[c].end();jt++)
			{
				set<node_t*>::iterator kt;
				for (kt=(*jt)->vertices.begin();kt!=(*jt)->vertices.end();kt++)
				{
					if (*kt == *it)
						coveringPatterns[*it].insert((*jt));
				}
			}
		}
		assert (!coveringPatterns[*it].empty());
		if (coveringPatterns[*it].size() == 1)
			selectedPatterns.insert(*coveringPatterns[*it].begin());
	}

	// select the largest pattern if there is no essential
	dfg_t* maxSizeDFG = NULL; int maxSize = 0;
	if (selectedPatterns.size() == 0)
	{
		for (int c=0;c<GreedySelector::NOF_CORE;c++)
		{
			// select the largest pattern
			set<dfg_t*>::iterator it = patterns[c].begin();
			while (it!=patterns[c].end())
			{
				int size = (*it)->vertices.size();
				if (size > maxSize) {
					maxSizeDFG = (*it);
					maxSize = size;
				}
				it++;
			}
		}
		assert(maxSize>0);
		selectedPatterns.insert(maxSizeDFG);
	}

}

// check all the nodes and find which patterns cover them
// if there is any which is covered by more than one, it returns it, so the greedy approach selects one of them
GreedySelector::node_t *GreedySelector::td_t::checkPatternCoverage(dfg_t* dfg, set<dfg_t*> &selectedPatterns, map<node_t*, set<dfg_t*> > &coveringPatterns)
{
	set<node_t*>::iterator it;
	node_t *not_single_covered = NULL;

	int n;
	for (it=dfg->vertices.begin(), n=0;it!=dfg->vertices.end();it++, n++)
	{
		set<dfg_t*>::iterator jt;
		bool clearedOnce=false;
		for (jt=selectedPatterns.begin();jt!=selectedPatterns.end();jt++)
		{
			if ((*jt)->vertices.find(*it) != (*jt)->vertices.end())
			{
				if (!clearedOnce) {
					coveringPatterns[*it].clear();
					clearedOnce = true;
				}
				coveringPatterns[*it].insert(*jt);
			}
		}
		if (coveringPatterns[*it].size() != 1) not_single_covered = *it;
	}
	return not_single_covered;
}

int GreedySelector::td_t::findCommunicationOverHead(node_t* node, dfg_t* pattern)
{
	int commOvhd=0;
	// find the communication overhead between node and (pattern minus node)
	set<edge_t*>::iterator it;
	for (it=pattern->edges.begin();it!=pattern->edges.end();it++)
		if ((*it)->src == node || (*it)->des == node) commOvhd++;
	return commOvhd;
}


int GreedySelector::td_t::findCommunicationOverHead(dfg_t* dfg, dfg_t* pattern, set<dfg_t*> selectedPatterns, set<overlappedNodeInfo_t *> &overlappedNodesInfo)
{
	int commOvhd = 0;

	// check if there is any overlapped node and compute the case that creates the minimum commOvhd
	set<dfg_t*>::iterator it;
	set<node_t*>::iterator ni;
	for (ni=pattern->vertices.begin();ni!=pattern->vertices.end();ni++)
	{
		for (it=selectedPatterns.begin();it!=selectedPatterns.end();it++)
		{
			if (pattern==(*it)) continue;
			if ((*it)->vertices.find(*ni) != (*it)->vertices.end())
			{
				int c1 = findCommunicationOverHead(*ni, pattern);
				int c2 = findCommunicationOverHead(*ni, (*it));
				commOvhd += (c1>c2) ? c2 : c1;
				if (c1>c2)
					overlappedNodesInfo.insert(new overlappedNodeInfo_t(*ni, (*it), pattern) );
				else
					overlappedNodesInfo.insert(new overlappedNodeInfo_t(*ni, pattern, (*it)) );
			}
		}
	}

	// check all the edges in DFG and see if they connect pattern and selectedPattern
	for (it=selectedPatterns.begin();it!=selectedPatterns.end();it++)
	{
		set<edge_t*>::iterator jt;
		for (jt=dfg->edges.begin();jt!=dfg->edges.end();jt++)
		{
			bool isSrcNodeInPattern = pattern->vertices.find((*jt)->src) != pattern->vertices.end();
			bool isDesNodeInPattern = pattern->vertices.find((*jt)->des) != pattern->vertices.end();
			bool isSrcNodeInSelectedPattern = (*it)->vertices.find((*jt)->src) != (*it)->vertices.end();
			bool isDesNodeInSelectedPattern = (*it)->vertices.find((*jt)->des) != (*it)->vertices.end();

			if ( !(isSrcNodeInPattern && isSrcNodeInSelectedPattern) && !(isDesNodeInPattern && isDesNodeInSelectedPattern) ) // node is not overlapped
				if ((isSrcNodeInPattern && isDesNodeInSelectedPattern)
					|| (isDesNodeInPattern && isSrcNodeInSelectedPattern))  /* there is an edge between selectedPatterns and pattern*/
					commOvhd++;
		}
	}
	return commOvhd==0 ? -1 : commOvhd;
}


void GreedySelector::td_t::selectPatterns(dfg_t *dfg, map<int, set<dfg_t*> > &patterns, set<dfg_t*> &selectedPatterns)
{
	set<overlappedNodeInfo_t *> overlappedNodesInfo;
	set<overlappedNodeInfo_t *> temp_overlappedNodesInfo;
	map<node_t*, set<dfg_t*> > coveringPatterns;

	// find essential patterns
	findEssentialPatterns(dfg, patterns, selectedPatterns, coveringPatterns);

	node_t* not_single_covered = checkPatternCoverage(dfg, selectedPatterns, coveringPatterns);
	while (not_single_covered)
	{
		// select the largest patterns that has the minimum communication with the current selection
		set<dfg_t*>::iterator it = coveringPatterns[not_single_covered].begin();
		dfg_t* minCommPattern = *it;
		int minCommOvhd = findCommunicationOverHead(dfg, *it, selectedPatterns, overlappedNodesInfo); it++;
		while (it!=coveringPatterns[not_single_covered].end())
		{
			int commOvhd = findCommunicationOverHead(dfg, *it, selectedPatterns, temp_overlappedNodesInfo);
			if (commOvhd<minCommOvhd)
			{
				minCommPattern = *it;
				overlappedNodesInfo.clear();
				overlappedNodesInfo = temp_overlappedNodesInfo;
			}
			it++;
		}
		coveringPatterns[not_single_covered].clear();
		coveringPatterns[not_single_covered].insert(minCommPattern);
		selectedPatterns.insert(minCommPattern);

		// remove the overlapped nodes
		set<overlappedNodeInfo_t *>::iterator oi;
		for (oi=overlappedNodesInfo.begin();oi!=overlappedNodesInfo.end();oi++)	{
			(*oi)->oldP->vertices.erase((*oi)->node);
			set<edge_t*>::iterator ei_bak, ei = (*oi)->oldP->edges.begin();
			while (ei!=(*oi)->oldP->edges.end()) {
				if ((*ei)->src == (*oi)->node || (*ei)->des == (*oi)->node ) {
					ei_bak = ei; ei_bak++;
					(*oi)->oldP->edges.erase(*ei);
					ei = ei_bak;
				}
				else
					ei++;
			}
		}

		//check for coverage after the new selection
		not_single_covered = checkPatternCoverage(dfg, selectedPatterns, coveringPatterns);
	}
}

void GreedySelector::td_t::allocateDFG(dfg_t *dfg, set<dfg_t*> &selectedPatterns)
{
	std::map< int, std::set<dfg_t*> > patterns;
	generatePatterns(dfg, patterns);
	selectPatterns(dfg, patterns, selectedPatterns);
}

bool GreedySelector::td_t::checkFeasibility(dfg_t *dfg)
{
	int i, j;
	// compute the needed CFU of type i
	int sumNeeded[GreedySelector::NOF_CFU]; 
	for (i=0;i<GreedySelector::NOF_CFU;i++)
	{
		sumNeeded[i]=0;
	}
	set<node_t*>::iterator ni;
	for (ni=dfg->vertices.begin();ni!=dfg->vertices.end();ni++)
	{
		sumNeeded[(*ni)->func]++;
	}
	
	// compute the available CFU of type i
	int sumAvail[GreedySelector::NOF_CFU]; 
	for (i=0;i<GreedySelector::NOF_CFU;i++)
	{
		sumAvail[i]=0;
	}
	for (i=0;i<GreedySelector::NOF_CORE;i++)
	{
		for (j=0;j<GreedySelector::NOF_CFU;j++)
		{
			sumAvail[j] += allocTable[i][j];
		}
	}

	for (i=0;i<GreedySelector::NOF_CFU;i++)
	{
		if (sumNeeded[i] > sumAvail[i]) return false;
	}

	return true;
}
void GreedySelector::td_t::clearAllocations()
{
	for(int i = 0; i < GreedySelector::NOF_CFU; i++)
	{
		for(int j = 0; j < GreedySelector::NOF_CORE; j++)
		{
			allocTable[j][i] = 0;
		}
	}
}
void GreedySelector::td_t::assignNewAllocation(int core, int cfu)
{
	assert(core >= 0);
	assert(core < NOF_CORE);
	assert(cfu >= 0);
	assert(cfu < NOF_CFU);
	allocTable[core][cfu]++;
}

const std::string& GreedySelector::GetSelectorName()
{
	return selectorName;
}
uint32_t GreedySelector::GetLastCalculationDelay()
{
	return 1;
}
bool GreedySelector::FindAssignment(int core, const TDProgram& prog, int cfusPerIsland, const std::vector<CFUIdentifier>& fullSystem, const std::vector<CFUIdentifier>& available, std::vector<CFUIdentifier>& assignment, std::vector<int>& requiredModeSet, bool allowPartial)
{
	assert(!allowPartial);
//condense allocations first
	std::map<unsigned int, unsigned int> modeToID;
	std::map<unsigned int, unsigned int> idToMode;
	for(size_t i = 0; i < available.size(); i++)
	{
		if(modeToID.find(available[i].acceleratorID) == modeToID.end())
		{
			unsigned int id = modeToID.size();
			assert(idToMode.find(id) == idToMode.end());
			modeToID[available[i].acceleratorID] = id;
			idToMode[id] = available[i].acceleratorID;
		}
	}

	td.clearAllocations();
	for(size_t i = 0; i < available.size(); i++)
	{
		assert(modeToID.find(available[i].acceleratorID) != modeToID.end());
		td.assignNewAllocation(available[i].cpuID, modeToID[available[i].acceleratorID]);
	}
	std::vector<node_t> nodes;
	for(size_t i = 0; i < prog.nodeSet.size(); i++)
	{
		node_t n;
		if(modeToID.find(prog.nodeSet[i].computeDesc.opcode) == modeToID.end())
		{
			return false;
		}
		n.func = modeToID[prog.nodeSet[i].computeDesc.opcode];
		n.visited = false;
		nodes.push_back(n);
	}
	std::vector<edge_t> edges;
	for(size_t i = 0; i < prog.edgeSet.size(); i++)
	{
		if(prog.edgeSet[i].from == TransferDescription::MemoryDevice || prog.edgeSet[i].to == TransferDescription::MemoryDevice)
		{
			continue;
		}
		edge_t e(&nodes[prog.edgeSet[i].from], &nodes[prog.edgeSet[i].to]);
		edges.push_back(e);
	}
	std::set<node_t*> inputNodes;
	std::set<edge_t*> inputEdges;
	for(size_t i = 0; i < nodes.size(); i++)
	{
		inputNodes.insert(&nodes[i]);
	}
	for(size_t i = 0; i < edges.size(); i++)
	{
		inputEdges.insert(&edges[i]);
	}
	set<dfg_t*> selectedPatterns;
	dfg_t dfg(inputNodes, inputEdges, -1);
	if(!td.checkFeasibility(&dfg))
	{
		return false;
	}
	td.allocateDFG(&dfg, selectedPatterns);
	set<int> selectionMask;
	for(std::set<dfg_t*>::iterator dfgIt = selectedPatterns.begin(); dfgIt != selectedPatterns.end(); dfgIt++)
	{
		for(std::set<node_t*>::iterator nodeIt = (*dfgIt)->vertices.begin(); nodeIt != (*dfgIt)->vertices.end(); nodeIt++)
		{

			assert(idToMode.find((*nodeIt)->func) != idToMode.end());
			unsigned int func = idToMode[(*nodeIt)->func];
			bool assigned = false;
			for(size_t i = 0; i < available.size(); i++)
			{
				if(func == available[i].acceleratorID && (*dfgIt)->core_id == available[i].cpuID && selectionMask.find(available[i].cfuID) == selectionMask.end())
				{
					assigned = true;
					assignment.push_back(available[i]);
					selectionMask.insert(available[i].cfuID);
					break;
				}
			}
			assert(assigned);
		}
	}
	for(std::set<dfg_t*>::iterator dfgIt = selectedPatterns.begin(); dfgIt != selectedPatterns.end(); dfgIt++)
	{
		delete *dfgIt;
	}
	return true;
}

