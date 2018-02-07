#include "TD.hh"
#include <iostream>
#include <cassert>
#include "../MsgLogger/MsgLogger.hh"
#include "../Common/TransferDescription.hh"
#include "../Common/ComputeDescription.hh"
#include "../Common/BitConverter.hh"
#include "../LCAcc/LCAccCommandListing.hh"
#include "gem5Interface.hh"
#include "DMAController.hh"
#include "SPMInterface.hh"
#include "NetworkInterface.hh"
#include "PatternSelector.hh"
#include "PatternSelectorInclude.hh"
#include "TDProgram.hh"
#include "base/misc.hh"
#include "arch/isa_traits.hh"
#include "config/the_isa.hh"
#include "mem/ruby/system/System.hh"

#define NO_SPM_ID -1
#define PAGE_SIZE (TheISA::PageBytes)

void
TD::JobDescription::WriteJobSegment(int logicalCFU,
  const std::map<int, int>& logicalToPhysCFUs, PacketBuilder& pb) const
{
  uint32_t transferCount =
    hostProgram->nodeSet[logicalCFU].outgoingEdges.size() +
    hostProgram->nodeSet[logicalCFU].incomingEdges.size();
  uint32_t computeCount = 1;
  uint32_t taskCount = taskEnd - taskStart;
  pb.Write((uint8_t)transferCount);
  pb.Write((uint8_t)computeCount);
  pb.Write((uint32_t)taskStart);
  pb.Write((uint32_t)taskCount);
  hostProgram->nodeSet[logicalCFU].computeDesc.WriteOut(pb);

  for (size_t i = 0; i < hostProgram->nodeSet[logicalCFU].incomingEdges.size(); i++) {
    const TDProgram::Edge& e =
      hostProgram->edgeSet[hostProgram->nodeSet[logicalCFU].incomingEdges[i]];
    int src, dst;

    if (e.from == -1) {
      src = -1;
    } else {
      assert(logicalToPhysCFUs.find(e.from) != logicalToPhysCFUs.end());
      src = logicalToPhysCFUs.find(e.from)->second;
    }

    if (e.to == -1) {
      dst = -1;
    } else {
      assert(logicalToPhysCFUs.find(e.to) != logicalToPhysCFUs.end());
      dst = logicalToPhysCFUs.find(e.to)->second;
    }

    TransferDescription t = e.transferDesc;
    t.srcDevice = src;
    t.dstDevice = dst;
    t.WriteOut(pb);
  }

  for (size_t i = 0; i < hostProgram->nodeSet[logicalCFU].outgoingEdges.size(); i++) {
    const TDProgram::Edge& e =
      hostProgram->edgeSet[hostProgram->nodeSet[logicalCFU].outgoingEdges[i]];
    int src, dst;

    if (e.from == -1) {
      src = -1;
    } else {
      assert(logicalToPhysCFUs.find(e.from) != logicalToPhysCFUs.end());
      src = logicalToPhysCFUs.find(e.from)->second;
    }

    if (e.to == -1) {
      dst = -1;
    } else {
      assert(logicalToPhysCFUs.find(e.to) != logicalToPhysCFUs.end());
      dst = logicalToPhysCFUs.find(e.to)->second;
    }

    TransferDescription t = e.transferDesc;
    t.srcDevice = src;
    t.dstDevice = dst;
    t.WriteOut(pb);
  }
}

std::vector<uint32_t>
TD::ExtractMemoryManifest(const TransferDescription& td, uint32_t task)
{
  std::vector<uint32_t> accSet;
  return accSet;

  if (td.srcDevice == -1) {
    PolyhedralAddresser pa(td.srcBaseAddress, td.srcSize, td.srcStride);
    uint32_t factor = 1;

    for (size_t i = td.srcSplit; i < td.srcSize.size(); i++) {
      factor *= td.srcSize[i];
    }

    for (size_t i = task * factor; i < (task + 1) * factor; i++) {
      assert(i < pa.TotalSize());
      accSet.push_back((uint32_t)pa.GetAddr(i));
    }
  }

  if (td.dstDevice == -1) {
    PolyhedralAddresser pa(td.dstBaseAddress, td.dstSize, td.dstStride);
    uint32_t factor = 1;

    for (size_t i = td.dstSplit; i < td.dstSize.size(); i++) {
      factor *= td.dstSize[i];
    }

    for (size_t i = task * factor; i < (task + 1) * factor; i++) {
      assert(i < pa.TotalSize());
      accSet.push_back((uint32_t)pa.GetAddr(i));
    }
  }

  return accSet;
}

