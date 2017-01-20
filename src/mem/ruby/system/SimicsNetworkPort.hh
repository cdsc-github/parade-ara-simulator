//BEGIN edit by Mike, 12/22/10
#ifndef SIMICS_NETWORK_PORT_H
#define SIMICS_NETWORK_PORT_H

#include <string>
#include <vector>
#include <ostream>
#include <map>
#include <stack>
#include <set>
#include "../common/Consumer.hh"
//#include "eventq.hh"
#include "../network/Network.hh"
#include "../slicc_interface/NetworkMessage.hh"
#include "../slicc_interface/AbstractController.hh"
#include "SimicsNetworkPortInterface.hh"

#define SIM_NET_PORTS
//class AbstractChip;
class AbstractController;
//class conf_object;
//typedef conf_object conf_object_t;
class SimicsNetworkPort
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
        //const AbstractChip* chipPtr;
	//const AbstractController* m_controlPtr;
        //const Network* net;
	int port;
	std::string name;
	static int nextMsgID;
	static int nextRecvHandle;
	static std::map<int, SNPMsg*> msgSet;
	static std::stack<SNPMsg*> msgPool;
	static void EndOfLineMessageDeliver(void*);
public:
        //SimicsNetworkPort(const AbstractChip* c, int i);
	SimicsNetworkPort(int i);
	virtual ~SimicsNetworkPort(){}
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
int GetSimicsPortID(int deviceID);
SimicsNetworkPort* GetSimicsPort(int nodeID);
int SimicsNetworkPortInterface_GetSimicsPortCount();
void SimicsNetworkPortInterface_BindDeviceToPort(int portID, int deviceID);
void SimicsNetworkPortInterface_UnbindDevice(int deviceID);
void SimicsNetworkPortInterface_SendMessageOnPort(int source, int target, const void* buffer, int length);
void SimicsNetworkPortInterface_SendMessageOnDevice(int deviceID, int destID, const void* buffer, int length);
int SimicsNetworkPortInterface_RegisterRecvHandlerOnPort(int portID, void (*handler)(void*, int, int, const char*, int), void* data);
int SimicsNetworkPortInterface_RegisterRecvHandlerOnDevice(int deviceID, void (*handler)(void*, int, int, const char*, int), void* data);
void* SimicsNetworkPortInterface_UnregisterRecvHandler(int handle);
extern "C"
{
        void InitializeSimicsNetworkPortInterface(SimicsNetworkPortInterface* snpi);
}

#endif
