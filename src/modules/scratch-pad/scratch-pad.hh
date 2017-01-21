/*
  scratch-pad.h - sample code for a Simics device
*/

#ifndef SCRATCH_PAD_H
#define SCRATCH_PAD_H

#include <stdint.h>
#include <vector>

class ScratchPad
{
public:
  std::vector<uint8_t> data;
  std::vector<bool> valid;

  void CheckSize(uint64_t size)
  {
    while (data.size() <= size) {
      data.push_back(0xcd);
      valid.push_back(false);
    }
  }

  void Clear()
  {
    data = std::vector<uint8_t>();
    valid = std::vector<bool>();
  }
};

typedef struct ScratchPadHandle_t {
  ScratchPad* sp;
} ScratchPadHandle;

typedef struct scratch_pad_interface {
  void (*read)(ScratchPadHandle* obj, uint64_t address, void* dataRd,
               unsigned int size);
  void (*write)(ScratchPadHandle* obj, uint64_t address, const void* dataWr,
                unsigned int size);
  void (*clear)(ScratchPadHandle* obj);
} scratch_pad_interface_t;

ScratchPadHandle* CreateNewScratchPad();
int DeleteScratchPad(ScratchPadHandle* obj);
scratch_pad_interface_t* CreateScratchPadInterface();

#endif /* SCRATCH_PAD_H */
