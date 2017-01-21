#ifndef BIT_CONVERTER_H
#define BIT_CONVERTER_H

#include <stdint.h>

class BitConverter
{
public:
  union {
    uint8_t u8[8];
    int8_t s8[8];
    uint16_t u16[4];
    int16_t s18[4];
    uint32_t u32[2];
    int32_t s32[2];
    uint64_t u64[1];
    int64_t s64[1];
    float f32[2];
    double f64[1];
  };
};

#endif
