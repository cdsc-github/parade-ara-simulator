#ifndef LCACCMANAGER_H
#define LCACCMANAGER_H

#include "LCAccDevice.hh"
#include "SPMInterface.hh"
#include "NetworkInterface.hh"
#include <map>
#include <vector>
#include <string>
#include <cassert>

namespace LCAcc
{
class LCAccManager
{
public:
  std::map<int, LCAccDevice*> deviceSet;
  std::map<int, NetworkInterface*> netSet;
  std::map<int, SPMInterface*> spmSet;
public:
  void RegisterDevice(int deviceHandle);
  void UnregisterDevice(int deviceHandle);
  void AddOperatingMode(int deviceHandle, const std::string& type);
  void AddNetworkPort(int deviceHandle, int port, int device);
  void SetPrefetchDistance(int deviceHandle, int dist);
  void SetSPMConfig(int deviceHandle, int banks, int bankMod, int readPorts, int readLatency, int writePorts, int writeLatency);
  void FinalizeDeviceCreation(int deviceHandle, uint32_t threadID);
  bool IsIdle(int deviceHandle);
  void ProcessMessage(int source, int destination, unsigned int size, const void* buffer);
  //depricated below this line, but is necessary for demo
  LCAccDevice* GetDevice(int deviceID)
  {
    assert(deviceSet.find(deviceID) != deviceSet.end());
    return deviceSet[deviceID];
  }
  void SetTiming(int deviceHandle, int CycleTime, int PipelineDepth, int InitiationInterval);
  void HookToMemoryDevice(int deviceHandle, const std::string& name);
};
}

#endif
