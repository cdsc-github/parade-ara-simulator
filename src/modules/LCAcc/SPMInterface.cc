#include "SPMInterface.hh"
#include "gem5Interface.hh"
#include <cassert>
#include "../MsgLogger/MsgLogger.hh"
#include "../scratch-pad/scratch-pad.hh"
#include "../../mem/ruby/common/Global.hh"
#include <iostream>

using namespace LCAcc;

namespace LCAcc
{
void SPMInterface::DrainPending()
{
  while (!waitingAccessSet.empty() && outstandingAccesses < maxOutstandingAccesses) {
    PendingRequest p = waitingAccessSet.front();

    if (p.isRead && pendingReadSet.size() < maxPendingReads) {
      bool repeat = pendingReadSet.find(p.addr) != pendingReadSet.end();
      pendingReadSet[p.addr].push_back(p.cb);

      if (!repeat) {
        gem5Interface::TimedBufferRead(cpu, p.addr, p.size, buffer, TimedReadCompleteCB::Create(this, p.addr));
      }

    } else if (!p.isRead && pendingWriteSet.size() < maxPendingWrites) {
      bool repeat = pendingWriteSet.find(p.addr) != pendingWriteSet.end();
      pendingWriteSet[p.addr].push_back(p.cb);

      if (!repeat) {
        gem5Interface::TimedBufferWrite(cpu, p.addr, p.size, buffer, TimedWriteCompleteCB::Create(this, p.addr));
      }
    } else {
      return;
    }

    outstandingAccesses++;
    waitingAccessSet.pop();
  }
}
void SPMInterface::TimedReadComplete(uint64_t addr)
{
  assert(pendingReadSet.find(addr) != pendingReadSet.end());
  assert(pendingReadSet[addr].size() > 0);
  assert(outstandingAccesses >= pendingReadSet[addr].size());
  outstandingAccesses -= pendingReadSet[addr].size();

  for (size_t i = 0; i < pendingReadSet[addr].size(); i++) {
    assert(pendingReadSet[addr][i]);
    gem5Interface::RegisterCallback(pendingReadSet[addr][i], 0);
  }

  pendingReadSet.erase(addr);
  DrainPending();
}
void SPMInterface::TimedWriteComplete(uint64_t addr)
{
  assert(pendingWriteSet.find(addr) != pendingWriteSet.end());
  assert(pendingWriteSet[addr].size() > 0);
  assert(outstandingAccesses >= pendingWriteSet[addr].size());
  outstandingAccesses -= pendingWriteSet[addr].size();

  for (size_t i = 0; i < pendingWriteSet[addr].size(); i++) {
    assert(pendingWriteSet[addr][i]);
    gem5Interface::RegisterCallback(pendingWriteSet[addr][i], 0);
  }

  pendingWriteSet.erase(addr);
  DrainPending();
}
SPMInterface::SPMInterface(const std::string& hostName, int cpuPort, int identifier, int size, int banks, int bankMod, int readPorts, int readLatency, int writePorts, int writeLatency)
{
  cpu = cpuPort;
  this->hostName = hostName;
  buffer = -1;
  id = identifier;
  std::cout << "LCAcc:SPMInterface Using g_spmObject " << RubySystem::deviceIDtoAccID(id) << std::endl;
  spmObject = g_spmObject[RubySystem::deviceIDtoAccID(id)];
  assert(spmObject);
  spmInterface = g_spmInterface;
  assert(spmInterface);
  outstandingAccesses = 0;
  maxOutstandingAccesses = 2048;
  maxPendingWrites = 512;
  maxPendingReads = 512;
  this->readLatency = readLatency;
  this->writeLatency = writeLatency;
  this->readPorts = readPorts;
  this->writePorts = writePorts;
  this->bankMod = bankMod;

  for (int i = 0; i < banks; i++) {
    readSlot.push_back(0);
    writeSlot.push_back(0);
  }

  num_writes = 0;
  num_reads = 0;
}
SPMInterface::~SPMInterface()
{

}
void SPMInterface::Write(unsigned int addr, unsigned int size, void* buffer)
{
  num_writes++;
  spmInterface->write(spmObject, addr, buffer, size);
}
void SPMInterface::Read(unsigned int addr, unsigned int size, void* buffer)
{
  num_reads++;
  spmInterface->read(spmObject, addr, buffer, size);
}
void SPMInterface::TimedWrite(unsigned int addr, unsigned int size, CallbackBase* cb)
{
  if (addrMapValid) {
    if (addrMap.find(addr) == addrMap.end()) std::cout << "Could not find address: " << addr << std::endl;

    assert(addrMap.find(addr) != addrMap.end());
    addr = addrMap[addr];
  }

  if (buffer == -1) {
    //bank stuff goes here
    size_t bank = (addr / bankMod) % writeSlot.size();
    unsigned long long slotOfInterest = writePorts * gem5Interface::GetSystemTime();
    unsigned long long timeOfInit;

    if (writeSlot[bank] < slotOfInterest) {
      writeSlot[bank] = slotOfInterest;
      timeOfInit = 0;
    } else {
      timeOfInit = (writeSlot[bank] - slotOfInterest) / writePorts;
      writeSlot[bank]++;
    }

    //std::cout << "Write info: slot of interest=" << slotOfInterest << " slot=" << writeSlot[bank] << " time delay=" << timeOfInit << " bank=" << bank << std::endl;
    gem5Interface::RegisterCallback(cb, timeOfInit + writeLatency);
  } else {
    addr = (addr / 64) * 64;

    //std::cout << "Timed write for addr " << addr << std::endl;
    if (outstandingAccesses >= maxOutstandingAccesses || (pendingWriteSet.find(addr) == pendingWriteSet.end() && pendingWriteSet.size() >= maxPendingWrites)) {
      PendingRequest pr;
      pr.isRead = false;
      pr.addr = addr;
      pr.cb = cb;
      pr.size = size;
      waitingAccessSet.push(pr);
    } else {
      outstandingAccesses++;
      bool repeat = pendingWriteSet.find(addr) != pendingWriteSet.end();
      pendingWriteSet[addr].push_back(cb);

      if (!repeat) {
        gem5Interface::TimedBufferWrite(cpu, addr, size, buffer, TimedWriteCompleteCB::Create(this, addr));
      }
    }
  }
}
void SPMInterface::TimedRead(unsigned int addr, unsigned int size, CallbackBase* cb)
{
  if (addrMapValid) {
    if (addrMap.find(addr) == addrMap.end()) std::cout << "Could not find address: " << addr << std::endl;

    assert(addrMap.find(addr) != addrMap.end());
    addr = addrMap[addr];
  }

  if (buffer == -1) {
    //bank stuff goes here
    size_t bank = (addr / bankMod) % readSlot.size();
    unsigned long long slotOfInterest = readPorts * gem5Interface::GetSystemTime();
    unsigned long long timeOfInit;

    if (readSlot[bank] < slotOfInterest) {
      readSlot[bank] = slotOfInterest;
      timeOfInit = 0;
    } else {
      timeOfInit = (readSlot[bank] - slotOfInterest) / readPorts;
      readSlot[bank]++;
    }

    //std::cout << "Read info: slot of interest=" << slotOfInterest << " slot=" << readSlot[bank] << " time delay=" << timeOfInit << " bank=" << bank << std::endl;
    gem5Interface::RegisterCallback(cb, timeOfInit + readLatency);
  } else {
    addr = (addr / 64) * 64;

    //std::cout << "Timed read for addr " << addr << std::endl;
    if (outstandingAccesses >= maxOutstandingAccesses || (pendingReadSet.find(addr) == pendingReadSet.end() && pendingReadSet.size() >= maxPendingReads)) {
      PendingRequest pr;
      pr.isRead = true;
      pr.addr = addr;
      pr.cb = cb;
      pr.size = size;
      waitingAccessSet.push(pr);
    } else {
      outstandingAccesses++;
      bool repeat = pendingReadSet.find(addr) != pendingReadSet.end();
      pendingReadSet[addr].push_back(cb);

      if (!repeat) {
        gem5Interface::TimedBufferRead(cpu, addr, size, buffer, TimedReadCompleteCB::Create(this, addr));
      }
    }
  }
}
void SPMInterface::Clear()
{
  spmInterface->clear(spmObject);
}
void SPMInterface::SetBuffer(int buf)
{
  buffer = buf;
}
void SPMInterface::SetAddressMap(const std::map<unsigned int, unsigned int>& addressMap)
{
  addrMap = addressMap;
  addrMapValid = true;
}
void SPMInterface::PurgeAddrMap()
{
  addrMap.clear();
  addrMapValid = false;
}
int SPMInterface::GetID() const
{
  return id;
}
}
