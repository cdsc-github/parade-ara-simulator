#include "modules/LCAcc/memInterface.hh"
#include "mem/request.hh"
#include "sim/system.hh"
#include "base/types.hh"
#include "mem/ruby/common/Global.hh"

MemoryInterface *globalMemInterface = NULL;

MemoryInterface *
MemoryInterfaceParams::create()
{
    return new MemoryInterface(this);
}

MemoryInterface::MemoryInterface(const Params* params)
    : MemObject(params), m_masterPort(this), m_latency(1),
      m_masterId(params->system->getMasterId(name()))
{
    if (globalMemInterface == NULL)
        globalMemInterface = this;
}

bool
MemoryInterface::AcceleratorMasterPort::recvTimingResp(PacketPtr pkt)
{
    assert(pkt->isResponse());
    assert(pkt->isWrite() || pkt->isRead());
    MemoryInterface *acc = (dynamic_cast<MemoryInterface *>(&owner));
    assert(acc);

    bool success = false;
    if(pkt->isWrite())
        success = acc->recvWriteResp(pkt);
    else
        success =  acc->recvReadResp(pkt);

    delete pkt->req;
    pkt->req = NULL;
    delete pkt;

    return success;
}

/*
 * Callback when a packet is returned from a read request.
 * Calls the callback function passed to the sendRequest functions.
 */
bool
MemoryInterface::recvReadResp(PacketPtr pkt)
{
    Addr address = pkt->getAddr();
    DPRINTF(Accelerator,"Accelerator accessed memory location %x.\n",\
            address);

    ReadCallbackState *state =
            dynamic_cast<ReadCallbackState*>(pkt->popSenderState());
    assert(state);
    if (state->m_callback)
        state->m_callback(state->m_arg);

    delete state;

    return true;
}

bool
MemoryInterface::recvWriteResp(PacketPtr pkt)
{
    Addr address = pkt->getAddr();
    DPRINTF(Accelerator,"Accelerator wrote memory location %x.\n",\
            address);

    WriteCallbackState *state =
            dynamic_cast<WriteCallbackState*>(pkt->popSenderState());
    assert(state);
    if (state->m_callback)
        state->m_callback(state->m_arg); //Inform device that the write has finished.
    delete state;

    return true;
}

BaseMasterPort&
MemoryInterface::getMasterPort(const std::string& if_name, PortID idx)
{
    if (if_name == "master_port")
        return m_masterPort;
    return MemObject::getMasterPort(if_name, idx);
}

/*
 * Helper function for other send request functions.
 * Sends any combination of read/write and functional/timing requests.
 * data pointer is either a buffer that will store the result, or
 * a buffer holding the data to be written.  The data buffer is owned
 * by the caller - if it dynamically allocated the data, it must delete it
 * after this packet receives a response.
 */
void
MemoryInterface::sendRequest(Addr paddr, uint8_t *data, int size, Packet::SenderState *state, MemCmd cmd, bool timing)
{
    Request::Flags flags = 0;
    // Note: although the M5 classic memory system (which includes
    // packets and requests) supports uncachable requests, Ruby
    // does not.  If changed to support M5, this can make simulations
    // more realistic (no unrealistically small caches).

    RequestPtr req = new Request(paddr, size, flags, m_masterId);
    // Assumption: do not want to read a block of memory, just
    // one or more addresses. (Not sure if reading more than one works)
    PacketPtr pkt = new Packet(req, cmd);

    pkt->dataStatic<uint8_t>(data);

    pkt->pushSenderState(state);

    if (timing)
        m_masterPort.schedTimingReq(pkt, clockEdge(Cycles(m_latency)));
    else
    {
      if (pkt->isRead()) {
        g_system_ptr->functionalRead(pkt);
      } else if (pkt->isWrite()) {
        g_system_ptr->functionalWrite(pkt);
      } else {
        panic("Unsupported functional command %s\n", pkt->cmdString());
      }

        assert(pkt->isWrite() || pkt->isRead());
        if (pkt->isWrite())
            recvWriteResp(pkt);
        else
            recvReadResp(pkt);
        delete pkt->req;
        delete pkt;
        //The state is deleted in the recieving function.
    }
}

void
MemoryInterface::sendReadRequest(Addr paddr, uint8_t* data, int size, void (*callback)(void*), void* arg)
{
    ReadCallbackState * state = new ReadCallbackState(callback, arg);

    sendRequest(paddr, data, size, state, MemCmd(MemCmd::Command::ReadReq), true);
}

void
MemoryInterface::sendWriteRequest(Addr paddr, uint8_t* data, int size, void (*callback)(void*), void* arg)
{
    //uint8_t* newData = new uint8_t[size];
    //memcpy(newData, data, size*sizeof(uint8_t));
    // NOTE: uncommenting out the above lines, and using newData instead
    // of data below frees caller from the responsibility of preserving
    // the data array until the packet returns.
    // HOWEVER: need to make sure newData is deleted: this may require
    // code changes, as currently all read/write commands use the same
    // interface.

    WriteCallbackState * state = new WriteCallbackState(callback, arg);

    sendRequest(paddr, data, size, state, MemCmd(MemCmd::Command::WriteReq), true);
}

void
MemoryInterface::functionalRead(Addr paddr, uint8_t* data, int size, void (*callback)(void*), void* arg)
{
    ReadCallbackState * state = new ReadCallbackState(callback, arg);

    sendRequest(paddr, data, size, state, MemCmd(MemCmd::Command::ReadExReq), false);
}

void
MemoryInterface::functionalWrite(Addr paddr, uint8_t *data, int size, void (*callback)(void*), void* arg)
{
    WriteCallbackState * state = new WriteCallbackState(callback, arg);

    sendRequest(paddr, data, size, state, MemCmd(MemCmd::Command::WriteInvalidateReq), false);
}

void
MemoryInterface::scheduleCallback(void (*callback)(void*), Cycles delay,
                                       void *arg, EventBase::Priority priority)
{
    MemoryInterface::ArbitraryEvent* event =
              new MemoryInterface::ArbitraryEvent(callback, arg, priority);
    schedule(event, clockEdge(delay));
}


MemoryInterface*
MemoryInterface::Instance()
{
    if (globalMemInterface == NULL)
        fatal ("Memory Interface expected to have been created already.\n");
    return globalMemInterface;
}