bool
TD::TryAllocateFpga(std::vector<CFUIdentifier>& cfuSet,
  const std::vector<int>& opCodeSet, int minLatency,
  int maxLatency, bool probeAllocate)
{
  if (fpgaPortLoad.size() == 0) {
    return false;
  }

  assert(minLatency <= maxLatency);
  assert(!opCodeSet.empty());
  cfuSet.clear();
  int currentFpgaAreaUsed = 0;
  std::vector<int> recipeSet;

  for (size_t x = 0; x < opCodeSet.size(); x++) {
    int recipe = -1;

    for (size_t i = 0; i < fpgaRecipeSet.size(); i++) {
      if (fpgaRecipeSet[i].opCode == opCodeSet[x] &&
          fpgaRecipeSet[i].ii * fpgaRecipeSet[i].clockMultiplier >= minLatency &&
          fpgaRecipeSet[i].ii * fpgaRecipeSet[i].clockMultiplier <= maxLatency &&
          fpgaRecipeSet[i].area <= fpgaAreaRemaining - currentFpgaAreaUsed) {
        if (recipe == -1 || fpgaRecipeSet[recipe].area > fpgaRecipeSet[i].area) {
          recipe = i;
        }
      }
    }

    if (recipe == -1) {
      return false;
    }

    currentFpgaAreaUsed += fpgaRecipeSet[recipe].area;
    recipeSet.push_back(recipe);
  }

  assert(!addingFpgaAccelerators);
  addingFpgaAccelerators = true; //This is done to skip self-registration of accelerators when an LCA is normally instanced.  This way, FPGA stuff is tracked seperately

  if (!probeAllocate) {
    for (size_t i = 0; i < recipeSet.size(); i++) {
      //find lowest pressure port
      int lowestPressure = 1024 * 1024 * 1024;
      int lowestPressurePort = -1;

      for (std::map<int, int>::iterator it = fpgaPortLoad.begin(); it != fpgaPortLoad.end(); it++) {
        if (it->second < lowestPressure) {
          lowestPressure = it->second;
          lowestPressurePort = it->first;
        }
      }

      assert(lowestPressurePort >= 0);

      if (fpgaCfuPool[lowestPressurePort].size() == 0) {
        int deviceID = fpgaDeviceIDCounter++;
        char nameBuffer[100];
        sprintf(nameBuffer, "lcacc_fpga_%d", deviceID);
        assert(fpgaCfuName.find(deviceID) == fpgaCfuName.end());
        assert(fpgaCfuPortMap.find(deviceID) == fpgaCfuPortMap.end());
        fpgaCfuPortMap[deviceID] = lowestPressurePort;
        fpgaCfuName[deviceID] = nameBuffer;
        //TODO: this is needed for CAMEL
        //TaskDistributor::gem5Interface::CreateAccelerator(nameBuffer, fpgaOpCodeSummary, lowestPressurePort, deviceID);
        fpgaCfuPool[lowestPressurePort].push(deviceID);
      }

      assert(!fpgaCfuPool[lowestPressurePort].empty());
      CFUIdentifier cfu;
      cfu.cfuID = fpgaCfuPool[lowestPressurePort].front();
      cfu.cpuID = lowestPressurePort;
      cfu.acceleratorID = fpgaRecipeSet[recipeSet[i]].opCode;
      cfuSet.push_back(cfu);
      fpgaCfuPool[lowestPressurePort].pop();
      fpgaPortLoad[lowestPressurePort]++;
      fpgaCfuRecipe[cfu.cfuID] = &(fpgaRecipeSet[recipeSet[i]]);
      assert(fpgaCfuName.find(cfu.cfuID) != fpgaCfuName.end());
      fpgaAreaRemaining -= fpgaRecipeSet[recipeSet[i]].area;
      //TODO: this is needed for CAMEL
      //TaskDistributor::gem5Interface::SetAcceleratorTiming(fpgaCfuName[cfu.cfuID], fpgaRecipeSet[recipeSet[i]].ii, fpgaRecipeSet[recipeSet[i]].pipelineDepth, fpgaRecipeSet[recipeSet[i]].clockMultiplier);
      ML_LOG(GetDeviceName(), "BEGIN FPGA Allocation " << cfu.cfuID
        << " Mode " << fpgaRecipeSet[recipeSet[i]].opCode
        << " II " << fpgaRecipeSet[recipeSet[i]].ii
        << " Depth " << fpgaRecipeSet[recipeSet[i]].pipelineDepth
        << " Clock " << fpgaRecipeSet[recipeSet[i]].clockMultiplier);
    }
  }

  assert(addingFpgaAccelerators);
  addingFpgaAccelerators = false;
  return true;
}

void
TD::DeallocateFpga(int id)
{
  assert(fpgaCfuPortMap.find(id) != fpgaCfuPortMap.end());
  assert(fpgaCfuRecipe.find(id) != fpgaCfuRecipe.end());
  ML_LOG(GetDeviceName(), "END FPGA Allocation " << id
    << " Mode " << fpgaCfuRecipe[id]->opCode
    << " II " << fpgaCfuRecipe[id]->ii
    << " Depth " << fpgaCfuRecipe[id]->pipelineDepth
    << " Clock " << fpgaCfuRecipe[id]->clockMultiplier);
  fpgaCfuPool[fpgaCfuPortMap[id]].push(id);
  fpgaAreaRemaining += fpgaCfuRecipe[id]->area;
  fpgaCfuRecipe.erase(id);
}

bool
TD::IsFpgaCfu(int id)
{
  return fpgaCfuPortMap.find(id) != fpgaCfuPortMap.end();
}

