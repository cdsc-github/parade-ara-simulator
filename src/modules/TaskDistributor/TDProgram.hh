#ifndef TD_PROGRAM_H
#define TD_PROGRAM_H

#include <vector>
#include <map>
#include <set>
#include <stdint.h>
#include "../Common/Packetizer.hh"
#include "../Common/TransferDescription.hh"
#include "../Common/ComputeDescription.hh"

class TDProgram
{
public:
  class Node;
  class Edge;
  class Node
  {
  public:
    int nodeID;
    ComputeDescription computeDesc;
    std::vector<int> outgoingEdges;
    std::vector<int> incomingEdges;
  };
  class Edge
  {
  public:
    int from;
    int to;
    TransferDescription transferDesc;
  };
  std::vector<Node> nodeSet;
  std::vector<Edge> edgeSet;
  uint32_t taskCount;
  uint32_t taskGrain;
  void ReadIn(PacketReader& pr)
  {
    int computeCount = pr.Read<uint16_t>();
    int transferCount = pr.Read<uint16_t>();
    taskGrain = pr.Read<uint32_t>();
    taskCount = pr.Read<uint32_t>();

    for (int i = 0; i < computeCount; i++) {
      Node n;
      n.computeDesc.ReadIn(pr);
      n.nodeID = i;
      nodeSet.push_back(n);
    }

    for (int i = 0; i < transferCount; i++) {
      Edge e;
      e.transferDesc.ReadIn(pr);
      e.from = e.transferDesc.srcDevice;
      e.to = e.transferDesc.dstDevice;

      if (e.from >= 0) {
        nodeSet[e.from].outgoingEdges.push_back(i);
      }

      if (e.to >= 0) {
        nodeSet[e.to].incomingEdges.push_back(i);
      }

      edgeSet.push_back(e);
    }
  }
  void WriteOut(PacketBuilder& br) const
  {
    br.Write((uint16_t)(nodeSet.size()));
    br.Write((uint16_t)(edgeSet.size()));
    br.Write((uint32_t)taskGrain);
    br.Write((uint32_t)taskCount);

    for (size_t i = 0; i < nodeSet.size(); i++) {
      nodeSet[i].computeDesc.WriteOut(br);
    }

    for (size_t i = 0; i < edgeSet.size(); i++) {
      edgeSet[i].transferDesc.WriteOut(br);
    }
  }
};

#endif
