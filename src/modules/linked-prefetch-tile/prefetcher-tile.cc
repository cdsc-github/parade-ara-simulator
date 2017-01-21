#include "prefetcher-tile.hh"
#include "SimicsInterface.hh"

void StartTransfer(DMAEngineHandle* dma, int srcDevice, uint64_t srcAddr,
                   unsigned int srcDimensions, const unsigned int* srcElementSize,
                   const int* srcStride, int dstDevice, uint64_t dstAddr,
                   unsigned int dstDimensions, const unsigned int* dstElementSize,
                   const int* dstStride, size_t transferSize, int buffer,
                   CallbackBase* onFinish);

void StartTransferPrio(DMAEngineHandle* dma, int srcDevice, uint64_t srcAddr,
                       unsigned int srcDimensions, const unsigned int* srcElementSize,
                       const int* srcStride, int dstDevice, uint64_t dstAddr,
                       unsigned int dstDimensions, const unsigned int* dstElementSize,
                       const int* dstStride, size_t transferSize, int priority, int buffer,
                       CallbackBase* onFinish);

void Prefetch(DMAEngineHandle* dma, uint64_t addr, unsigned int dimensions,
              const unsigned int* elementSize, const int* elementStride, size_t transferSize);

void StartSingleTransfer(DMAEngineHandle* dma, int srcSpm, uint64_t src,
                         int dstSpm, uint64_t dst, uint32_t size, int buffer, CallbackBase* onFinish);

void StartSingleTransferPrio(DMAEngineHandle* dma, int srcSpm, uint64_t src,
                             int dstSpm, uint64_t dst, uint32_t size, int priority, int buffer,
                             CallbackBase* onFinish);

void Configure(DMAEngineHandle* obj, int node, int spm,
               Arg1CallbackBase<TransferData*>* beginTranslateTiming,
               Arg1CallbackBase<uint64_t>* onError);

void finishTranslation(DMAEngineHandle* dma, TransferData* td);

void HookToMemoryPort(DMAEngineHandle* dma, const char* deviceName);

void UnhookMemoryPort(DMAEngineHandle* dma);

void Configure(DMAEngineHandle* dma, int node, int spm,
               Arg1CallbackBase<TransferData*>* beginTranslateTiming,
               Arg1CallbackBase<uint64_t>* onError)
{
  DMAEngineHandle* x = (DMAEngineHandle*)dma;
  x->ptd->Configure(node, spm, beginTranslateTiming, onError);
}

void finishTranslation(DMAEngineHandle* dma, TransferData* td)
{
  DMAEngineHandle* x = (DMAEngineHandle*)dma;
  x->ptd->finishTranslation(td);
}

void StartTransfer(DMAEngineHandle* dma, int srcDevice, uint64_t srcAddr,
                   unsigned int srcDimensions, const unsigned int* srcElementSize,
                   const int* srcStride, int dstDevice, uint64_t dstAddr,
                   unsigned int dstDimensions, const unsigned int* dstElementSize,
                   const int* dstStride, size_t elementSize, int buffer, CallbackBase* onFinish)
{
  StartTransferPrio(dma, srcDevice, srcAddr, srcDimensions, srcElementSize,
                    srcStride, dstDevice, dstAddr, dstDimensions, dstElementSize,
                    dstStride, elementSize, 0, buffer, onFinish);
}

void
StartTransferPrio(DMAEngineHandle* dma, int srcDevice, uint64_t srcAddr,
                  unsigned int srcDimensions, const unsigned int* srcElementSize,
                  const int* srcStride, int dstDevice, uint64_t dstAddr,
                  unsigned int dstDimensions, const unsigned int* dstElementSize,
                  const int* dstStride, size_t elementSize, int priority, int buffer,
                  CallbackBase* onFinish)
{
  DMAEngineHandle* x = (DMAEngineHandle*)dma;
  assert(x);
  assert(x->ptd);
  x->ptd->AddTransferSet(srcDevice, srcAddr, srcDimensions, srcElementSize,
                         srcStride, dstDevice, dstAddr, dstDimensions, dstElementSize,
                         dstStride, elementSize, priority, buffer, onFinish);
}

void
Prefetch(DMAEngineHandle* dma, uint64_t addr, unsigned int dimensions,
         const unsigned int* elementSize, const int* elementStride, size_t transferSize)
{
  DMAEngineHandle* x = (DMAEngineHandle*)dma;
  assert(x);
  assert(x->ptd);
  x->ptd->PrefetchSet(addr, dimensions, elementSize, elementStride, transferSize);
}

void
StartSingleTransfer(DMAEngineHandle* dma, int srcSpm, uint64_t src,
                    int dstSpm, uint64_t dst, uint32_t size, int buffer, CallbackBase* onFinish)
{
  StartSingleTransferPrio(dma, srcSpm, src, dstSpm, dst,
                          size, 0, buffer, onFinish);
}

void
StartSingleTransferPrio(DMAEngineHandle* dma, int srcSpm, uint64_t src,
                        int dstSpm, uint64_t dst, uint32_t size, int priority, int buffer,
                        CallbackBase* onFinish)
{
  DMAEngineHandle* x = (DMAEngineHandle*)dma;
  assert(x);
  assert(x->ptd);
  x->ptd->AddSingleTransfer(srcSpm, src, dstSpm, dst,
                            size, priority, buffer, onFinish);
}

void
HookToMemoryPort(DMAEngineHandle* dma, const char* deviceName)
{
  DMAEngineHandle* x = (DMAEngineHandle*)dma;
  assert(x);
  assert(x->ptd);
  x->ptd->HookToMemoryPort(deviceName);
}

void
UnhookMemoryPort(DMAEngineHandle* dma)
{
  DMAEngineHandle* x = (DMAEngineHandle*)dma;
  assert(x);
  assert(x->ptd);
  x->ptd->UnhookMemoryPort();
}

DMAEngineHandle*
CreateDMAEngineHandle()
{
  DMAEngineHandle* ptdh = (DMAEngineHandle *)malloc(sizeof(DMAEngineHandle));
  ptdh->ptd = new DMAEngine();
  return ptdh;
}

int
DeleteDMAEngineHandle(DMAEngineHandle* obj)
{
  assert(obj);
  DMAEngineHandle* ptdh = (DMAEngineHandle*)obj;
  assert(ptdh->ptd);
  delete ptdh->ptd;
  return 0;
}

prftch_direct_interface_t*
CreateDMAEngineInterface()
{
  warn("Create a new DMA engine [prefetcher]");
  prftch_direct_interface_t* prftch_direct_interface =
    (prftch_direct_interface_t *)malloc(sizeof(prftch_direct_interface_t));
  prftch_direct_interface->StartTransfer = StartTransfer;
  prftch_direct_interface->StartTransferPrio = StartTransferPrio;
  prftch_direct_interface->Prefetch = Prefetch;
  prftch_direct_interface->Configure = Configure;
  prftch_direct_interface->StartSingleTransfer = StartSingleTransfer;
  prftch_direct_interface->StartSingleTransferPrio = StartSingleTransferPrio;
  prftch_direct_interface->HookToMemoryPort = HookToMemoryPort;
  prftch_direct_interface->UnhookMemoryPort = UnhookMemoryPort;
  prftch_direct_interface->finishTranslation = finishTranslation;

  return prftch_direct_interface;
}