void
TD::MsgHandler(int src, const void* packet, unsigned int packetSize)
{
  assert(packetSize >= 8);
  uint32_t* msg = (uint32_t*)packet;
  int command = msg[0];
  uint32_t process = msg[1];

  switch (command) {
  case (TD_CMD_CLEAR_FILTER): {
    ClearCFUFilter(process);
  }
  break;

  case (TD_CMD_ADD_FILTER): {
    AddCFUFilter(process, msg[2]);
  }
  break;

  case (BIN_CMD_ARBITRATE_REQUEST): { //a message requesting arbitration for a buffer size
    uint32_t accTypeCount = msg[2];
    uint32_t pointCount = msg[3];
    std::vector<uint32_t> bufferSize;
    std::vector<uint32_t> performance;
    std::vector<uint32_t> cacheImpact;
    std::vector<uint32_t> accType;
    std::vector<uint32_t> accCount;
    assert(pointCount > 0);
    assert(packetSize == 4 * sizeof(uint32_t) + accTypeCount * (2 * sizeof(uint32_t)) + pointCount * (3 * sizeof(uint32_t)));

    for (uint32_t i = 0; i < accTypeCount; i++) {
      accType.push_back(msg[4 + 2 * i]);
      accCount.push_back(msg[4 + 2 * i + 1]);
    }

    for (uint32_t i = 0; i < pointCount; i++) {
      bufferSize.push_back(msg[4 + accTypeCount * 2 + 3 * i]);
      performance.push_back(msg[4 + accTypeCount * 2 + 3 * i + 1]);
      cacheImpact.push_back(msg[4 + accTypeCount * 2 + 3 * i + 2]);
    }

    uint32_t chosenBufferSize;
    //Do something productive here FIXME
    chosenBufferSize = bufferSize[0];
    assert(selectedBiNSize.find(process) == selectedBiNSize.end());
    selectedBiNSize[process] = bufferSize[0];

    //In the mean time, I'm using the following as a fill-in
    uint32_t buf[3];
    buf[0] = BIN_CMD_ARBITRATE_RESPONSE;
    buf[1] = process;
    buf[2] = chosenBufferSize;
    netPort->SendMessage(src, buf, sizeof(buf));
  }
  break;

  case (TD_CMD_BEGIN_PROGRAM): { //message sent to TD with program description
    assert(packetSize == 9 * sizeof(uint32_t));
    lastKnownCore[process] = src;

    ML_LOG(GetDeviceName(), "command to begin program received from device "
      << src << " for userthread " << process);

    BitConverter bc;
    bc.u32[0] = msg[2];
    bc.u32[1] = msg[3];
    uint64_t logicalAddr = bc.u64[0];
    bc.u32[0] = msg[4];
    bc.u32[1] = msg[5];
    uint64_t physicalAddr = bc.u64[0];

    assert(logicalAddr);
    assert(physicalAddr);

    TaskReadData readData;
    readData.process = process;
    readData.logicalAddr = logicalAddr;
    readData.physicalAddr = physicalAddr;
    readData.size = msg[6];

    if (selectedBiNSize.find(process) != selectedBiNSize.end()) {
      readData.useSharedBuffer = true;
      readData.sharedBufferSize = selectedBiNSize[process];
      selectedBiNSize.erase(process);
    }

    pendingTaskReads.push(readData);
    TaskDistributor::gem5Interface::RegisterCallback(
      StartProgramReadCB::Create(this), deviceDelay);
  }
  break;

  case (LCACC_CMD_TASK_COMPLETED): { //message from lca marking task as completed
    assert(cfuUseMap.find(src) != cfuUseMap.end());
    assert(cfuBufferMap.find(src) != cfuBufferMap.end());
    assert(bufferReferenceCount.find(cfuBufferMap[src]) != bufferReferenceCount.end());
    assert(bufferReferenceCount[cfuBufferMap[src]] > 0);
    assert(patternSelector);
    unsigned int userProcess = cfuUseMap[src];
    cfuUseMap.erase(src);
    bufferReferenceCount[cfuBufferMap[src]]--;

    if (bufferReferenceCount[cfuBufferMap[src]] == 0) {
      if (cfuBufferMap[src] >= 0) {
        FreeBuffer(cfuBufferMap[src], false);
      }

      stalledPrograms.clear();
      bufferReferenceCount.erase(cfuBufferMap[src]);
    }

    cfuBufferMap.erase(src);

    if (!IsFpgaCfu(src)) {
      patternSelector->NotifyDeallocate(src);
      assert(cfuIndexMap.find(src) != cfuIndexMap.end());
      assert(userProcess == systemCfuSet[cfuIndexMap[src]].owningThread);
      systemCfuSet[cfuIndexMap[src]].owningThread = 0;
    } else {
      DeallocateFpga(src);
    }

    assert(pendingJobSet.find(userProcess) != pendingJobSet.end());
    bool allDone = false;

    if (pendingJobSet[userProcess].size() == 0) {
      //whole task is done
      allDone = true;

      for (std::map<int, unsigned int>::iterator it = cfuUseMap.begin(); it != cfuUseMap.end(); it++) {
        if (it->second == userProcess) {
          allDone = false;
        }
      }
    }

    if (allDone) {
      assert(lastKnownCore.find(userProcess) != lastKnownCore.end());
      assert(requiredBufferSize.find(userProcess) != requiredBufferSize.end());

      ML_LOG(GetDeviceName(), "END Program Execute for userthread "
        << userProcess);

      TaskDistributor::gem5Interface::RegisterCallback(
        NotifyHostCB::Create(this, userProcess), deviceDelay);

      assert(programSet.find(userProcess) != programSet.end());
      delete programSet[userProcess];
      pendingJobSet.erase(userProcess);
      programSet.erase(userProcess);
      requiredBufferSize.erase(userProcess);

      if (stalledTaskReads.find(userProcess) != stalledTaskReads.end()) {
        assert(!stalledTaskReads[userProcess].empty());
        TaskReadData& trd = stalledTaskReads[userProcess].front();
        ExecuteProgram(trd);
        stalledTaskReads[userProcess].pop();
        delete trd.loadedPacket;

        if (stalledTaskReads[userProcess].empty()) {
          stalledTaskReads.erase(userProcess);
        }
      }
    }

    TryNewAllocation();
  }
  break;

  case (LCACC_CMD_TLB_MISS): { //tlb miss from lca
    assert(process == myThreadID);
    assert(cfuUseMap.find(src) != cfuUseMap.end());
    assert(lastKnownCore.find(cfuUseMap[src]) != lastKnownCore.end());
    int core = lastKnownCore[cfuUseMap[src]];
    BitConverter bc;
    bc.u32[0] = msg[2];
    bc.u32[1] = msg[3];
    uint64_t logicalAddr = bc.u64[0];
    unsigned int userProcess = cfuUseMap[src];
    uint64_t logicalPage = (logicalAddr / PAGE_SIZE) * PAGE_SIZE;
    // ML_LOG(GetDeviceName(), "BEGIN TLB Service " << logicalPage << " " << src);
    assert(logicalPage);

    if (localTlb[userProcess].find(logicalPage) != localTlb[userProcess].end()) {
      // service locally
      uint32_t outMsg[9];
      outMsg[0] = LCACC_CMD_TLB_SERVICE;
      outMsg[1] = myThreadID;
      bc.u64[0] = logicalPage;
      outMsg[2] = bc.u32[0];
      outMsg[3] = bc.u32[1];
      assert(localTlb[userProcess][logicalPage]);
      assert(localTlb[userProcess][logicalPage] % PAGE_SIZE == 0);
      bc.u64[0] = localTlb[userProcess][logicalPage];
      outMsg[4] = bc.u32[0];
      outMsg[5] = bc.u32[1];
      outMsg[6] = outMsg[7] = outMsg[8] = 0;
      netPort->SendMessage(src, outMsg, sizeof(outMsg));
      // ML_LOG(GetDeviceName(), "END TLB Service " << logicalPage << " " << src);
    } else if (cfuTlbMisses[userProcess].find(logicalPage) != cfuTlbMisses[userProcess].end()) {
      // redundant miss
      cfuTlbMisses[userProcess][logicalPage].push_back(src);
    } else {
      // actual new miss
      cfuTlbMisses[userProcess][logicalPage].push_back(src);
      uint32_t outMsg[4];
      outMsg[0] = LCACC_CMD_TLB_MISS;
      outMsg[1] = userProcess;
      bc.u64[0] = logicalPage;
      outMsg[2] = bc.u32[0];
      outMsg[3] = bc.u32[1];
      // ML_LOG(GetDeviceName(), "BEGIN TLB miss " << logicalPage);
      netPort->SendMessage(core, outMsg, sizeof(outMsg));
    }
  }
  break;

  case (LCACC_CMD_TLB_SERVICE): { //tlb response from core
    lastKnownCore[process] = src;

    BitConverter bc;
    bc.u32[0] = msg[2];
    bc.u32[1] = msg[3];
    uint64_t logicalAddr = bc.u64[0];
    bc.u32[0] = msg[4];
    bc.u32[1] = msg[5];
    uint64_t physicalAddr = bc.u64[0];

    assert(logicalAddr % PAGE_SIZE == 0);
    uint64_t logicalPage = (logicalAddr / PAGE_SIZE) * PAGE_SIZE;
    assert(physicalAddr % PAGE_SIZE == 0);
    uint64_t physicalPage = (physicalAddr / PAGE_SIZE) * PAGE_SIZE;
    assert(physicalPage);
    assert(logicalPage);

    localTlb[process][logicalPage] = physicalPage;

    if (programSet.find(process) == programSet.end()) {
      // no program loaded, thus the miss must be on loading the program
      // ML_LOG(GetDeviceName(), "END Personal TLB miss " << msg[2]);
      ML_LOG(GetDeviceName(), "TLB miss was serviced for program loading");
      dma->AddTLBEntry(logicalAddr, physicalPage);
    } else {
      // ML_LOG(GetDeviceName(), "END TLB miss " << logicalPage);
      assert(cfuTlbMisses.find(process) != cfuTlbMisses.end());
      assert(cfuTlbMisses[process].find(logicalPage) != cfuTlbMisses[process].end());
      assert(logicalPage % PAGE_SIZE == 0);
      assert(physicalPage % PAGE_SIZE == 0);
      uint32_t outMsg[9];
      outMsg[0] = LCACC_CMD_TLB_SERVICE;
      outMsg[1] = myThreadID;
      BitConverter bc;
      bc.u64[0] = logicalPage;
      outMsg[2] = bc.u32[0];
      outMsg[3] = bc.u32[1];
      bc.u64[0] = physicalPage;
      outMsg[4] = bc.u32[0];
      outMsg[5] = bc.u32[1];
      outMsg[6] = outMsg[7] = outMsg[8] = 0;

      for (size_t i = 0; i < cfuTlbMisses[process][logicalPage].size(); i++) {
        if (cfuTlbMisses[process][logicalPage][i] != 0) {
          // ML_LOG(GetDeviceName(), "END TLB Service " << logicalPage << " " << cfuTlbMisses[process][logicalPage][i]);
          netPort->SendMessage(cfuTlbMisses[process][logicalPage][i], outMsg, sizeof(outMsg));
        }
      }

      cfuTlbMisses[process].erase(logicalPage);
    }
  }
  break;

  case (LCACC_CMD_CANCEL_TASK): {
    assert(0);//not yet supported
  }
  break;

  case (DMA_MEMORY_REQUEST):
  case (DMA_MEMORY_RESPONSE):
    break;

  default:
    ML_LOG(GetDeviceName(), "error receiving unsupported message");
    assert(0);
  }
}

