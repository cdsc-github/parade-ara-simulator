#include "SimicsHeader.h"
#include <stdint.h>
#include <stdlib.h>

#include "m5op.h"

#define MAGIC_LWI_REGISTER 0xC000
#define MAGIC_LWI_CLEAR_INTERRUPT 0xC004
#define MAGIC_LWI_CHECK 0xC00C
#define MAGIC_LCACC_RESERVE 0xC010
#define MAGIC_LCACC_REQUEST 0xC011
#define MAGIC_LCACC_COMMAND 0xC012
#define MAGIC_LCACC_DECLARE_ACC 0xC019
#define MAGIC_BIN_CURVE 0xC020
#define MAGIC_LCACC_FREE 0xC013

/** MAGIC_CMD:
 * This is a macro that loads the 6 arguments for the added pseudo command
 * 0x0F 0x04 0x58, which causes a message to be sent to the specified
 * accelerator.
 */
#define MAGIC_CMD(RET,X,A1,A2,A3,A4,A5,A6,A7) \
uint64_t ret_val; \
__asm__ __volatile__ ("movq %1,%%rdi\n\tmovq %2,%%rsi\n\tmovq %3,%%rdx\n\tmovq %4,%%rcx\n\tmovq %5,%%r8\n\tmovq %6,%%r9\n\tmovq %7,%%r10\n\tmovq %8,%%r11\n\t.byte 0x0F,0x04\n\t.word 0x58\n\tmovq %%rax,%0" : "=r" (ret_val) : "g" ((uint64_t)X), "g" ((uint64_t)A1), "g" ((uint64_t)A2), "g" ((uint64_t)A3), "g" ((uint64_t)A4), "g" ((uint64_t)A5), "g" ((uint64_t)A6), "g" ((uint64_t)A7): "rdi", "rsi", "rdx", "rcx", "r8", "r9", "r10", "r11", "rax" ); \
RET = ret_val;

InterruptArgs*
LWI_CheckInterrupt(uint32_t thread)
{
    InterruptArgs* ret = NULL;
    uint64_t val;
    //std::cout << "[LWI_CheckInterrupt]MAGIC_LWI_CHECK for thread " << thread << std::endl;
    MAGIC_CMD(val, MAGIC_LWI_CHECK, thread, 0,0,0,0,0,0);
    ret = reinterpret_cast<InterruptArgs*>(val);
    //std::cout << "[LWI_CheckInterrupt]Returned " << ret << std::endl;
    //std::cout << "[LWI_CheckInterrupt]InterruptArgs[" << ret << "]" << ret->status << ", " << ret->threadID << ", " << ret->lcaccID << ", " << ret->taskIndex << ", " << ret->lcaccMode << ", " << ret->replicate << std::endl;
    return ret;
}

void
LWI_RegisterInterruptHandler(InterruptArgs* argPtr)
{
    uint32_t lcacc = argPtr->lcaccID;
    uint32_t threadID = argPtr->threadID;
    int max = sizeof(InterruptArgs);

    //force the OS to allocate physical pages
    for (int i = 0; i < max; i += 4096) {
        uint8_t* x = ((uint8_t*)argPtr) + i;
        uint8_t touched = *x;
        *x = touched;
    }

    // Also need to touch the last byte, previous loop may miss the last page
    // (unaligned case) because the step is 4096
    uint8_t trashVal = *(((uint8_t*)argPtr) + max - 1);
    *(((uint8_t*)argPtr) + max - 1) = trashVal;

    for (int i = 0; i < max;) {
        MAGIC_CMD(i, MAGIC_LWI_REGISTER, threadID, lcacc, argPtr, i, max,0,0);
    }
}

void
LWI_ClearInterrupt(uint32_t thread)
{
    int ret;
    MAGIC_CMD(ret, MAGIC_LWI_CLEAR_INTERRUPT, thread, 0,0,0,0,0,0);
}

void
LWI_ClearInterrupt(uint32_t thread, InterruptArgs* args)
{
    int ret;
    MAGIC_CMD(ret, MAGIC_LWI_CLEAR_INTERRUPT, thread, args,0,0,0,0,0);
}

void
LWI_UnregisterInterruptHandler(uint32_t threadID, int lcacc)
{

}

