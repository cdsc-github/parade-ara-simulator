#ifndef METERED_MEMORY_INTERFACE_H
#define METERED_MEMORY_INTERFACE_H

#include <string>
#include <map>
#include "MemoryDeviceInterface.hh"
#include "modules/Common/BaseCallbacks.hh"

class MeteredMemory
{
  uint64_t currentOverflow;

  // void SendResponse(gem5MemoryInterfaceMemResponse mResp, int port, int deviceID);

  // typedef MemberCallback3<MeteredMemory, gem5MemoryInterfaceMemResponse,
  //   int, int, &MeteredMemory::SendResponse> SendResponseCB;

  uint64_t bytesPerSec;
  uint64_t clock;
  uint64_t latency;

  int currentOccupancy;
  int tick;

  uint64_t nextTransfer;

  int portID;
  std::map<int, int> accessesByDevice;

public:
  MeteredMemory(int portID);

  void Tick();

  void IssueRead(uint64_t addr, size_t size, CallbackBase* cb);

  void IssueWrite(uint64_t addr, size_t size, CallbackBase* cb);

  void SetLatency(uint64_t x);

  void SetClock(uint64_t x);

  void SetBytesPerSec(uint64_t x);

  // void RecvRequest(const gem5MemoryInterfaceMemRequest& mReq, int port);

  // std::string TypeToString(int type)
  // {
  //   if (type == SIM_MEMORY_READ) {
  //     return "Read";
  //   } else if(type == SIM_MEMORY_WRITE) {
  //     return "Write";
  //   } else {
  //     return "Access";
  //   }
  // }

  inline std::string GetDeviceName()
  {
    char s[20];
    sprintf(s, "meteredmemory.%02d", portID);
    return s;
  }
};

#endif