void TD::NotifyHost(uint32_t userProcess)
{
  uint32_t endMsg[2];
  endMsg[0] = LCACC_CMD_TASK_COMPLETED;
  endMsg[1] = userProcess;

  netPort->SendMessage(lastKnownCore[userProcess], endMsg, sizeof(endMsg));
}

void TD::ExecuteProgram(TaskReadData& trd)
{
  assert(programSet.find(trd.process) == programSet.end());
  ML_LOG(GetDeviceName(), "BEGIN Program Execute for userthread "
    << trd.process);
  assert(trd.loadedPacket != NULL);
  ML_LOG(GetDeviceName(), "parsing program description");
  PacketReader& pr = *(trd.loadedPacket);
  TDProgram* tdp = new TDProgram();
  tdp->ReadIn(pr);
  ML_LOG(GetDeviceName(), "parsing done");

  if (tdp->taskGrain == 0) {
    ML_LOG(GetDeviceName(), "setting task grain to default of "
      << defaultTaskGrain);
    tdp->taskGrain = defaultTaskGrain;
  } else {
    ML_LOG(GetDeviceName(), "task grain : " << tdp->taskGrain);
  }

  requiredBufferSize[trd.process] = trd.sharedBufferSize;
  assert(pr.SizeRemaining() < 4);
  uint32_t totalJobId = (tdp->taskCount + (tdp->taskGrain - 1)) / tdp->taskGrain;

  ML_LOG(GetDeviceName(), "total task count : " << tdp->taskCount);
  ML_LOG(GetDeviceName(), "total job count : " << totalJobId);
  assert(totalJobId > 0);

  for (uint32_t i = 0; i < totalJobId; i++) {
    JobDescription j;
    j.hostProgram = tdp;
    j.taskStart = i * tdp->taskGrain;
    j.taskEnd = std::min((i + 1) * tdp->taskGrain, tdp->taskCount);
    pendingJobSet[trd.process].push(j);
  }

  for (uint32_t task = 0; task < tdp->taskCount; task++) {
    for (size_t edge = 0; edge < tdp->edgeSet.size(); edge++) {
      TransferDescription& td = tdp->edgeSet[edge].transferDesc;
      std::vector<uint32_t> memAddrs = ExtractMemoryManifest(td, task);

      for (size_t i = 0; i < memAddrs.size(); i++) {
        uint32_t logicalPage = (memAddrs[i] / PAGE_SIZE) * PAGE_SIZE;
        assert(logicalPage);

        if (localTlb[trd.process].find(logicalPage) == localTlb[trd.process].end() && cfuTlbMisses[trd.process].find(logicalPage) == cfuTlbMisses[trd.process].end()) {
          cfuTlbMisses[trd.process][logicalPage].push_back(0);
          uint32_t outMsg[4];
          outMsg[0] = LCACC_CMD_TLB_MISS;
          outMsg[1] = trd.process;
          BitConverter bc_logical;
          bc_logical.u64[0] = logicalPage;
          outMsg[2] = bc_logical.u32[0];
          outMsg[3] = bc_logical.u32[1];
          // ML_LOG(GetDeviceName(), "BEGIN TLB miss " << logicalPage);
          netPort->SendMessage(lastKnownCore[trd.process], outMsg, sizeof(outMsg));
        }
      }
    }
  }

  programSet[trd.process] = tdp;
  TryNewAllocation();
}

