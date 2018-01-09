#ifndef BENCHMARK_NODE_H
#define BENCHMARK_NODE_H

#include "SimicsHeader.h"
#include "m5op.h"
#include <cstdlib>
#include <cassert>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sched.h>
#include <iostream>

int set_cpu(int cpu_no)
{
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(cpu_no, &mask);
  int result = sched_setaffinity(syscall(SYS_gettid), sizeof(cpu_set_t), &mask);
  return result;
}

class BenchmarkNode
{
public:
    virtual ~BenchmarkNode(){}
    virtual void Initialize(int threadID, int procID) = 0;
    virtual void Run() = 0;
    virtual void Shutdown() = 0;
};

BenchmarkNode* CreateNode();
#define BENCH_DECL(X) \
BenchmarkNode* CreateNode() { return new X(); } \

int main(int varCount, char** vars)
{
    assert(varCount == 3);
    int proc = atoi(vars[1]);
    int thread = atoi(vars[2]);
    std::cout << vars[0] << ": binding to core " << proc << std::endl;
    if(set_cpu(proc) != 0)
    {
        KillSimulation();
    }
    BenchmarkNode* node = CreateNode();
    BarrierTick(proc, 0);
    BarrierWait(proc, 0);
    std::cout << vars[0] << ": initializing" << std::endl;
    node->Initialize(thread, proc);
    BarrierTick(proc, 1);
    BarrierWait(proc, 1);
    std::cout << vars[0] << ": entering roi" << std::endl;
    StartSim(proc, 0);
    // BarrierTick(thread, 2);
    // BarrierWait(thread, 2);
    // std::cout << vars[0] << ": running" << std::endl;
    node->Run();
    // BarrierTick(thread, 3);
    // BarrierWait(thread, 3);
    delete node;
    std::cout << vars[0] << ": exiting roi" << std::endl;
    EndSim(proc, 0);
    BarrierTick(proc, 4);
    // BarrierWait(proc, 4);
    ExitSim();
}

#endif
