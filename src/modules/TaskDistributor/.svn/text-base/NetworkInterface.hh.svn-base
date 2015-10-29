#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include "../Common/BaseCallbacks.hh"
#include <vector>
#include <stdint.h>

class NetworkInterface
{
	int networkPort;
	int nodeID;
	std::vector<Arg3CallbackBase<int, const void*, unsigned int>*> handlers;
public:
	NetworkInterface(int node, int port);
	int GetNetworkPort() const;
	int GetNodeID() const;
	void SendMessage(int dest, const void* buffer, unsigned int size);
	void SendMessage(int dest, const void* buffer, unsigned int size, int delay);
	void RegisterRecvHandler(Arg3CallbackBase<int, const void*, unsigned int>* handler);
	void RouteMsg(int src, const void*, unsigned int);
private:
	void SendMessageDelay(int dest, uint8_t* buffer, unsigned int size);
	typedef Arg3MemberCallback<NetworkInterface, int, const void*, unsigned int, &NetworkInterface::RouteMsg> RouteMsgCB;
	typedef MemberCallback3<NetworkInterface, int, uint8_t*, unsigned int, &NetworkInterface::SendMessageDelay> SendMessageDelayCB;
};

#endif
