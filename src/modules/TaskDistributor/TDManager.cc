#include "TDManager.hh"
#include <cassert>

void TDManager::RegisterNewDevice(int deviceHandle)
{
  assert(tdSet.find(deviceHandle) == tdSet.end());
  tdSet[deviceHandle] = new TD();
}
void TDManager::AddCFU(int device, int id, int cpu, int type)
{
  assert(tdSet.find(device) != tdSet.end());
  tdSet[device]->AddCFU(id, cpu, type);
}
void TDManager::RemoveCFU(int device, int id)
{
  assert(tdSet.find(device) != tdSet.end());
  tdSet[device]->RemoveCFU(id);
}
void TDManager::SetSelector(int device, const std::string& selectorName)
{
  assert(tdSet.find(device) != tdSet.end());
  tdSet[device]->SetSelector(selectorName);
}
void TDManager::SetTaskGrain(int device, int grain)
{
  assert(tdSet.find(device) != tdSet.end());
  tdSet[device]->SetTaskGrain(grain);
}
void TDManager::SetCFUAllocationPerIsland(int device, int cfuCount)
{
  assert(tdSet.find(device) != tdSet.end());
  tdSet[device]->SetCFUAllocationPerIsland(cfuCount);
}
void TDManager::AddNetworkPort(int deviceHandle, int port, int device)
{
  assert(tdSet.find(deviceHandle) != tdSet.end());
  assert(netSet.find(device) == netSet.end());
  //TODO: somehow the port and device meaning in NetworkInterface has been reversed.
  //so if you want to get port, use device; vice versa.
  //To fix this in the future.
  NetworkInterface* netInf = new NetworkInterface(port, device);
  netSet[device] = netInf;
  tdSet[deviceHandle]->AddNetworkInterface(netInf);
}
void TDManager::SetFpgaArea(int device, int area)
{
  assert(tdSet.find(device) != tdSet.end());
  tdSet[device]->SetFpgaArea(area);
}
void TDManager::AddFpgaPort(int device, int port)
{
  assert(tdSet.find(device) != tdSet.end());
  tdSet[device]->AddFpgaPort(port);
}
void TDManager::AddFpgaRecipe(int device, int opCode, int area, int ii, int pipeline, int cycleMult)
{
  assert(tdSet.find(device) != tdSet.end());
  tdSet[device]->AddFpgaRecipe(opCode, area, ii, pipeline, cycleMult);
}
void TDManager::FinalizeDeviceCreation(int deviceHandle)
{
  assert(tdSet.find(deviceHandle) != tdSet.end());
  tdSet[deviceHandle]->Initialize();
}
void TDManager::ProcessMessage(int source, int destination, unsigned int size, const void* buffer)
{
  assert(netSet.find(destination) != netSet.end());
  netSet[destination]->RouteMsg(source, buffer, size);
}
void TDManager::ClearCFUFilter(int device, uint32_t thread)
{
  assert(tdSet.find(device) != tdSet.end());
  tdSet[device]->ClearCFUFilter(thread);
}
void TDManager::AddCFUFilter(int device, uint32_t thread, int cfuID)
{
  assert(tdSet.find(device) != tdSet.end());
  tdSet[device]->AddCFUFilter(thread, cfuID);
}
