#ifndef SIMICS_HEADER_H
#define SIMICS_HEADER_H

#include <stdint.h>
#include <iostream>
#include <cassert>
#include <string.h>

#define LCACC_STATUS_TLB_MISS 45
#define LCACC_STATUS_COMPLETED 46
#define LCACC_STATUS_ERROR 3
#define LCACC_STATUS_PENDING 4
#define LCACC_GAM_WAIT 5
#define LCACC_GAM_GRANT 4
#define LCACC_GAM_ERROR 6
#define LCACC_GAM_REVOKE 7
#define LCACC_CMD_BEGIN_TASK 42
#define LCACC_CMD_BEGIN_PROGRAM 50
#define LCACC_CMD_BEGIN_TASK_SIGNATURE 47
#define LCACC_CMD_TLB_SERVICE 44
#define LCACC_CMD_CANCEL_TASK 43
#define LWI_WAIT 2
#define LWI_PROCEED 1
#define LWI_ERROR 0
#define LWI_ARG_MAX_SIZE 128

class InterruptArgs
{
public:
        uint32_t status;
        uint32_t threadID;
        uint32_t v[(LWI_ARG_MAX_SIZE - (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t))) / 4];
        uint32_t lcaccID;
        uint32_t taskIndex;
        uint32_t lcaccMode;
        uint32_t replicate;
};
InterruptArgs* LWI_CheckInterrupt(uint32_t thread);
void LWI_RegisterInterruptHandler(InterruptArgs* argPtr);
void LWI_ClearInterrupt(uint32_t thread);
void LWI_ClearInterrupt(uint32_t thread, InterruptArgs* args);
void LWI_UnregisterInterruptHandler(uint32_t threadID, int lcacc);
void LCAcc_Reserve(uint32_t threadID, int lcaccID, uint32_t kCycles);
void LCAcc_Request(uint32_t threadID, uint32_t opMode, int requestAll);
void LCAcc_DeclareLCAccUse(uint32_t threadID, uint32_t opMode, uint32_t accCount);
void LCAcc_SendBiNCurve(uint32_t threadID, uint32_t bufSize, uint32_t bufTime, uint32_t cacheMod);
void LCAcc_Command(uint32_t threadID, int lcaccID, int32_t action, void* addr, int32_t arg1, int32_t arg2, int32_t arg3);
void LCAcc_Free(uint32_t threadID, int lcaccID);
void BarrierTick(int t, int id);
void StartSim(uint64_t workid, uint64_t threadid);
void EndSim(uint64_t workid, uint64_t threadid);
void ExitSim(); // Stops the simulation, calling m5_exit
void KillSimulation(); //Same as StartSim() followed by StopSim()
void BarrierWait(int t, int id);
/*****************legacy name************************/
void simics_break();// Attempt to connect to debuger, then exit simulation.
void simics_debug(uint32_t thread, uint32_t marker); // Prints out time, thread + marker
/*****************legacy name************************/
void Touch(int thread, void* addr, uint32_t size);

#ifndef simics_assert
#define simics_assert(x) {if(!(x)){simics_debug(0xBADBAD, __LINE__); while(true){std::cout << __FILE__ << ":" << __LINE__ << std::endl; simics_break();}}}
#endif
#endif
