#include <cassert>
#include <stdint.h>
#include <iostream>
#include "NetworkInterrupts.hh"
#define SIM_ENHANCE
#define SIM_NET_PORTS
#define SIM_MEMORY
//#define SIM_VISUAL_TRACE
#define BUFFER_IN_CACHE
#include "../Common/mf_api.hh"
#include "../LCAcc/LCAccCommandListing.hh"
#include "../Common/BitConverter.hh"
#include "../TLBHack/TLBHack.hh"
#include "../../sim/system.hh"
#include "../../mem/ruby/system/System.hh"
#include "modules/Synchronize/Synchronize.hh"
#include "sim/pseudo_inst.hh"
#include "lwi.hh"
#include "modules/LCAcc/gem5Interface.hh"
#include "arch/vtophys.hh"
#include "debug/LWI.hh"

#define MAX_ISR_BUFFER_SIZE 128

uint64_t GetSystemTime()
{
        return uint64_t(g_system_ptr->curCycle());
}

std::vector<std::queue<CBContainer> > cycleCBRing;
bool localCBsForCycle(Cycles current_cycle) {
  uint64_t m_current_cycle = uint64_t(current_cycle);
  size_t oldIndex = (m_current_cycle - 1) % cycleCBRing.size();
  size_t index = m_current_cycle % cycleCBRing.size();
  if(!cycleCBRing[oldIndex].empty()) {
    std::cout << "[assertion fail]oldIndex: " << oldIndex << ", currentIndex: " << index << std::endl;
  }
  assert(cycleCBRing[oldIndex].empty());
  return !(cycleCBRing[index].empty());
}
std::queue<CBContainer>& getCurrentCycleQueue(Cycles current_cycle) {
  uint64_t m_current_cycle = uint64_t(current_cycle);
  assert(localCBsForCycle(current_cycle));
  size_t index = m_current_cycle % cycleCBRing.size();
  return cycleCBRing[index];
}
void retireCBsForCycle(Cycles current_cycle) {
  assert(!localCBsForCycle(current_cycle));
}
void scheduleCB(void (*cb)(void*), void* args, uint64_t delta) {
  uint64_t m_global_cycles = uint64_t(g_system_ptr->curCycle());
  assert(cb);
  if(delta + 10 >= cycleCBRing.size()) {
    std::cout << "Expanding event ring from " << cycleCBRing.size() << " to " << delta * 2 << std::endl;
    std::vector<std::queue<CBContainer> > oldSchedule = cycleCBRing;
    cycleCBRing.clear();
    for(size_t i = 0; i < delta * 2; i++)
    {
      cycleCBRing.push_back(std::queue<CBContainer>());
    }
    assert(cycleCBRing.size() > oldSchedule.size());
    for(size_t i = 0; i < oldSchedule.size(); i++)
    {
      cycleCBRing[(m_global_cycles + i) % cycleCBRing.size()] = oldSchedule[(m_global_cycles + i) % oldSchedule.size()];
    }
  }
  size_t index = (m_global_cycles + delta) % cycleCBRing.size();
  cycleCBRing[index].push(CBContainer(cb, args));
}

