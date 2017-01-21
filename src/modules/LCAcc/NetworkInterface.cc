#include "NetworkInterface.hh"
#include "SimicsInterface.hh"

using namespace LCAcc;

namespace LCAcc
{
NetworkInterface::NetworkInterface(int node, int port)
{
  nodeID = node;
  networkPort = port;
}
NetworkInterface::~NetworkInterface()
{
  for (size_t i = 0; i < handlers.size(); i++) {
    handlers[i]->Dispose();
  }
}
int NetworkInterface::GetNetworkPort() const
{
  return networkPort;
}
int NetworkInterface::GetNodeID() const
{
  return nodeID;
}
void NetworkInterface::SendMessage(int dest, const void* buffer, unsigned int size)
{
  SimicsInterface::SendMessage(GetNetworkPort(), dest, size, buffer);
}
void NetworkInterface::RegisterRecvHandler(Arg3CallbackBase<int, const void*, unsigned int>* handler)
{
  handlers.push_back(handler);
}
void NetworkInterface::RouteMsg(int src, const void* buffer, unsigned int size)
{
  for (size_t i = 0; i < handlers.size(); i++) {
    handlers[i]->Call(src, buffer, size);
  }
}
}
