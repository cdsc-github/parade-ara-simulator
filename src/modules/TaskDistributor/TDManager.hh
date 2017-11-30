#ifndef TD_MANAGER_H
#define TD_MANAGER_H

#include "TD.hh"
#include "SPMInterface.hh"
#include "NetworkInterface.hh"
#include <map>
#include <vector>
#include <string>
#include <cassert>

class TDManager
{
public:
  std::map<int, TD*> tdSet;
  std::map<int, NetworkInterface*> netSet;
  std::map<int, SPMInterface*> spmSet;
public:
  void RegisterNewDevice(int deviceHandle);
  void AddNetworkPort(int deviceHandle, int port, int device);
  void AddCFU(int device, int id, int cpu, int type);
  void RemoveCFU(int device, int id);
  void SetSelector(int device, const std::string& selectorName);
  void SetTaskGrain(int device, int grain);
  void SetCFUAllocationPerIsland(int device, int cfuCount);
  void SetFpgaArea(int device, int area);
  void AddFpgaPort(int device, int port);
  void AddFpgaRecipe(int device, int opcode, int area, int ii, int pipeline, int cycleMult);
  void FinalizeDeviceCreation(int deviceHandle);
  void ProcessMessage(int source, int destination, unsigned int size, const void* buffer);
  void ClearCFUFilter(int device, uint32_t thread);
  void AddCFUFilter(int device, uint32_t thread, int cfuID);
  TD* GetTD(int deviceID)
  {
    assert(tdSet.find(deviceID) != tdSet.end());
    return tdSet[deviceID];
  }
};

#endif