TLBHackInterface* tlbHackInterface = g_TLBHack_interface;
std::map<int, NetworkInterrupts*> NetworkInterrupts::cpuMap;
std::map<int, std::vector<int> > NetworkInterrupts::pendingReservation;
std::map<int, std::vector<NetworkInterrupts::BiNPerformancePoint> > NetworkInterrupts::BiNCurveInfo; //key thread, second key lcacc
std::map<int, std::vector<NetworkInterrupts::AcceleratorDeclaration> > NetworkInterrupts::accDeclInfo;
void NetworkInterrupts::TryRaise(int thread)
{
	if(!pendingMsgs[thread].empty())
	{
		assert(pendingMsgs[thread].top().thread == thread);
		lwInt_ifc_t* lwi = g_lwInt_interface;
		if(lwi->isReady(thread))
		{
			Msg m = pendingMsgs[thread].top();
			std::cout << "Raising interrupt from " << m.lcacc << " for thread " << thread << " with size " << m.packet.size() << " and payload ";
			uint32_t* ptr = (uint32_t*)&(m.packet[0]);
			for(unsigned int i = 0; i < m.packet.size() / 4; i++)
			{
				std::cout << "[" << ptr[i] << "]";
			}
			std::cout << std::endl;
			assert(m.packet.size() <= MAX_ISR_BUFFER_SIZE);
			lwi->raiseLightWeightInt(m.thread, &(m.packet[0]), m.packet.size(), m.lcacc);
			pendingMsgs[thread].pop();
		}
		EnqueueEvent(TryRaiseCB::Create(this, thread), 1);		
	}
}
int NetworkInterrupts::CalcPriority(int source, int threadID, const std::vector<uint8_t>& packet)
{
	if(source >= 1000 && source <= 1031)
	{
		return 100;
	}
	return 0;
}
NetworkInterrupts* NetworkInterrupts::LookupNIByCpu(int cpu)
{
	if(cpuMap.find(cpu) != cpuMap.end())
	{
		return cpuMap[cpu];
	}
	return NULL;
}
NetworkInterrupts::NetworkInterrupts(NetworkInterruptHandle* x)
{
	assert(x);
	nih = x;
	if(x->attachedCPU)
	{
  	        warn("NetworkInterrupts attachedCPU = %d\n", x->attachedCPU->threadId());
		//Zhenman: Currently we allow multiple ports/devices binded with one CPU core
	        //assert(cpuMap.find(x->attachedCPU) == cpuMap.end());
	        cpuMap[x->procID] = this;
		x->snpi = g_networkPort_interface;
		x->snpi->BindDeviceToPort(x->portID, x->deviceID);
		x->snpi->RegisterRecvHandlerOnDevice(x->deviceID, HandleNetworkMessage, RecvMessageCB::Create(this));
		warn("bind CPU device %d to port %d\n", x->deviceID, x->portID);
	}
}
NetworkInterrupts::~NetworkInterrupts()
{
        assert(nih);
        assert(cpuMap.find(nih->procID) != cpuMap.end());
        assert(cpuMap[nih->procID] == this);
        cpuMap.erase(nih->procID);
}
int NetworkInterrupts::GetSignal(int thread)
{
	if(pendingSignals.find(thread) != pendingSignals.end())
	{
		assert(!pendingSignals[thread].empty());
		int ret = pendingSignals[thread].front();
		pendingSignals[thread].pop();
		if(pendingSignals[thread].empty())
		{
			pendingSignals.erase(thread);
		}
		return ret;
	}
	return 0;
}
void NetworkInterrupts::RaiseInterrupt(int source, int threadID, const void* buffer, int bufferSize)
{
	assert(buffer);
	assert(bufferSize <= MAX_ISR_BUFFER_SIZE);
	bool mustTryRaise = pendingMsgs[threadID].empty();
	Msg m;
	const uint8_t* srcBuffer = (const uint8_t*) buffer;
	for(int i = 0; i < bufferSize; i++)
	{
		m.packet.push_back(srcBuffer[i]);
	}
	m.lcacc = source;
	m.thread = threadID;
	m.priority = CalcPriority(source, threadID, m.packet);
	pendingMsgs[threadID].push(m);
	if(mustTryRaise)
	{
		TryRaise(threadID);
	}
}
void NetworkInterrupts::PutOffInterrupt(int source, int threadID, const void* buffer, int bufferSize, int delay)
{
        CallbackBase* cb = RaiseInterruptCB::Create(this, source, threadID, buffer, bufferSize);
        EnqueueEvent(cb, delay);
}
void NetworkInterrupts::RecvMessage(int source, const char* buffer, int size)
{
        tlbHackInterface = g_TLBHack_interface;
        assert((size_t)size >= sizeof(int32_t));
        const int32_t* args = (const int*) buffer;
	std::cout << "[" << nih->deviceID << "] Message recieved from " << source << " of command type " << args[0] << "   Payload :";
	for(int i = 1; i < size / 4; i++)
	{
		std::cout << "[" << args[i] << "]";
	}
	std::cout << std::endl;
	if(args[0] == LCA_RAISE_SIGNAL)
	{
		assert(size == 3 * sizeof(int32_t));
		int thread = args[1];
		int signal = args[2];
		assert(signal != 0);
		pendingSignals[thread].push(signal);
	}
	else
	{
		if(args[0] == LCACC_CMD_TLB_MISS)
		{
		        assert(size >= 9);
			uint64_t pAddr;
			int thread = args[1];
			BitConverter bc_vAddr;
			bc_vAddr.u32[0] = args[2];
			bc_vAddr.u32[1] = args[3];
			uint64_t vAddr = bc_vAddr.u64[0];
			assert(vAddr);
			System *m5_system = *(System::systemList.begin());
			ThreadContext* cpu = m5_system->getThreadContext(nih->procID);
			if(tlbHackInterface && tlbHackInterface->PageKnown(thread, vAddr))
			{
				pAddr = tlbHackInterface->Lookup(thread, vAddr);
			}
			else
			{
			        pAddr = TheISA::vtophys(cpu, vAddr);
				tlbHackInterface->AddEntry(thread, vAddr, pAddr);
			}
			assert(pAddr);
			std::cout << "Short-curcuiting TLB service routine with lookup" << std::endl;
			uint32_t buffer[9];
			buffer[0] = LCACC_CMD_TLB_SERVICE;
			buffer[1] = args[1];
			buffer[2] = bc_vAddr.u32[0];
			buffer[3] = bc_vAddr.u32[1];
			BitConverter bc;
			bc.u64[0] = pAddr;
			buffer[4] = bc.u32[0];
			buffer[5] = bc.u32[1];
			buffer[6] = 0;
			buffer[7] = 0;
			buffer[8] = 0;
			nih->snpi->SendMessageOnDevice(nih->deviceID, source, buffer, sizeof(buffer));
			return;
		}
	        RaiseInterrupt(source, args[1], args, size);
	}
}
void HandleNetworkMessage(void* voidCB, int source, int destination, const char* buffer, int length)
{
	Arg3CallbackBase<int, const char*, int>* cb = (Arg3CallbackBase<int, const char*, int>*)voidCB;
	cb->Call(source, buffer, length);
}
void HandleEvent(void* arg)
{
        CallbackBase* cb = (CallbackBase*)arg;
        cb->Call();
        cb->Dispose();
}
void EnqueueEvent(void* arg, int dt)
{
        scheduleCB(HandleEvent, arg, dt);
}
NetworkInterruptHandle* createNetworkInterruptHandle(int portID, int deviceID, int procID) {
  NetworkInterruptHandle* handle = (NetworkInterruptHandle*)malloc(sizeof(NetworkInterruptHandle));
  //Zhenman: suppose we just have one M5 system running
  //System::printSystems();
  std::vector<System *>::iterator system_iterator = System::systemList.begin();
  System *m5_system = *system_iterator;
  //assert(m5_system);
  int num_thread_contexts = m5_system->numContexts();
  std::cerr << "System " << m5_system->name() << ": " << std::hex << m5_system << " num of thread contexts " << num_thread_contexts << std::endl;
  handle->snpi = g_networkPort_interface;
  handle->portID = portID;
  handle->deviceID = deviceID;
  handle->attachedCPU = m5_system->getThreadContext(procID);
  handle->procID = procID;

  return handle;
}

