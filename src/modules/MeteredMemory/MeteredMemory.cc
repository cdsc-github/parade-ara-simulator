#include <iostream>
#include "MeteredMemory.hh"
#include "gem5Interface.hh"
#include "modules/MsgLogger/MsgLogger.h"
#include "base/trace.hh"

using namespace MeteredMemory_Interface;

void
MeteredMemory::SendResponse(gem5MemoryInterfaceMemResponse mResp, int port,
  int deviceID)
{
  ML_LOG("meteredmemory", "END memory access " << mResp.physicalAddr << " "
    << TypeToString(mResp.type));

  interface->EmitMemResponse(mResp, port);
  assert(accessesByDevice.find(deviceID) != accessesByDevice.end());
  assert(accessesByDevice[deviceID] > 0);
  accessesByDevice[deviceID]--;
}

MeteredMemory::MeteredMemory(gem5Interface* interface,
  const std::string& deviceName)
{
  assert(interface);
  this->interface = interface;
  currentOverflow = 0;
  bytesPerSec = 100;
  clock = 100;
  latency = 100;
  currentOccupancy = 0;
  tick = 0;
  this->deviceName = deviceName;
}

void
MeteredMemory::Tick()
{
  if (currentOverflow > bytesPerSec) {
    currentOccupancy++;
    currentOverflow -= bytesPerSec;
  } else {
    currentOverflow = 0;
  }

  tick++;

  if (tick == 1000) {
    tick = 0;
    ML_LOG("meteredmemory", "running at " << currentOccupancy
      << "/1000, with overflow of " << (currentOverflow / bytesPerSec));
    currentOccupancy = 0;
  }
}

void
MeteredMemory::IssueRead(uint64_t addr, size_t size, CallbackBase* cb)
{
  uint64_t delay = (size * clock + currentOverflow) / bytesPerSec;
  currentOverflow += size * clock;
  gem5Interface::ScheduleCB(cb, (latency + delay));
}

void
MeteredMemory::IssueWrite(uint64_t addr, size_t size, CallbackBase* cb)
{
  uint64_t delay = (size * clock + currentOverflow) / bytesPerSec;
  currentOverflow += size * clock;
  gem5Interface::ScheduleCB(cb, (latency + delay));
}

void
MeteredMemory::SetLatency(uint64_t x)
{
  latency = x;
}

void
MeteredMemory::SetClock(uint64_t x)
{
  clock = x;
}

void
MeteredMemory::SetBytesPerSec(uint64_t x)
{
  bytesPerSec = x;
}

void
MeteredMemory::RecvRequest(const SimicsMemoryInterfaceMemRequest& mReq, int port)
{
  if (!mReq.responseNeeded) {
    return;
  }

  ML_LOG("meteredmemory", "BEGIN memory access " << mReq.solicitingDeviceID
    << " " << mReq.physicalAddr << " " << TypeToString(mReq.type));

  int conflictCounter = 0;
  for (std::map<int, int>::iterator it = accessesByDevice.begin(); it != accessesByDevice.end(); it++) {
    if (it->first == mReq.solicitingDeviceID) {
      continue;
    }
    if (it->second > 0) {
      conflictCounter++;
    }
  }

  if (conflictCounter > 0) {
    ML_LOG("meteredmemory", "conflict from device " << mReq.solicitingDeviceID
      << " with " << conflictCounter << " devices");
  }

  if (accessesByDevice.find(mReq.solicitingDeviceID) == accessesByDevice.end()) {
    accessesByDevice[mReq.solicitingDeviceID] = 0;
  }

  accessesByDevice[mReq.solicitingDeviceID]++;

  gem5MemoryInterfaceMemResponse resp;
  resp.requestID = mReq.requestID;
  resp.bufferID = mReq.bufferID;
  resp.bufferSize = mReq.bufferSize;
  resp.priority = mReq.priority;
  resp.target = mReq.source;
  resp.targetType = mReq.sourceType;
  resp.source = mReq.target;
  resp.sourceType = mReq.targetType;
  resp.logicalAddr = mReq.logicalAddr;
  resp.physicalAddr = mReq.physicalAddr;
  resp.size = mReq.size;
  resp.type = mReq.type;

  uint64_t delay = (mReq.size * clock + currentOverflow) / bytesPerSec;
  currentOverflow += mReq.size * clock;

  gem5Interface::ScheduleCB(
    SendResponseCB::Create(this, resp, port, mReq.solicitingDeviceID),
    (latency + delay));
}
