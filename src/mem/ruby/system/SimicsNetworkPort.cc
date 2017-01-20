
//BEGIN edit by Mike, 12/22/10

//#include "interface.hh"
#include <iostream>
#include <cstdio>
#include <map>
#include <cassert>
//#include "AbstractChip.hh"
#include "SimicsNetworkPort.hh"
#include "SimicsPortMessage.hh"
#include "../network/MessageBuffer.hh"
#include "mem/ruby/common/MachineID.hh"
#include "mem/protocol/MachineType.hh"
#include "../common/Global.hh"
#include "../../../modules/NetworkInterrupt/NetworkInterrupts.hh"
#include "System.hh"
#include "../../../modules/Common/mf_api.hh"

extern "C"
{
        #include "SimicsNetworkPortInterface.hh"
}
#ifdef SIM_NET_PORTS
Network* currentNetwork;
std::vector<SimicsNetworkPort*> portSet;
std::map<int, int> deviceToPortMap;
std::map<int, int> handleToPortMap;
int SimicsNetworkPort::nextMsgID = 0;
int SimicsNetworkPort::nextRecvHandle = 0;
std::map<int, SimicsNetworkPort::SNPMsg*> SimicsNetworkPort::msgSet;
std::stack<SimicsNetworkPort::SNPMsg*> SimicsNetworkPort::msgPool;
class EndOfLineMessageDeliverStruct
{
public:
	SimicsNetworkPort* snp;
	char* buffer;
	size_t size;
	int destinationID;
	int source;
};
void SimicsNetworkPort::EndOfLineMessageDeliver(void* a)
{
	assert(a);
	EndOfLineMessageDeliverStruct* eol = (EndOfLineMessageDeliverStruct*)a;
	assert(eol->snp);
	if(eol->snp->deviceToRecvHandlerMap.find(eol->destinationID) != eol->snp->deviceToRecvHandlerMap.end())
	{
		std::vector<int>& handles = eol->snp->deviceToRecvHandlerMap[eol->destinationID];
		assert(deviceToPortMap.find(eol->destinationID) != deviceToPortMap.end());
		assert(deviceToPortMap.find(eol->source) != deviceToPortMap.end());
		for(size_t i = 0; i < handles.size(); i++)
		{
			assert(eol->snp->recvHandlerSet.find(handles[i]) != eol->snp->recvHandlerSet.end());
			assert(eol->snp->recvHandlerSet[handles[i]].callback);
			eol->snp->recvHandlerSet[handles[i]].callback(eol->snp->recvHandlerSet[handles[i]].args, eol->source, eol->destinationID, eol->buffer, eol->size);
		}
	}
	delete [] eol->buffer;
	delete eol;
}
#endif
int GetSimicsPortID(int deviceID)
{
#ifdef SIM_NET_PORTS
	assert(deviceToPortMap.find(deviceID) != deviceToPortMap.end());
	return deviceToPortMap[deviceID];
#else
	DEBUG_OUT("System was not compiled with SIM_NET_PORTS\n");
	ASSERT(0);
	return 0;
#endif
}
SimicsNetworkPort* GetSimicsPort(int nodeID)
{
#ifdef SIM_NET_PORTS
	if(!(nodeID >= 0 && nodeID < SimicsNetworkPortInterface_GetSimicsPortCount()))
	{
		std::cout << nodeID << " is not a simics port" << std::endl;
		assert(nodeID >= 0 && nodeID < SimicsNetworkPortInterface_GetSimicsPortCount());
	}
	assert(portSet[nodeID]);
	return portSet[nodeID];
#else
	DEBUG_OUT("System was not compiled with SIM_NET_PORTS\n");
	ASSERT(0);
	return NULL;
#endif
}
int SimicsNetworkPortInterface_GetSimicsPortCount()
{
#ifdef SIM_NET_PORTS
	return RubySystem::numberOfSimicsNetworkPort();
#else
        DEBUG_OUT("System was not compiled with SIM_NET_PORTS\n");
        ASSERT(0);
	return 0;
#endif
}
void SimicsNetworkPortInterface_BindDeviceToPort(int portID, int deviceID)
{
#ifdef SIM_NET_PORTS
	assert(deviceID >= 0);
	assert(deviceToPortMap.find(deviceID) == deviceToPortMap.end());
	assert(GetSimicsPort(portID) != NULL);
	deviceToPortMap[deviceID] = portID;
	//warn("device %d binded to port %d\n", deviceID, portID);
#else
        DEBUG_OUT("System was not compiled with SIM_NET_PORTS\n");
        ASSERT(0);
#endif
}
void SimicsNetworkPortInterface_UnbindDevice(int deviceID)
{
#ifdef SIM_NET_PORTS
	assert(deviceToPortMap.find(deviceID) != deviceToPortMap.end());
	deviceToPortMap.erase(deviceID);
#else
	DEBUG_OUT("System was not compiled with SIM_NET_PORTS\n");
	ASSERT(0);
#endif
}
void SimicsNetworkPortInterface_SendMessageOnPort(int source, int target, const void* buffer, int length)
{
#ifdef SIM_NET_PORTS
	GetSimicsPort(source)->SendPortMessage((const char*)buffer, length, target);
#else
        DEBUG_OUT("System was not compiled with SIM_NET_PORTS\n");
        ASSERT(0);
#endif
}
void SimicsNetworkPortInterface_SendMessageOnDevice(int source, int target, const void* buffer, int length)
{
#ifdef SIM_NET_PORTS
	if(deviceToPortMap.find(target) == deviceToPortMap.end())
	{
		std::cout << "Could not find " << target << std::endl;
		assert(deviceToPortMap.find(target) != deviceToPortMap.end());
	}
	GetSimicsPort(deviceToPortMap[source])->SendDeviceMessage(source, (const char*)buffer, length, target);
#else
        DEBUG_OUT("System was not compiled with SIM_NET_PORTS\n");
        ASSERT(0);
#endif
}
int SimicsNetworkPortInterface_RegisterRecvHandlerOnPort(int portID, void (*handler)(void*, int, int, const char*, int), void* data)
{
#ifdef SIM_NET_PORTS
	int handle = GetSimicsPort(portID)->RegisterPortMessageHandler(handler, data);
	assert(handleToPortMap.find(handle) == handleToPortMap.end());
	handleToPortMap[handle] = portID;
	return handle;
#else
        DEBUG_OUT("System was not compiled with SIM_NET_PORTS\n");
        ASSERT(0);
	return 0;
#endif
}
int SimicsNetworkPortInterface_RegisterRecvHandlerOnDevice(int deviceID, void (*handler)(void*, int, int, const char*, int), void* data)
{
#ifdef SIM_NET_PORTS
	assert(deviceToPortMap.find(deviceID) != deviceToPortMap.end());
	int handle = GetSimicsPort(deviceToPortMap[deviceID])->RegisterDeviceMessageHandler(deviceID, handler, data);
	assert(handleToPortMap.find(handle) == handleToPortMap.end());
	handleToPortMap[handle] = deviceToPortMap[deviceID];
	return handle;
#else
        DEBUG_OUT("System was not compiled with SIM_NET_PORTS\n");
        ASSERT(0);
	return 0;
#endif
}
void* SimicsNetworkPortInterface_UnregisterRecvHandler(int handle)
{
#ifdef SIM_NET_PORTS
	assert(handleToPortMap.find(handle) != handleToPortMap.end());
	void* ret = GetSimicsPort(handleToPortMap[handle])->UnregisterMessageHandler(handle);
	handleToPortMap.erase(handle);
	return ret;
#else
	DEBUG_OUT("System was not compiled with SIM_NET_PORTS\n");
	ASSERT(0);
	return NULL;
#endif
}
extern "C" void InitializeSimicsNetworkPortInterface(SimicsNetworkPortInterface* snpi)
{
	snpi->BindDeviceToPort = SimicsNetworkPortInterface_BindDeviceToPort;
	snpi->UnbindDevice = SimicsNetworkPortInterface_UnbindDevice;
	snpi->SendMessageOnDevice = SimicsNetworkPortInterface_SendMessageOnDevice;
	snpi->SendMessageOnPort = SimicsNetworkPortInterface_SendMessageOnPort;
	snpi->GetSimicsPortCount = SimicsNetworkPortInterface_GetSimicsPortCount;
	snpi->RegisterRecvHandlerOnDevice = SimicsNetworkPortInterface_RegisterRecvHandlerOnDevice;
        snpi->RegisterRecvHandlerOnPort = SimicsNetworkPortInterface_RegisterRecvHandlerOnPort;
	snpi->UnregisterRecvHandler = SimicsNetworkPortInterface_UnregisterRecvHandler;
}
#ifdef SIM_NET_PORTS
//SimicsNetworkPort::SimicsNetworkPort(const AbstractChip* c, int i)
SimicsNetworkPort::SimicsNetworkPort(int i)
{
	while(i >= (int)portSet.size())
	{
		portSet.push_back(NULL);
	}
	portSet[i] = this;
	//m_controlPtr = c;
	//net = c->getNetwork();
	port = i;
	name = "";
}
void SimicsNetworkPort::print(std::ostream& outs) const
{

}
void SimicsNetworkPort::SendPortMessage(const char* buffer, int length, int destinationID)
{
	assert(buffer);
	assert(length > 0);
	SNPMsg* m;
	m = new SNPMsg();
	m->id = nextMsgID++;
	m->refCount = 1;
	m->src = port;
	m->dst = -1;
	for(int i = 0; i < length; i++)
	{
		m->msg.push_back(buffer[i]);
	}
	assert(msgSet.find(m->id) == msgSet.end());
	msgSet[m->id] = m;
	//calc Msg
	SimicsPortMessage spm(curTick());
	spm.msgHandle = m->id;
	MachineID target;
	target.type = MachineType_SimicsNetworkPortInterface;
	target.num = destinationID;
	spm.dest.add(target);
	spm.length = length;
	//chipPtr->m_SimicsNetworkPort_mandatoryQueue_vec[port]->enqueue(spm, 1);
	//g_abs_controls[MachineType_SimicsNetworkPortInterface][port]->getMandatoryQueue()->enqueue(MsgPtr((Message *)&spm), Cycles(1));
	AbstractController* controller = g_abs_controls[MachineType_SimicsNetworkPortInterface][port];
	MessageBuffer* mandatoryQueue = controller->getMandatoryQueue();
	mandatoryQueue->setSender(controller);
	mandatoryQueue->enqueue(std::make_shared<SimicsPortMessage>(spm), Cycles(1));
	//mandatoryQueue->enqueue(MsgPtr(&spm), Cycles(1));
}
void SimicsNetworkPort::SendDeviceMessage(int source, const char* buffer, int length, int destinationID)
{
  // std::cout << "SendDeviceMessage: source " << source << " destination " << destinationID << " port " << port << " message length " << std::hex << length << " buffer content";
  // int buffer_int_length = length/4;
  // int* buffer_int = (int *)buffer;
  // for(int i = 0; i < buffer_int_length; i++)
  //   std::cout << "[" << buffer_int[i] << "]";
  // std::cout << std::dec << std::endl;
	assert(buffer);
	assert(length > 0);
	assert(deviceToPortMap.find(source) != deviceToPortMap.end());
	assert(deviceToPortMap.find(destinationID) != deviceToPortMap.end());
	assert(deviceToPortMap[source] == port);
	int destinationPortID = deviceToPortMap[destinationID];
	if(destinationPortID == port)
	{
		EndOfLineMessageDeliverStruct* eol = new EndOfLineMessageDeliverStruct();
		eol->snp = this;
		eol->source = source;
		eol->destinationID = destinationID;
		eol->size = length;
		eol->buffer = new char[length];
		memcpy(eol->buffer, buffer, length);
		//SIMICS_get_opal_interface()->scheduleCB(EndOfLineMessageDeliver, eol, 0);
		scheduleCB(EndOfLineMessageDeliver, eol, 0);
		return;
	}
	SNPMsg* m;
	m = new SNPMsg();
	m->id = nextMsgID++;
	m->refCount = 1;
	m->src = source;
	m->dst = destinationID;
	for(int i = 0; i < length; i++)
	{
		m->msg.push_back(buffer[i]);
	}
	assert(msgSet.find(m->id) == msgSet.end());
	msgSet[m->id] = m;
	SimicsPortMessage* spm = new SimicsPortMessage(curTick());
	spm->msgHandle = m->id;
	MachineID target;
	target.type = MachineType_SimicsNetworkPortInterface;
	target.num = destinationPortID;
	spm->dest.add(target);
	spm->length = length;
	AbstractController* controller = g_abs_controls[MachineType_SimicsNetworkPortInterface][port];
	MessageBuffer* mandatoryQueue = controller->getMandatoryQueue();
	mandatoryQueue->setSender(controller);
	mandatoryQueue->enqueue(std::make_shared<SimicsPortMessage>(*spm), Cycles(1));
	//mandatoryQueue->enqueue(MsgPtr(spm), Cycles(1));
	/*SimicsPortMessage spm(curTick());
	spm.msgHandle = m->id;
	MachineID target;
	target.type = MachineType_SimicsNetworkPortInterface;
	target.num = destinationPortID;
	spm.dest.add(target);
	spm.length = length;
	//chipPtr->m_SimicsNetworkPort_mandatoryQueue_vec[port]->enqueue(spm, 1);
	AbstractController* controller = g_abs_controls[MachineType_SimicsNetworkPortInterface][port];
	MessageBuffer* mandatoryQueue = controller->getMandatoryQueue();
	mandatoryQueue->setSender(controller);
	mandatoryQueue->enqueue(MsgPtr((Message *)&spm), Cycles(1));//*/
}
void SimicsNetworkPort::HandleMessage(int msgHandle)
{
	if(msgSet.find(msgHandle) != msgSet.end())
	{
		SNPMsg* m = msgSet[msgHandle];
		assert(m);
		msgSet.erase(msgHandle);
		int srcPort = m->src;
		if(deviceToRecvHandlerMap.find(m->dst) != deviceToRecvHandlerMap.end())
		{
			std::vector<int>& recvSet = deviceToRecvHandlerMap[m->dst];
			assert(deviceToPortMap.find(m->dst) != deviceToPortMap.end());
			assert(deviceToPortMap.find(m->src) != deviceToPortMap.end());
			//std::cout << "recvHandlerSet port: " << port << ", src: " << m->src << ", dst: " << m->dst << ", size: " << recvHandlerSet.size() << std::endl;

			/*for (std::map<int, SNPRecvHandler>::iterator it = recvHandlerSet.begin(); it != recvHandlerSet.end(); ++it)
			{
			  std::cout << it->first << std::endl;
			  } //*/

			for(size_t i = 0; i < recvSet.size(); i++)
			{
			  //std::cout << "recvSet[" << i << "]: " << recvSet[i] << std::endl;
				assert(recvHandlerSet.find(recvSet[i]) != recvHandlerSet.end());
				assert(recvHandlerSet[recvSet[i]].callback);
				recvHandlerSet[recvSet[i]].callback(recvHandlerSet[recvSet[i]].args, m->src, m->dst, &(m->msg[0]), (int)(m->msg.size()));
			}
		}
	        for(size_t i = 0; i < portHandlers.size(); i++)
	        {
			assert(recvHandlerSet.find(portHandlers[i]) != recvHandlerSet.end());
			assert(recvHandlerSet[portHandlers[i]].callback);
			recvHandlerSet[portHandlers[i]].callback(recvHandlerSet[portHandlers[i]].args, srcPort, port, &(m->msg[0]), (int)(m->msg.size()));
	        }
		delete m;
	}
}
int SimicsNetworkPort::RegisterPortMessageHandler(void (*handler)(void*, int, int, const char*, int), void* data)
{
	assert(handler);
	int handle = nextRecvHandle++;
	assert(recvHandlerSet.find(handle) == recvHandlerSet.end());
	SNPRecvHandler srh;
	srh.callback = handler;
	srh.args = data;
	portHandlers.push_back(handle);
	std::cout << "register recvHandlerSet, handle: " << handle << " at port: " << port << std::endl;
	recvHandlerSet[handle] = srh;
	return handle;
}
int SimicsNetworkPort::RegisterDeviceMessageHandler(int device, void (*handler)(void*, int, int, const char*, int), void* data)
{
	assert(handler);
	assert(deviceToPortMap.find(device) != deviceToPortMap.end());
	assert(deviceToPortMap[device] == port);
	int handle = nextRecvHandle++;
	assert(recvHandlerSet.find(handle) == recvHandlerSet.end());
	SNPRecvHandler srh;
	srh.callback = handler;
	srh.args = data;
	// std::cout << "register recvHandlerSet, handle: " << handle << ", device: " << device << ", at port: " << port << std::endl;
	recvHandlerSet[handle] = srh;
	deviceToRecvHandlerMap[device].push_back(handle);
	return handle;
}
void* SimicsNetworkPort::UnregisterMessageHandler(int handle)
{
  std::cout << "UnregisterMessageHandler handle " << handle << std::endl;
	assert(recvHandlerSet.find(handle) != recvHandlerSet.end());
	void* ret = recvHandlerSet[handle].args;
	bool found = false;
	for(std::map<int, std::vector<int> >::iterator it = deviceToRecvHandlerMap.begin(); it != deviceToRecvHandlerMap.end() && !found; it++)
	{
		for(std::vector<int>::iterator vit = it->second.begin(); vit != it->second.end() && !found; vit++)
		{
			if(*vit == handle)
			{
				found = true;
				it->second.erase(vit);
				if(it->second.empty())
				{
					deviceToRecvHandlerMap.erase(it);
				}
			}
		}
	}
	assert(found);
	return ret;
}
void SimicsNetworkPort::IncrementRef(int handle)
{
}
void SimicsNetworkPort::DecrementRef(int handle)
{
}
#endif
