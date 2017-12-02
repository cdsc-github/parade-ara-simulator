#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include "../Common/BaseCallbacks.hh"
#include <vector>

namespace LCAcc
{
class NetworkInterface
{
  int networkPort;
  int nodeID;
  std::vector<Arg3CallbackBase<int, const void*, unsigned int>*> handlers;
public:
  NetworkInterface(int node, int port);
  ~NetworkInterface();
  int GetNetworkPort() const;
  int GetNodeID() const;
  void SendMessage(int dest, const void* buffer, unsigned int size);
  void RegisterRecvHandler(Arg3CallbackBase<int, const void*, unsigned int>* handler);
  void RouteMsg(int src, const void*, unsigned int);
private:
  typedef Arg3MemberCallback<NetworkInterface, int, const void*, unsigned int, &NetworkInterface::RouteMsg> RouteMsgCB;
};
}

#endif