uint64_t LCAccMagicIntercept(void*, ThreadContext* cpu, int32_t op, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6, uint64_t arg7)
{
	uint64_t retVal = 0;
        tlbHackInterface = g_TLBHack_interface;
	//Note Barrier and TLB Touch are needed by initilization stage, where ruby is disabled
	//We only need the TLB inerface setup for these two operations
	if(op == 0xBA00 || op == 0xBA01) {
	  //Barrier Tick or Barrier Blocked
	  retVal = MagicIntercept(NULL, cpu, op, (int)arg2);
          //#ifdef SIM_VISUAL_TRACE
          #if 0
	  if(op == 0xBA01 && arg2 == 2) //NOTE: arg2 need to be consistent with benchmarks
	    g_system_ptr->getProfiler()->wakeup();
          #endif
	  return retVal;
	}
        else if (op == 0xC101) {
	  //Touch (TLBHack)
	  MagicHandler(NULL, cpu, op, (int)arg1, arg2);
	  return retVal;
	}
 
	NetworkInterrupts* ni = NetworkInterrupts::LookupNIByCpu(cpu->threadId());
	if(ni == NULL)
	{
		std::cout << "WARNING!  Magic instruction called from processor that lacks a network interface." << std::endl;
		std::cout << "CPU: " << cpu->threadId() << " called with magic instr : " << op << std::endl;
		std::cout << "If you are seeing this message, something is likely quite wrong." << std::endl;
		assert(0);
	}
	else
	{
		NetworkInterruptHandle* nih = ni->GetHandle();
		assert(nih);
		switch(op)
		{
		case(0xC010)://MAGIC_LCACC_RESERVE
			{
			  int thread = (int)arg1;
			  int lcacc = (int)arg2;
			  int delay = (int)arg3;
			  std::cout << "LCAcc_Reserve: thread " << thread << " lcacc " << lcacc << " delay " << delay << std::endl;
				if(lcacc == 0)
				{
					assert(NetworkInterrupts::pendingReservation.find(thread) != NetworkInterrupts::pendingReservation.end());
					assert(NetworkInterrupts::pendingReservation[thread].size() >= 1);
					std::cout << "Reserving ";
					for(size_t i = 0; i < NetworkInterrupts::pendingReservation[thread].size(); i++)
					{
						std::cout << "[" << NetworkInterrupts::pendingReservation[thread][i] << "]";
					}
					std::cout << " from " << nih->deviceID << std::endl;
					std::vector<uint32_t> packet;
					int32_t target = 0;//gam target
					packet.push_back(GAM_CMD_RESERVE);
					packet.push_back(thread);
					packet.push_back(delay);
					packet.push_back(NetworkInterrupts::pendingReservation[thread].size());
					for(size_t i = 0; i < NetworkInterrupts::pendingReservation[thread].size(); i++)
					{
						packet.push_back(NetworkInterrupts::pendingReservation[thread][i]);
					}
					if(NetworkInterrupts::BiNCurveInfo.find(thread) != NetworkInterrupts::BiNCurveInfo.end())
					{
						packet.push_back(NetworkInterrupts::BiNCurveInfo[thread].size());
						for(size_t i = 0; i < NetworkInterrupts::BiNCurveInfo[thread].size(); i++)
						{
							packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].bufferSize);
							packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].performance);
							packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].cacheImpact);
						}
						NetworkInterrupts::BiNCurveInfo.erase(thread);
					}
					nih->snpi->SendMessageOnDevice(nih->deviceID, target, &(packet[0]), sizeof(uint32_t) * packet.size());
					NetworkInterrupts::pendingReservation.erase(thread);
				}
				else
				{
					std::cout << "Adding reservation for " << lcacc << " to pending queue of thread " << thread << std::endl;
					NetworkInterrupts::pendingReservation[thread].push_back(lcacc);
				}
			}
			break;
		case(0xC019)://MAGIC_LCACC_DECLARE_ACC
			{//Declare accelerator use.  Used for TD arbitration with BiN
			        int thread = (int)arg1;
				NetworkInterrupts::AcceleratorDeclaration aDecl;
				aDecl.type = (int)arg2;
				aDecl.count = (int)arg3;
				NetworkInterrupts::accDeclInfo[thread].push_back(aDecl);
			}
			break;
		case(0xC020)://MAGIC_BiN_CURVE
			{
				std::cout << "***** MAGIC_SEND_BiN_CURVE\n";
				int thread = (int)arg1;
				std::cout << "Adding BiN curve info for thread " << thread << std::endl;
				uint32_t size = (uint32_t)arg2;
				if(size != 0)
				{//non zero size = valid element
					NetworkInterrupts::BiNPerformancePoint p;
					p.bufferSize = size;
					p.performance = (uint32_t)arg3; 
					p.cacheImpact = (uint32_t)arg4;
					NetworkInterrupts::BiNCurveInfo[thread].push_back(p);
				}
				else
				{//size zero is a special marker that indicates that we should fire off the existing curve points and request arbitration.
					assert(NetworkInterrupts::BiNCurveInfo.find(thread) != NetworkInterrupts::BiNCurveInfo.end());
					std::vector<uint32_t> packet;
					packet.push_back(BIN_CMD_ARBITRATE_REQUEST);
					packet.push_back(thread);
					packet.push_back(NetworkInterrupts::accDeclInfo[thread].size());
					packet.push_back(NetworkInterrupts::BiNCurveInfo[thread].size());
					for(size_t i = 0; i < NetworkInterrupts::accDeclInfo[thread].size(); i++)
					{
						packet.push_back(NetworkInterrupts::accDeclInfo[thread][i].type);
						packet.push_back(NetworkInterrupts::accDeclInfo[thread][i].count);
					}
					for(size_t i = 0; i < NetworkInterrupts::BiNCurveInfo[thread].size(); i++)
					{
						packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].bufferSize);
						packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].performance);
						packet.push_back(NetworkInterrupts::BiNCurveInfo[thread][i].cacheImpact);
					}
					nih->snpi->SendMessageOnDevice(nih->deviceID, 0, &(packet[0]), sizeof(uint32_t) * packet.size());
					NetworkInterrupts::BiNCurveInfo.erase(thread);
					NetworkInterrupts::accDeclInfo.erase(thread);
				}
			}
			break;
		case(0xC011)://MAGIC_LCACC_REQUEST
			{
				int32_t target = 0;
				uint32_t buffer[3];
	                        buffer[0] = GAM_CMD_REQUEST;
	                        buffer[1] = (uint32_t)arg1;
	                        buffer[2] = (uint32_t)arg2;
				std::cout << "Requesting " << std::dec << (int32_t)buffer[2]  << " from " << nih->deviceID << std::endl;
	                        nih->snpi->SendMessageOnDevice(nih->deviceID, target, buffer, sizeof(buffer));
			}
			break;
		case(0xC012)://MAGIC_LCACC_COMMAND
			{
				uint32_t buffer[9];
				int target = (int)arg2;
				buffer[0] = (uint32_t)arg3;
				buffer[1] = (uint32_t)arg1;
				BitConverter bc_vAddr;
				bc_vAddr.u64[0] = (uint64_t)arg4;
				buffer[2] = bc_vAddr.u32[0];
				buffer[3] = bc_vAddr.u32[1];
				std::cout << "Sending LCAcc Command Addr at " << bc_vAddr.u64[0] << " to " << target << " from " << nih->deviceID << std::endl;
				BitConverter bc;
				if(bc_vAddr.u64[0])
				{
					if(tlbHackInterface && tlbHackInterface->PageKnown(buffer[1], bc_vAddr.u64[0]))
					{
						bc.u64[0] = tlbHackInterface->Lookup(buffer[1], bc_vAddr.u64[0]);
						std::cout << bc.u64[0] << ", " << TheISA::vtophys(cpu, bc_vAddr.u64[0]) << std::endl;
					}
					else
					{
					        bc.u64[0] = TheISA::vtophys(cpu, bc_vAddr.u64[0]);				    
					}
					buffer[4] = bc.u32[0];
					buffer[5] = bc.u32[1];
					if(buffer[4] == 0 && buffer[5] == 0)
					{
						std::cout << "Command issued with un-translated non-zero tlb page" << std::endl;
						assert(buffer[4] == 0 && buffer[5] == 0);
					}
				}
				else
				{
					bc.u64[0] = 0;
					buffer[4] = 0;
					buffer[5] = 0;
				}
				buffer[6] = (uint32_t)arg5;
		                buffer[7] = (uint32_t)arg6; 
		                buffer[8] = (uint32_t)arg7;

				std::cout << "Sending a Command to " << target << ", " << buffer[0] << " [addr=" << bc_vAddr.u64[0] << "->" << bc.u64[0] << "], " << buffer[6] << " " << buffer[7] << " " << buffer[8] << std::endl;
				std::cout << " " << buffer[1] << " " << target << " " << buffer[0] << " " << bc_vAddr.u64[0] << " " << buffer[6] << " " << buffer[7] << " " << buffer[8] << std::endl;
				retVal = 1;
				nih->snpi->SendMessageOnDevice(nih->deviceID, target, buffer, sizeof(buffer));
			}
			break;
		case(0xC013)://MAGIC_LCACC_FREE
			{
				int target = 0;//fixed GAM addr
				int32_t buffer[3];
				buffer[0] = GAM_CMD_RELEASE;
				buffer[1] = (int32_t)arg1;
				buffer[2] = (int32_t)arg2;
				std::cout << "Freeing " << buffer[2] << std::endl;
				nih->snpi->SendMessageOnDevice(nih->deviceID, target, buffer, sizeof(buffer));
			}
			break;
		case(0x911C)://Get signal
			{
			        int thread = (int)arg3;
				retVal = ni->GetSignal(thread);
			}
			break;
                case(0XCCCD):
                        {
			  fatal("Should never invoke KillThread in the benchmarks! Use BarrierTick and BarrierWait instead.");
			}
                        break;
                case(0xCCCE):
                        {
                                System* sys = cpu->getSystemPtr();
                                warn("===>>>>> Cycle: %u Tick: %u", sys->curCycle(), sys->clockEdge());
                                warn("CPU %u", cpu->cpuId());
                                warn("Thread %d", (int)arg1);
                                warn("Debug Value %d", (int)arg2);
                        }
                        break;
		case(0xBA00)://Barrier Tick
		case(0xBA01)://Barrier Blocked
                        {
  			        retVal = MagicIntercept(NULL, cpu, op, (int)arg2);
                        }
			break;
                case(0xC101)://Touch (TLBHack)
                        {
   			        MagicHandler(NULL, cpu, op, (int)arg1, arg2);
                        }
                        break;
                case(0xC000)://MAGIC_LWI_REGISTER
                        {
			        int thread = (int)arg1;
                                int lcacc = (int)arg2;
                                logical_address_t addr = (uint64_t)arg3;
                                int i;
                                int max = (int32_t)arg5;
                                for (i = (int32_t)arg4;
                                        i < max; i++)
                                {
                                        logical_address_t logicalAddress = addr + i;
                                        physical_address_t physicalAddress = TheISA::vtophys(cpu, logicalAddress);
                                        if(physicalAddress)
                                        {					    
                                                LWI_RegisterAccepter(thread, lcacc, physicalAddress, logicalAddress, i);
                                        }
                                        else
                                        {
                                                break;
                                        }
                                } //End for loop
				retVal = i;
                        }
                        break;
                case(0xC004)://MAGIC_LWI_CLEAR_INTERRUPT
                        {
			  LWI_EndInterruptHandling(arg1);
                        }
                        break;
                case(0xC00C)://MAGIC_LWI_CHECK
                        {
			        int thread = (int)arg1;
                                logical_address_t la = 0;
                                LWI_StoredMessage msg;
                                LWI_MessageAccepter accepter;
                                if(LWI_GetMessagePair(thread, msg, accepter))
                                {
                                    assert(accepter.la_args.size());
                                    assert(accepter.la_args[0]);
                                    assert(accepter.pa_args.size());
                                    assert(accepter.la_args[1]);
                                    assert(msg.packet.size() < 100);
                                    assert(msg.packet.size() % 4 == 0);
				    std::cout << "[LWI_MAGIC_CHECK] @ thread " << thread << ": msg of size" << msg.packet.size() / 4 << std::endl;
                                    for(int i = 0; i < msg.packet.size() / 4; i++)
                                    {
                                      assert(accepter.la_args.size() > i * 4);
                                      uint32_t* v = (uint32_t* )&msg.packet[i * 4];
				      std::cout << i << ", " << accepter.la_args[i*4] << ", " << accepter.pa_args[i*4] << "," << *v << std::endl;
				      LCAcc::gem5Interface::WritePhysical(accepter.pa_args[i*4], (void*)v, 4);
                                    }
				    std::cout << std::endl;
                                    la = accepter.la_args[0];
                                    assert(la);
                                    DPRINTF(LWI, "Interrupt raised on thread %d ", thread);
                                    DPRINTF(LWI, "setting register to %u\n", la);
                                }
                                assert(la != 1 && la != 2);
				retVal = la;
                        }
                        break;
		}
	}
	return retVal;
}
