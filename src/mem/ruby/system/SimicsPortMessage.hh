#ifndef SIMICS_PORT_MESSAGE_H
#define SIMICS_PORT_MESSAGE_H

#include "SimicsNetworkPort.hh"
#include "mem/protocol/MessageSizeType.hh"
#include "../common/Address.hh"
#include <iostream>

#ifdef SIM_NET_PORTS
class SimicsPortMessage : public NetworkMessage
{
public:
	//This was added as garbage:
	Address GetAddress() const { return Address(); }	

	MessageSizeType sizeType;
	NetDest dest;
	int msgHandle;
	int length;
	unsigned long long index;
	const SimicsPortMessage& operator = (const SimicsPortMessage& m)
	{
		msgHandle = m.msgHandle;
		length = m.length;
		dest = m.dest;
		sizeType = m.sizeType;
		index = m.index;
		return *this;
	}
        SimicsPortMessage(Tick curTime) : NetworkMessage(curTime)
	{
		static unsigned long long nextIndex = 1;
		index = nextIndex++;
		msgHandle = -1;
		sizeType = MessageSizeType_Persistent_Control;
	}
	
	virtual MsgPtr clone() const
	{
 	        SimicsPortMessage* spm = new SimicsPortMessage(curTick());
		spm->dest = dest;
		spm->msgHandle = msgHandle;
		spm->length = length;
		spm->index = index;
		SimicsNetworkPort::IncrementRef(msgHandle);
		//return MsgPtr(spm);
		return std::make_shared<SimicsPortMessage>(*spm);
	}
	virtual void destroy()
	{
		SimicsNetworkPort::DecrementRef(msgHandle);
		delete this;
	}
        virtual void print(std::ostream& out) const
	{
		getDestination().print(out);
	}	
	virtual const NetDest& getDestination() const {return dest;}
	virtual NetDest& getDestination() {return dest;}
	virtual const MessageSizeType& getMessageSize() const { return sizeType; }
	virtual MessageSizeType& getMessageSize() { return sizeType; }
	virtual int getVariableLengthMessageSize() { return length; }
	virtual unsigned long long getIndex() const {return index;}
	int GetMsgHandle() const {assert(msgHandle != -1); return msgHandle;}
};
#endif

#endif
