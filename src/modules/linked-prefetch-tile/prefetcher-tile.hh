#ifndef PRFTCH_H
#define PRFTCH_H

#define SIM_ENHANCE
#define SIM_NET_PORTS
#include "../Common/BaseCallbacks.hh"
#include <stdlib.h>
#include <stdint.h>

class TransferData;

class DMAEngine;

typedef struct {
  DMAEngine* ptd;
} DMAEngineHandle;

typedef struct prftch_direct_interface {
  void (*StartTransfer)(DMAEngineHandle* dma, int srcDevice, uint64_t srcAddr,
                        unsigned int srcDimensions, const unsigned int* srcElementSize,
                        const int* srcStride, int dstDevice, uint64_t dstAddr,
                        unsigned int dstDimensions, const unsigned int* dstElementSize,
                        const int* dstStride, size_t transferSize, int buffer,
                        CallbackBase* onFinish);

  void (*StartTransferPrio)(DMAEngineHandle* dma, int srcDevice,
                            uint64_t srcAddr, unsigned int srcDimensions,
                            const unsigned int* srcElementSize, const int* srcStride, int dstDevice,
                            uint64_t dstAddr, unsigned int dstDimensions,
                            const unsigned int* dstElementSize, const int* dstStride,
                            size_t transferSize, int priority, int buffer, CallbackBase* onFinish);

  void (*Prefetch)(DMAEngineHandle* dma, uint64_t srcAddr,
                   unsigned int srcDimensions, const unsigned int* srcElementSize,
                   const int* srcStride, size_t transferSize);

  void (*StartSingleTransfer)(DMAEngineHandle* dma, int srcSpm, uint64_t src,
                              int dstSpm, uint64_t dst, uint32_t size, int buffer,
                              CallbackBase* onFinish);

  void (*StartSingleTransferPrio)(DMAEngineHandle* dma, int srcSpm,
                                  uint64_t src, int dstSpm, uint64_t dst, uint32_t size, int priority,
                                  int buffer, CallbackBase* onFinish);

  void (*Configure)(DMAEngineHandle* dma, int node, int spm,
                    Arg1CallbackBase<TransferData*>* beginTranslateTiming,
                    Arg1CallbackBase<uint64_t>* onError);

  void (*finishTranslation)(DMAEngineHandle* dma, TransferData* td);

  void (*HookToMemoryPort)(DMAEngineHandle* dma, const char* deviceName);

  void (*UnhookMemoryPort)(DMAEngineHandle* dma);

} prftch_direct_interface_t;

prftch_direct_interface_t* CreateDMAEngineInterface();

DMAEngineHandle* CreateDMAEngineHandle();

int DeleteDMAEngineHandle(DMAEngineHandle* obj);

#endif
