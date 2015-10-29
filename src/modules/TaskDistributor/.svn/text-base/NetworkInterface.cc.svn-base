#include <iostream>
#include <cstring>
#include "NetworkInterface.hh"
#include "gem5Interface.hh"

NetworkInterface::NetworkInterface(int node, int port)
{
	nodeID = node;
	networkPort = port;
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
	TaskDistributor::gem5Interface::SendMessage(GetNetworkPort(), dest, size, buffer);
}
void NetworkInterface::SendMessage(int dest, const void* buffer, unsigned int size, int delay)
{
	uint8_t* b = new uint8_t[size];
	memcpy(b, buffer, size);
	TaskDistributor::gem5Interface::RegisterCallback(SendMessageDelayCB::Create(this, dest, b, size), delay);
}
void NetworkInterface::RegisterRecvHandler(Arg3CallbackBase<int, const void*, unsigned int>* handler)
{
	handlers.push_back(handler);
}
void NetworkInterface::RouteMsg(int src, const void* buffer, unsigned int size)
{
	for(size_t i = 0; i < handlers.size(); i++)
	{
		handlers[i]->Call(src, buffer, size);
	}
}
void NetworkInterface::SendMessageDelay(int dest, uint8_t* buffer, unsigned int size)
{
	SendMessage(dest, buffer, size);
	delete [] buffer;
}
