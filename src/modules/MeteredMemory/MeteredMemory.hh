#ifndef METERED_MEMORY_INTERFACE_H
#define METERED_MEMORY_INTERFACE_H

#include "Global.hh"
#include <string>
#include <map>

namespace MeteredMemory_Interface
{
class gem5Interface;
}

class MeteredMemory
{
  MeteredMemory_Interface::gem5Interface* interface;

  uint64_t currentOverflow;

  void SendResponse(gem5MemoryInterfaceMemResponse mResp, int port, int deviceID);

  typedef MemberCallback3<MeteredMemory, SimicsMemoryInterfaceMemResponse,
    int, int, &MeteredMemory::SendResponse> SendResponseCB;

  uint64_t bytesPerSec;
  uint64_t clock;
  uint64_t latency;

  int currentOccupancy;
  int tick;

  std::string deviceName;
  std::map<int, int> accessesByDevice;

public:
  MeteredMemory(MeteredMemory_Interface::gem5Interface* interface,
    const std::string& deviceName);

  void Tick();

  void IssueRead(uint64_t addr, size_t size, CallbackBase* cb);

  void IssueWrite(uint64_t addr, size_t size, CallbackBase* cb);

  void SetLatency(uint64_t x);

  void SetClock(uint64_t x);

  void SetBytesPerSec(uint64_t x);

  void RecvRequest(const SimicsMemoryInterfaceMemRequest& mReq, int port);

  std::string TypeToString(int type)
  {
    if (type == SIM_MEMORY_READ) {
      return "Read";
    } else if(type == SIM_MEMORY_WRITE) {
      return "Write";
    } else {
      return "Access";
    }
  }
};

#endif