void
TD::ReadProgramDone()
{
  assert(!pendingTaskReads.empty());
  TaskReadData& trd = pendingTaskReads.front();
  ML_LOG(GetDeviceName(), "END Program Read for userthread " << trd.process);
  assert(trd.loadedPacket == NULL);
  uint8_t* buf = new uint8_t[trd.size];

  spm->Read(0, trd.size, buf);

  trd.loadedPacket = new PacketReader(buf, trd.size);
  delete [] buf;

  trd.loadedPacket->SetEndianSwap(false);

  if (programSet.find(trd.process) == programSet.end()) {
    // no program is executing, start the program
    ExecuteProgram(trd);
    delete trd.loadedPacket;
  } else {
    // We are already running the program, so queue it up, and we'll get it later
    ML_LOG(GetDeviceName(), "adding task to pending list");
    stalledTaskReads[trd.process].push(trd);
  }

  pendingTaskReads.pop();
  currentlyReadingTask = false;
  StartProgramRead();
}

void
TD::StartProgramRead()
{
  if (pendingTaskReads.size() == 0 || currentlyReadingTask) {
    return;
  }

  currentlyReadingTask = true;

  TaskReadData& trd = pendingTaskReads.front();
  ML_LOG(GetDeviceName(), "BEGIN Program Read for userthread "
    << trd.process);

  std::vector<unsigned int> size;
  std::vector<int> stride;
  size.push_back(trd.size);
  stride.push_back(1);

  dma->FlushTLB();
  dma->AddTLBEntry(trd.logicalAddr, trd.physicalAddr);

  dma->BeginTransfer(NO_SPM_ID, trd.logicalAddr, size, stride, spm->GetID(),
    0, size, stride, 1, ReadProgramDoneCB::Create(this));
}

void
TD::HandleTLBMiss(uint64_t addr)
{
  uint32_t msg[4];
  assert(!pendingTaskReads.empty());

  TaskReadData& trd = pendingTaskReads.front();

  assert(lastKnownCore.find(trd.process) != lastKnownCore.end());

  msg[0] = LCACC_CMD_TLB_MISS;
  msg[1] = trd.process;
  BitConverter bc;
  bc.u64[0] = (addr / PAGE_SIZE) * PAGE_SIZE;
  msg[2] = bc.u32[0];
  msg[3] = bc.u32[1];
  ML_LOG(GetDeviceName(), "TLB miss on TD DMA vaddr 0x"
    << std::hex << bc.u64[0]);
  netPort->SendMessage(lastKnownCore[trd.process], msg, sizeof(msg));
}

