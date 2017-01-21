#include "LCAccManager.hh"
#include <cassert>

namespace LCAcc
{
void LCAccManager::SetTiming(int deviceHandle, int CycleTime, int PipelineDepth, int InitiationInterval)
{
  std::cout << "SetTiming for Device " << deviceHandle << "CycleTime: " << CycleTime << "PipelineDepth: " << PipelineDepth << "InitiationInterval: " << InitiationInterval << std::endl;
  deviceSet[deviceHandle]->SetTiming(CycleTime, PipelineDepth, InitiationInterval);
}
void LCAccManager::RegisterDevice(int deviceHandle)
{
  assert(deviceSet.find(deviceHandle) == deviceSet.end());
  deviceSet[deviceHandle] = new LCAccDevice();
}
void LCAccManager::UnregisterDevice(int deviceHandle)
{
  assert(deviceSet.find(deviceHandle) != deviceSet.end());
  delete deviceSet[deviceHandle];
  deviceSet.erase(deviceHandle);
}
void LCAccManager::AddOperatingMode(int deviceHandle, const std::string& type)
{
  assert(deviceSet.find(deviceHandle) != deviceSet.end());
  deviceSet[deviceHandle]->AddOperatingMode(type);
}
void LCAccManager::AddNetworkPort(int deviceHandle, int port, int device)
{
  // std::cout << "Establishing network port " << port << " device " << device << std::endl;
  assert(deviceSet.find(deviceHandle) != deviceSet.end());
  assert(netSet.find(device) == netSet.end());
  //TODO: somehow the port and device meaning in NetworkInterface has been reversed.
  //so if you want to get port, use device; vice versa.
  //To fix this in the future.
  NetworkInterface* netInf = new NetworkInterface(port, device);
  netSet[device] = netInf;
  deviceSet[deviceHandle]->AddNetworkInterface(netInf);
}
void LCAccManager::SetPrefetchDistance(int deviceHandle, int dist)
{
  assert(deviceSet.find(deviceHandle) != deviceSet.end());
  deviceSet[deviceHandle]->SetPrefetchDistance(dist);
}
void LCAccManager::SetSPMConfig(int deviceHandle, int banks, int bankMod, int readPorts, int readLatency, int writePorts, int writeLatency)
{
  assert(deviceSet.find(deviceHandle) != deviceSet.end());
  deviceSet[deviceHandle]->SetSPMConfig(banks, bankMod, readPorts, readLatency, writePorts, writeLatency);
}
void LCAccManager::FinalizeDeviceCreation(int deviceHandle, uint32_t threadID)
{
  assert(deviceSet.find(deviceHandle) != deviceSet.end());
  deviceSet[deviceHandle]->Initialize(threadID);
}
bool LCAccManager::IsIdle(int deviceHandle)
{
  assert(deviceSet.find(deviceHandle) != deviceSet.end());
  return deviceSet[deviceHandle]->IsIdle();
}
void LCAccManager::ProcessMessage(int source, int destination, unsigned int size, const void* buffer)
{
  assert(netSet.find(destination) != netSet.end());
  netSet[destination]->RouteMsg(source, buffer, size);
}
void LCAccManager::HookToMemoryDevice(int deviceHandle, const std::string& name)
{
  assert(deviceSet.find(deviceHandle) != deviceSet.end());
  deviceSet[deviceHandle]->HookToMemoryDevice(name);
}
}
