#ifndef SIMICS_NETWORK_PORT_H
#define SIMICS_NETWORK_PORT_H

#include <string>
#include <vector>
#include <ostream>
#include <map>
#include <stack>
#include <set>
#include "../common/Consumer.hh"
#include "../network/Network.hh"
#include "../slicc_interface/NetworkMessage.hh"
#include "../slicc_interface/AbstractController.hh"
#include "gem5NetworkPortInterface.hh"

#define SIM_NET_PORTS
class AbstractController;
class gem5NetworkPort
{
#ifdef SIM_NET_PORTS
	class SNPMsg
	{
	public:
		int id;
		std::vector<char> msg;
		int src;
		int dst;
		int refCount;
		bool seen;
	};
	class SNPRecvHandler
	{
	public:
		void* args;
		void (*callback)(void*, int, int, const char*, int);
	};
	std::map<int, std::vector<int> > deviceToRecvHandlerMap;
	std::map<int, SNPRecvHandler> recvHandlerSet;
	std::vector<int> portHandlers;
	int port;
	std::string name;
	static int nextMsgID;
	static int nextRecvHandle;
	static std::map<int, SNPMsg*> msgSet;
	static std::stack<SNPMsg*> msgPool;
	static void EndOfLineMessageDeliver(void*);
public:
	gem5NetworkPort(int i);
	virtual ~gem5NetworkPort(){}
	virtual void print(std::ostream& outs) const;
	void printConfig(std::ostream& output) const { print(output); }
	void SendPortMessage(const char* buffer, int length, int destination);
	void SendDeviceMessage(int source, const char* buffer, int length, int destination);
	bool MayEmitMemoryRead(physical_address_t address, size_t size);
	bool MayEmitMemoryWrite(physical_address_t address, size_t size);
	void EmitMemoryRead(int targetCPU, int targetCache, physical_address_t address, size_t size);
	void EmitMemoryWrite(int targetCPU, int targetCache, physical_address_t address, size_t size);
	void RegisterMemoryHandler(void (*handler)(void* data, bool read, bool write, physical_address_t address, size_t size), void* data);
	void HandleMessage(int msgHandle);
	int RegisterPortMessageHandler(void (*handler)(void*, int, int, const char*, int), void* data);
	int RegisterDeviceMessageHandler(int device, void (*handler)(void*, int, int, const char*, int), void* data);
	void* UnregisterMessageHandler(int handle);
	static void IncrementRef(int handle);
	static void DecrementRef(int handle);
#endif
};
int Getgem5PortID(int deviceID);
gem5NetworkPort* Getgem5Port(int nodeID);
int gem5NetworkPortInterface_Getgem5PortCount();
void gem5NetworkPortInterface_BindDeviceToPort(int portID, int deviceID);
void gem5NetworkPortInterface_UnbindDevice(int deviceID);
void gem5NetworkPortInterface_SendMessageOnPort(int source, int target, const void* buffer, int length);
void gem5NetworkPortInterface_SendMessageOnDevice(int deviceID, int destID, const void* buffer, int length);
int gem5NetworkPortInterface_RegisterRecvHandlerOnPort(int portID, void (*handler)(void*, int, int, const char*, int), void* data);
int gem5NetworkPortInterface_RegisterRecvHandlerOnDevice(int deviceID, void (*handler)(void*, int, int, const char*, int), void* data);
void* gem5NetworkPortInterface_UnregisterRecvHandler(int handle);
extern "C"
{
        void Initializegem5NetworkPortInterface(gem5NetworkPortInterface* snpi);
}

#endif