void
LCAcc_Reserve(uint32_t threadID, int lcaccID, uint32_t kCycles)
{
    int ret;
    MAGIC_CMD(ret, MAGIC_LCACC_RESERVE, threadID, lcaccID, kCycles, 0,0,0,0);
}

void
LCAcc_Request(uint32_t threadID, uint32_t opMode, int requestAll)
{
    int32_t actualOpMode = (int32_t)opMode;
    if (requestAll) {
        actualOpMode *= -1;
    }
    int ret;
    MAGIC_CMD(ret, MAGIC_LCACC_REQUEST, threadID, actualOpMode, 0,0,0,0,0);
}

void
LCAcc_DeclareLCAccUse(uint32_t threadID, uint32_t opMode, uint32_t accCount)
{
    int ret;
    MAGIC_CMD(ret, MAGIC_LCACC_DECLARE_ACC, threadID, opMode, accCount, 0,0,0,0);
}

void
LCAcc_SendBiNCurve(uint32_t threadID, uint32_t bufSize, uint32_t bufTime,
                   uint32_t cacheMod)
{
    int ret;
    MAGIC_CMD(ret, MAGIC_BIN_CURVE, threadID, bufSize, bufTime, cacheMod, 0,0,0);
}

void
LCAcc_Command(uint32_t threadID, int lcaccID, int32_t action, void* addr,
              int32_t arg1, int32_t arg2, int32_t arg3)
{
    int finished = 0;
    while (!finished) {
        if (addr) {
            char* dummy = (char*) addr;
            char dummy2 = *dummy;
            *dummy = dummy2;
        }
        MAGIC_CMD(finished, MAGIC_LCACC_COMMAND, threadID, lcaccID, action,
                  reinterpret_cast<uint64_t>(addr), arg1, arg2, arg3); //set return addresses
    }
}

void
LCAcc_Free(uint32_t threadID, int lcaccID)
{
    uint64_t ret;
    MAGIC_CMD(ret, MAGIC_LCACC_FREE, threadID, lcaccID, 0,0,0,0,0); //set return addresses
}

void
BarrierTick(int t, int id)
{
    uint64_t ret;
    //__asm__ __volatile__("lduw %0, %%l3;"  :  :"m"(t) : "%l3");
    //__asm__ __volatile__("lduw %0, %%l4;"  :  :"m"(id) : "%l4");
    //MAGIC_CMD(0xBA00);
    MAGIC_CMD(ret, 0xBA00, t, id, 0,0,0,0,0);
}

void
StartSim(uint64_t workid, uint64_t threadid)
{
    m5_reset_stats(0, 0);
    m5_work_begin(workid, threadid);
}

void
EndSim(uint64_t workid, uint64_t threadid)
{
    m5_work_end(workid, threadid);
}

void
ExitSim()
{
    m5_exit(0);
}

void
KillSimulation()
{
    while (true) {
        ExitSim();
    }
}

void
BarrierWait(int t, int id)
{
    int x = 1;
    while (x) {
        MAGIC_CMD(x, 0xBA01, t, id, 0,0,0,0,0);
    }
    for (x = 0; x < 10; x++);
}

void
simics_break()
{
    m5_debugbreak();
}

void
simics_debug(uint32_t thread, uint32_t marker)
{
    uint64_t ret;
    MAGIC_CMD(ret, 0xCCCE, thread, marker, 0,0,0,0,0);
}

void
Touch(int thread, void* addr, uint32_t size)
{
    uint8_t* a = (uint8_t*) addr;
    uint64_t ret;
    for (int i = 0; i < size; i += 4096) {
        {
            //enforce OS to allocate a physical page and setup the page table
            uint8_t trashVal = *(a + i);
            *(a + i) = trashVal;
        }
        MAGIC_CMD(ret, 0xC101, thread, (a + i), i, size, 0, 0, 0);
    }

    // Also need to touch the last byte, previous loop may miss the last page
    // (unaligned case) because the step is 4096
    uint8_t trashVal = *(a + size - 1);
    *(a + size - 1) = trashVal;
    MAGIC_CMD(ret, 0xC101, thread, (a + size - 1), size - 1, size, 0, 0, 0);
}