void
TD::HandleAccessViolation(uint64_t)
{
  std::cerr << "[ERROR] Access violation picked up in TD." << std::endl;
  assert(0);
}

void
TD::AllocateMore()
{
  assert(currentlyAllocating);
  currentlyAllocating = false;
  TryNewAllocation();
}

void
TD::FreeBuffer(int bufferID, bool silent)
{
  if (!silent) {
    assert(bufferNodes.find(bufferID) != bufferNodes.end());
    TaskDistributor::gem5Interface::FreeBuffer(bufferNodes[bufferID],
      bufferID, BufferFreeCompleteCB::Create(this, bufferID));
  } else {
    BufferFreeComplete(bufferID);
  }
}

void
TD::BufferFreeComplete(int bufferID)
{
  assert(bufferNodes.find(bufferID) != bufferNodes.end());
  freeBuffers[bufferNodes[bufferID]].push_back(bufferID);
  bufferNodes.erase(bufferID);
}

int
TD::AcquireBufferID(uint32_t node)
{
  if (freeBuffers[node].size() == 0) {
    freeBuffers[node].push_back(
      TaskDistributor::gem5Interface::GetBufferID(node));
  }

  int ret = freeBuffers[node].back();
  freeBuffers[node].pop_back();
  assert(bufferNodes.find(ret) == bufferNodes.end());
  bufferNodes[ret] = node;
  return ret;
}

void
TD::AllocateBufferComplete(bool success, int bufferSize, int bufferID,
  std::vector<CFUIdentifier> selected, unsigned int process)
{
  assert(busyPrograms.find(process) != busyPrograms.end());
  busyPrograms.erase(process);
  assert(bufferReferenceCount.find(bufferID) != bufferReferenceCount.end());
  assert(bufferID == -1 || bufferReferenceCount[bufferID] == selected.size());

  for (size_t i = 0; i < selected.size(); i++) {
    assert(cfuUseMap.find(selected[i].cfuID) != cfuUseMap.end());
    assert(cfuUseMap[selected[i].cfuID] == process);
    assert(cfuBufferMap.find(selected[i].cfuID) != cfuBufferMap.end());
    assert(cfuBufferMap[selected[i].cfuID] == bufferID);
  }

  if (!success) {
    assert(bufferID >= 0);

    for (size_t i = 0; i < selected.size(); i++) {
      cfuUseMap.erase(selected[i].cfuID);
      cfuBufferMap.erase(selected[i].cfuID);

      if (!IsFpgaCfu(selected[i].cfuID)) {
        systemCfuSet[cfuIndexMap[selected[i].cfuID]].owningThread = 0;
        assert(patternSelector);
        patternSelector->NotifyDeallocate(selected[i].cfuID);
      } else {
        DeallocateFpga(selected[i].cfuID);
      }
    }

    bufferReferenceCount.erase(bufferID);
    FreeBuffer(bufferID, true);
    stalledPrograms.insert(process);

    if (cfuUseMap.empty()) {
      ML_LOG(GetDeviceName(), "this buffer can NEVER be allocated");
      assert(!cfuUseMap.empty());
    }

    return;
  }

  std::map<int, int> cfuMapping;

  for (size_t x = 0; x < selected.size(); x++) {
    cfuMapping[(int)x] = selected[x].cfuID;
  }

  assert(programSet.find(process) != programSet.end());
  assert(pendingJobSet.find(process) != pendingJobSet.end());
  JobDescription& job = pendingJobSet[process].front();

  for (size_t x = 0; x < selected.size(); x++) {
    PacketBuilder pb;
    pb.Write((int32_t)LCACC_CMD_BEGIN_EMBEDDED_TASK_SIGNATURE);
    pb.Write((int32_t)0);
    pb.Write((int32_t)bufferID);
    pb.Write((int32_t)bufferSize);

    ML_LOG(GetDeviceName(), "configuring LCAcc " << selected[x].cfuID);

    job.WriteJobSegment((int)x, cfuMapping, pb);
    std::vector<uint32_t> memoryManifest;

    for (size_t i = 0; i < job.hostProgram->edgeSet.size(); i++) {
      if (job.hostProgram->edgeSet[i].from == (int)x || job.hostProgram->edgeSet[i].to == (int)x) {
        for (size_t j = job.taskStart; j < job.taskEnd; j++) {
          std::vector<uint32_t> transAccesses = ExtractMemoryManifest(job.hostProgram->edgeSet[i].transferDesc, j);

          for (size_t z = 0; z < transAccesses.size(); z++) {
            memoryManifest.push_back(transAccesses[z]);
          }
        }
      }
    }

    std::map<uint32_t, uint64_t> privateTlb;

    for (size_t i = 0; i < memoryManifest.size(); i++) {
      uint32_t logicalPage = (memoryManifest[i] / PAGE_SIZE) * PAGE_SIZE;

      if (privateTlb.find(logicalPage) == privateTlb.end() && localTlb[process].find(logicalPage) != localTlb[process].end()) {
        privateTlb[logicalPage] = localTlb[process][logicalPage];
      }
    }

    if (privateTlb.size() > 0) {
      pb.Write((uint32_t)privateTlb.size());

      for (std::map<uint32_t, uint64_t>::iterator it = privateTlb.begin(); it != privateTlb.end(); it++) {
        pb.Write(it->first);
        pb.Write(it->second);
      }
    }

    netPort->SendMessage(selected[x].cfuID, pb.GetBuffer(), pb.GetBufferSize(), patternSelector->GetLastCalculationDelay());
  }

  pendingJobSet[process].pop();
  TryNewAllocation();
}

