/**
 * This class serves as the interface between the gem5 simulator and the
 * added lcacc accelerator unit.  This allows the accelerator to place memory
 * requests into the system.
 * Based on RubyPort::Mem*Port and the O3 IcachePort and DcachePort.
 */

#ifndef __MEMORY_INTERFACE_HH__
#define __MEMORY_INTERFACE_HH__

#include "mem/mem_object.hh"
#include "mem/mport.hh"
#include "debug/Accelerator.hh"
#include "sim/eventq.hh"
#include "mem/packet.hh"
#include "params/MemoryInterface.hh"

class MemoryInterface : public MemObject
{
  /*
   * M5 ports for the accelerator.
   *
   * TODO: Alt design: inherit from MasterPort and SlavePort instead.
   * The only difference will be that the accelerator handles resending
   * packets if the port is busy.
   */

  class AcceleratorMasterPort : public MessageMasterPort
  {
  public:
    AcceleratorMasterPort(MemoryInterface* owner)
      : MessageMasterPort(owner->name() + ".accelerator_port", owner)
    { }

  protected:
    bool recvTimingResp(PacketPtr pkt);
    void recvRangeChange() {}
  };

  class ArbitraryEvent : public Event
  {
  public:
    ArbitraryEvent(void (*callback)(void *), void *arg = NULL,
                   Priority p = Default_Pri)
      : Event(p, AutoDelete), my_callback(callback), my_arg(arg)
    {
    }
    void process()
    {
      my_callback(my_arg);
    }
  private:
    void (*my_callback)(void *);
    void *my_arg;
  };

  /* Packages the callback with the sent packet. */
  struct WriteCallbackState : public Packet::SenderState {
    void (*m_callback)(void*);
    void *m_arg;
    WriteCallbackState(void (callback)(void*), void* arg)
      : m_callback(callback), m_arg(arg) {}
  };
  struct ReadCallbackState : public Packet::SenderState {
    void (*m_callback)(void*);
    void *m_arg;
    ReadCallbackState(void (callback)(void*), void* arg)
      : m_callback(callback), m_arg(arg) {}
  };

public:
  typedef MemoryInterfaceParams Params;
  const Params *params() const
  {
    return dynamic_cast<const Params *>(_params);
  }

  MemoryInterface(const Params *params);
  virtual ~MemoryInterface() {}

  /**
   * A temporary measure to give accelerators global access to the
   * accelerator interface.  Will not work as implemented with multiple
   * accelerator interfaces - Should then be altered to use python
   * configuration.
   */
  static MemoryInterface *Instance();

protected:
  /**
   * Callback functions that are called when a response from memory
   * is recieved.
   */
  virtual bool recvReadResp(PacketPtr pkt);
  virtual bool recvWriteResp(PacketPtr pkt);
  virtual BaseMasterPort& getMasterPort(const std::string& if_name,
                                        PortID idx = InvalidPortID);
private:
  /**
   * Helper function to send all types of packets.
   */
  void sendRequest(Addr paddr, uint8_t *data, int size, Packet::SenderState *state, MemCmd cmd, bool timing);

public:
  /**
   * Given a physical address and size of data, send a request to get
   * this data from memory.
   * May only support word size requests.
   * Data is returned through recvTimingResp callback.
   * Assumption: data requested lies within one block.  Does not split
   * requests.
   * The buffer provided will contain the result when the packet returns.
   */
  virtual void sendReadRequest(Addr paddr, uint8_t* data, int size, void (*callback)(void*), void* arg);
  /**
   * Same as sendReadRequest, except it writes to the address instead
   * of reading.  Takes a pointer to the data, which is not copied,
   * and must remain valid until the packet returns.
   */
  virtual void sendWriteRequest(Addr paddr, uint8_t* data, int size, void (*callback)(void*), void* arg);
  /**
   * Same as sendReadRequest, except the request travels instantly.
   * Read value is stored in the provided array.
   */
  virtual void functionalRead(Addr paddr, uint8_t* data, int size, void (*callback)(void*) = NULL, void* arg = NULL);
  /**
   * Same as sendWriteRequest, except the request travels instantly.
   * Because we know that the write finished when this returns, the
   * callback function is not necessary, and can be ignored.
   */
  virtual void functionalWrite(Addr paddr, uint8_t *data, int size, void (*callback)(void*) = NULL, void* arg = NULL);
  /**
   * Schedule a callback to occur in delay cycles, where delay can be 0.
   * If multiple events are scheduled for the same cycle, they will
   * be executed in FIFO order.
   */
  virtual void scheduleCallback(void (*callback)(void *), Cycles delay,
                                void *arg = NULL,
                                EventBase::Priority priority = EventBase::Default_Pri);

protected:
  AcceleratorMasterPort m_masterPort;
  int m_latency;
  MasterID m_masterId;
};

#endif //__MEMORY_INTERFACE_HH__
