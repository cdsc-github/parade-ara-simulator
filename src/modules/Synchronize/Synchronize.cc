#include "Synchronize.hh"
#include <cassert>
#include <stdint.h>
#include <iostream>
#include <map>
#include <stdint.h>
#include <map>
#include <set>
#include <queue>
#include <vector>
#include "modules/MsgLogger/MsgLogger.hh"

std::map<int, int> barrierSet;
int barrierMax;
void
SetBarrierWidth(int val)
{
  barrierMax = val;
}

uint64_t
MagicIntercept(void*, ThreadContext* cpu, int32_t op, int barrierID)
{
  uint64_t retVal = 0;

  switch (op) {
  case (0xBA00): { //Barrier Tick
    ML_LOG("barrier", "threadcontext " << cpu->contextId()
           << " tick(" << barrierID << ")");

    if (barrierSet.find(barrierID) == barrierSet.end()) {
      barrierSet[barrierID] = 0;
    }

    barrierSet[barrierID]++;
  }
  break;

  case (0xBA01): { //Barrier Blocked
    // ML_LOG("barrier", "threadcontext " << cpu->contextId()
    //     << " wait(" << barrierID << ")");
    if (barrierSet.find(barrierID) != barrierSet.end()
        && barrierSet[barrierID] == barrierMax) {
      barrierSet.erase(barrierID);
    }

    if (barrierSet.find(barrierID) == barrierSet.end()) {
      retVal = 0;
    } else {
      retVal = 1;
    }
  }
  break;
  }

  return retVal;
}