void
TD::TryNewAllocation()
{
  if (currentlyAllocating || pendingJobSet.empty()) {
    return;
  }

  static size_t selectorBias = 0;
  bool allEmpty = true;
  unsigned int delay = 1;
  bool foundAny = false;

  for (size_t i = 0; i < pendingJobSet.size(); i++) {
    std::map<unsigned int, std::queue<JobDescription> >::iterator it = pendingJobSet.begin();

    for (size_t x = 0; x < (i + selectorBias) % pendingJobSet.size(); x++) {
      it++;
    }

    if (busyPrograms.find(it->first) != busyPrograms.end() || stalledPrograms.find(it->first) != stalledPrograms.end()) {
      continue;
    }

    if (!it->second.empty()) {
      assert(programSet.find(it->first) != programSet.end());
      TDProgram& prog = *(programSet[it->first]);
      assert(lastKnownCore.find(it->first) != lastKnownCore.end());
      int core = lastKnownCore[it->first];
      allEmpty = false;
      assert(patternSelector);
      std::vector<CFUIdentifier> availableSet;

      for (size_t x = 0; x < systemCfuSet.size(); x++) {
        if (cfuUseMap.find(systemCfuSet[x].cfuID) == cfuUseMap.end() && (cfuUseFilter.find(it->first) == cfuUseFilter.end() || cfuUseFilter[it->first].find(systemCfuSet[x].cfuID) != cfuUseFilter[it->first].end())) {
          availableSet.push_back(systemCfuSet[x]);
        }
      }

      std::vector<CFUIdentifier> selected;
      std::vector<int> cfuNeededSet;
      bool placedAll = false;

      if (patternSelector->FindAssignment(core, prog, cfusPerIsland, systemCfuSet, availableSet, selected, cfuNeededSet, false)) {
        assert(cfuNeededSet.empty());
        placedAll = true;
      } else {
        assert(!cfuNeededSet.empty());
        std::vector<CFUIdentifier> fpgaAssignedSet;
        int maxAsicLatency = 1;

        for (size_t x = 0; x < prog.nodeSet.size(); x++) {
          int opcode = prog.nodeSet[x].computeDesc.opcode;
          unsigned int ii, pipelineDepth, mult;
          TaskDistributor::gem5Interface::GetOpModeTiming(opcode, &ii, &pipelineDepth, &mult);
          maxAsicLatency = ((unsigned int)maxAsicLatency > ii * mult) ? maxAsicLatency : (ii * mult);
        }//*/

        //fatal ("Cannot currently access timing information of fpga accelerator.\nSee src/modules/TaskDistributor/TD.cc");
        int maxFpgaLatency = 1;

        for (size_t x = 0; x < fpgaRecipeSet.size(); x++) {
          maxFpgaLatency = (maxFpgaLatency > fpgaRecipeSet[x].ii * fpgaRecipeSet[x].clockMultiplier) ? maxFpgaLatency : (fpgaRecipeSet[x].ii * fpgaRecipeSet[x].clockMultiplier);
        }

        for (size_t x = 0; x > fpgaOpCodeSummary.size(); x++) {
          bool foundSlower = false;

          for (size_t y = 0; y < fpgaRecipeSet.size() && !foundSlower; y++) {
            if (fpgaRecipeSet[y].opCode == fpgaOpCodeSummary[x] && fpgaRecipeSet[y].ii * fpgaRecipeSet[y].clockMultiplier >= maxAsicLatency) {
              foundSlower = true;
            }
          }

          if (!foundSlower) {
            ML_LOG(GetDeviceName(), "asic speed " << maxAsicLatency
              << "  for op code " << fpgaOpCodeSummary[x]);
            assert(foundSlower);
          }
        }

        for (int latency = maxAsicLatency; latency <= maxFpgaLatency && !placedAll; latency++) {
          if (TryAllocateFpga(fpgaAssignedSet, cfuNeededSet, maxAsicLatency, latency, false)) {
            size_t y = 0;//index into 'fpga assigned set'
            assert(patternSelector->FindAssignment(core, prog, cfusPerIsland, systemCfuSet, availableSet, selected, cfuNeededSet, true));

            for (size_t x = 0; x < selected.size(); x++) {
              if (selected[x].cfuID == (unsigned int) - 1) {
                assert(y < fpgaAssignedSet.size());
                selected[x] = fpgaAssignedSet[y];
                y++;
              }
            }

            assert(y == fpgaAssignedSet.size());
            placedAll = true;
          }
        }
      }

      if (placedAll) {
        assert(!selected.empty());
        selectorBias++;
        foundAny = true;
        busyPrograms.insert(it->first);
        int bufferID = -1;
        int bufferSize = requiredBufferSize[it->first];

        if (bufferSize) {
          bufferID = AcquireBufferID(selected[0].cpuID);
          assert(bufferReferenceCount.find(bufferID) == bufferReferenceCount.end());
          bufferReferenceCount[bufferID] = 0;
        }

        bufferReferenceCount[bufferID] += selected.size();

        for (size_t x = 0; x < selected.size(); x++) {
          cfuUseMap[selected[x].cfuID] = it->first;
          assert(cfuBufferMap.find(selected[x].cfuID) == cfuBufferMap.end());
          cfuBufferMap[selected[x].cfuID] = bufferID;

          if (!IsFpgaCfu(selected[x].cfuID)) {
            assert(cfuIndexMap.find(selected[x].cfuID) != cfuIndexMap.end());
            systemCfuSet[cfuIndexMap[selected[x].cfuID]].owningThread = it->first;
          }
        }

        if (bufferID >= 0) {
          assert(bufferSize > 0);
          TaskDistributor::gem5Interface::TryBufferAllocate(selected[0].cpuID, bufferID, bufferSize, AllocateBufferCompleteCB::Create(this, bufferSize, bufferID, selected, it->first));
        } else {
          //spoof successful allocation
          assert(bufferSize == 0);
          TaskDistributor::gem5Interface::RegisterCallback(AllocateBufferCompleteConstCB::Create(this, true, bufferSize, bufferID, selected, it->first), 0);
        }
      } else if (cfuUseMap.empty()) {
        //the requested configuration can never be satisfied.
        std::cout << "This request can never, ever be satisfied in this configuration.  Error UNACCEL" << std::endl;
        std::cout << "Needs :";

        for (size_t x = 0; x < prog.nodeSet.size(); x++) {
          std::cout << prog.nodeSet[x].computeDesc.opcode << " ";
        }

        std::cout << std::endl;
        std::cout << "Has :";

        for (size_t x = 0; x < availableSet.size(); x++) {
          std::cout << availableSet[x].acceleratorID << " ";
        }

        std::cout << std::endl;
        assert(0);
      }

      delay = patternSelector->GetLastCalculationDelay();
      break;
    }
  }

  if (!allEmpty && foundAny) {
    currentlyAllocating = true;
    TaskDistributor::gem5Interface::RegisterCallback(AllocateMoreCB::Create(this), delay);
  }
}

