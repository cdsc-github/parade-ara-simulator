#ifndef SPMINTERFACE_H
#define SPMINTERFACE_H

#include <stdint.h>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include "../Common/BaseCallbacks.hh"
#include "../scratch-pad/scratch-pad.hh"

class scratch_pad_interface;
typedef scratch_pad_interface scratch_pad_interface_t;

namespace LCAcc
{
	class SPMInterface
	{
		class PendingRequest
		{
		public:
			bool isRead;
			uint64_t addr;
			size_t size;
			CallbackBase* cb;
		};
		int id;
		int cpu;
	        ScratchPadHandle* spmObject;
		scratch_pad_interface_t* spmInterface;
//start of BiN stuff
		int buffer;
		size_t maxPendingReads;
		size_t maxPendingWrites;
		size_t maxOutstandingAccesses;
		size_t outstandingAccesses;
		std::map<uint64_t, std::vector<CallbackBase*> > pendingReadSet;
		std::map<uint64_t, std::vector<CallbackBase*> > pendingWriteSet;
		std::queue<PendingRequest> waitingAccessSet;
//end of BiN stuff
//Start of banked access stuff
		int bankMod;
		std::vector<unsigned long long> readSlot;//"Slots" reference sub-cycle read/write allocations.  Each 'slot' refers to a port per cycle.  This is used to track the number of accesses that have occured during the current, and future cycles.  The number here represents <cycle> * <port count> + <port>, and indicates the most recently allocated port for a given cycle.  When an access is performed, this value is read and compared against <cycle> * <port count>.  If it is smaller, then it is set to <cycle> * <port count>, and the access occurs during this cycle.  If it is equal or larger, it is set to <old value> + 1, and the access is initiated at <old value> / <port count>, which may be a time in the future.  This tracks the ordering of accesses without needing to maintain an explicit queue. or re-try accesses.  There is one of these values per bank.
		std::vector<unsigned long long> writeSlot;
		int readPorts;
		int writePorts;
		int readLatency;
		int writeLatency;
//end of banked access stuff
		std::map<unsigned int, unsigned int> addrMap;
		bool addrMapValid;
		std::string hostName;
		void DrainPending();
		void TimedReadComplete(uint64_t addr);
		void TimedWriteComplete(uint64_t addr);
		typedef MemberCallback1<SPMInterface, uint64_t, &SPMInterface::TimedReadComplete> TimedReadCompleteCB;
		typedef MemberCallback1<SPMInterface, uint64_t, &SPMInterface::TimedWriteComplete> TimedWriteCompleteCB;
	  uint64_t num_reads;
	  uint64_t num_writes;
	public:
		SPMInterface(const std::string& hostName, int cpuPort, int id, int size, int banks, int bankMod, int readPorts, int readLatency, int writePorts, int writeLatency);
		~SPMInterface();
		void Write(unsigned int addr, unsigned int size, void* buffer);
		void Read(unsigned int addr, unsigned int size, void* buffer);
		void TimedWrite(unsigned int addr, unsigned int size, CallbackBase* cb);
		void TimedRead(unsigned int addr, unsigned int size, CallbackBase* cb);
		void Clear();
		void SetBuffer(int buf);
		void SetAddressMap(const std::map<unsigned int, unsigned int>& addressMap);
		void PurgeAddrMap();
		int GetID() const;
    	        uint64_t get_num_spm_reads() {return num_reads;}
	        uint64_t get_num_spm_writes() {return num_writes;}
	};
}

#endif
