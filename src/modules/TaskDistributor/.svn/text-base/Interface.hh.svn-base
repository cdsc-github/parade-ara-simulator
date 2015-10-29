#ifndef INTERFACE_TASKDISTRIBUTOR_H
#define INTERFACE_TASKDISTRIBUTOR_H

class TDHandle;
class TaskDistributorInterface
{
public:
  void (*SetCFUAllocationPerIsland)(TDHandle* handle, int val);
  void (*Initialize)(TDHandle* handle);
  void (*SetNetworkPort)(TDHandle* handle, int port, int device);
  void (*SetTaskGrain)(TDHandle* handle, int val);
  void (*AddCFU)(TDHandle* obj, int id, int port, const char* opName);
  void (*RemoveCFU)(TDHandle* obj, int id);
  void (*AddCFUFilter)(TDHandle* obj, uint32_t threadID, int cfuID);
  void (*ClearCFUFilter)(TDHandle* obj, uint32_t threadID);
  void (*SetSelector)(TDHandle* handle, std::string val);
  void (*SetFpgaArea)(TDHandle* handle, int val);
  void (*AddFpgaPort)(TDHandle* handle, int val);
};

TDHandle* CreateNewTDHandle();
TaskDistributorInterface* CreateTDInterface();

#endif