void
TD::AddNetworkInterface(NetworkInterface* port)
{
  assert(netPort == NULL);
  netPort = port;
}

void
TD::AddCFU(int id, int cpu, int type)
{
  if (!addingFpgaAccelerators) {
    CFUIdentifier c;
    assert(cfuIndexMap.find(id) == cfuIndexMap.end());
    c.cfuID = id;
    c.cpuID = cpu;
    c.acceleratorID = type;
    cfuIndexMap[id] = (int) systemCfuSet.size();
    systemCfuSet.push_back(c);
  }
}

void
TD::RemoveCFU(int id)
{
  assert(cfuBufferMap.find(id) == cfuBufferMap.end());
  assert(cfuUseMap.find(id) == cfuUseMap.end());
  assert(cfuIndexMap.find(id) != cfuIndexMap.end());
  std::map<int, int> newIndexMap;
  systemCfuSet.erase(systemCfuSet.begin() + cfuIndexMap[id]);

  for (size_t i = 0; i < systemCfuSet.size(); i++) {
    newIndexMap[systemCfuSet[i].cfuID] = i;
  }

  cfuIndexMap = newIndexMap;
}

void
TD::SetSelector(const std::string& selectorName)
{
  assert(patternSelector);
  delete patternSelector;
#define PS_LISTING(x, y) if(selectorName == x) patternSelector = new y();
#include "PatternSelectorListing.hh"
#undef PS_LISTING
  assert(patternSelector);
  ML_LOG(GetDeviceName(), "set pattern selector to " << selectorName);
}

void
TD::SetCFUAllocationPerIsland(int cfuCount)
{
  assert(cfuCount > 0);
  cfusPerIsland = cfuCount;
}

void
TD::SetTaskGrain(int grain)
{
  defaultTaskGrain = grain;
}

void
TD::SetFpgaArea(int area)
{
  assert(area >= 0);
  fpgaAreaRemaining = area;
}

void
TD::AddFpgaPort(int networkPort)
{
  assert(fpgaPortLoad.find(networkPort) == fpgaPortLoad.end());
  fpgaPortLoad[networkPort] = 0;
}

void
TD::AddFpgaRecipe(int opCode, int area, int ii, int pipelineDepth, int cycleMult)
{
  assert(ii > 0);
  assert(pipelineDepth > 0);
  assert(cycleMult > 0);
  assert(area > 0);
  FpgaRecipe rec;
  rec.opCode = opCode;
  rec.ii = ii;
  rec.pipelineDepth = pipelineDepth;
  rec.clockMultiplier = cycleMult;
  rec.area = area;
  fpgaRecipeSet.push_back(rec);
  bool unknown = true;

  for (size_t i = 0; i < fpgaOpCodeSummary.size() && unknown; i++) {
    if (fpgaOpCodeSummary[i] == opCode) {
      unknown = false;
    }
  }

  if (unknown) {
    fpgaOpCodeSummary.push_back(opCode);
  }
}

void
TD::ClearCFUFilter(uint32_t process)
{
  assert(cfuUseFilter.find(process) != cfuUseFilter.end());
  cfuUseFilter.erase(process);
}

void
TD::AddCFUFilter(uint32_t process, int cfuID)
{
  assert(cfuUseFilter[process].find(cfuID) == cfuUseFilter[process].end());
  cfuUseFilter[process].insert(cfuID);
}

void
TD::Initialize()
{
  assert(netPort);
  spm = new SPMInterface(netPort->GetNetworkPort(), 1024 * 1024 * 8);
  dma = new DMAController(netPort, spm, HandleTLBMissCB::Create(this),
    HandleAccessViolationCB::Create(this));
  netPort->RegisterRecvHandler(MsgHandlerCB::Create(this));
}

TD::TD()
{
  spm = NULL;
  netPort = NULL;
  dma = NULL;
  currentlyReadingTask = false;
  currentlyAllocating = false;
  patternSelector = new DumbSelector();
  myThreadID = 0;
  defaultTaskGrain = 1;
  cfusPerIsland = 99999999;
  fpgaAreaRemaining = 0;
  fpgaDeviceIDCounter = 15000;
  addingFpgaAccelerators = false;
  deviceDelay = RubySystem::getDeviceDelay();
}
