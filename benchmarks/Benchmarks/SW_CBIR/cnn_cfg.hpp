#include <stdlib.h>
#include <sys/types.h>

////////////////////////////////////////////////////////////////////////////////
// hardware specification
#define UNROLL 16
#define HWFIn UNROLL
#define HWFOut UNROLL
#define HWFinR 30
#define HWFinC 226
#define HWFR 28
#define HWFC 224
#define HWKsize 3
#define HWKstride 1

#define HWin HWFIn
#define HWout HWFOut
#define BitW 32*UNROLL

#define POOL 1
#define RELU 1
////////////////////////////////////////////////////////////////////////////////
// cnn module specification
#define READ 1
#define WRITE 0

#define DATA_R   32
#define DATA     1
#define CONV11   64
#define CONV12   64
#define POOL12   64
#define CONV21   128
#define CONV22   128
#define POOL22   128
#define CONV31   256
#define CONV32   256
#define CONV33   256
#define POOL33   256
#define CONV41   512
#define CONV42   512
#define CONV43   512
#define POOL43   512
#define CONV51   512
#define CONV52   512
#define CONV53   512
#define POOL53   512
#define PAD      1
#define STRIDE   1
#define KSIZE    3

#define CONV11_I (DATA_R + 2*PAD)
#define CONV11_R ((CONV11_I - KSIZE + STRIDE)/STRIDE)
#define CONV12_I (CONV11_R + 2*PAD)
#define CONV12_R ((CONV12_I - KSIZE + STRIDE)/STRIDE)
#define POOL12_R (CONV12_R/2)
#define CONV21_I (POOL12_R + 2*PAD)
#define CONV21_R ((CONV21_I - KSIZE + STRIDE)/STRIDE)
#define CONV22_I (CONV21_R + 2*PAD)
#define CONV22_R ((CONV22_I - KSIZE + STRIDE)/STRIDE)
#define POOL22_R (CONV22_R/2)
#define CONV31_I (POOL22_R + 2*PAD)
#define CONV31_R ((CONV31_I - KSIZE + STRIDE)/STRIDE)
#define CONV32_I (CONV31_R + 2*PAD)
#define CONV32_R ((CONV32_I - KSIZE + STRIDE)/STRIDE)
#define CONV33_I (CONV32_R + 2*PAD)
#define CONV33_R ((CONV33_I - KSIZE + STRIDE)/STRIDE)
#define POOL33_R (CONV33_R/2)
#define CONV41_I (POOL33_R + 2*PAD)
#define CONV41_R ((CONV41_I - KSIZE + STRIDE)/STRIDE)
#define CONV42_I (CONV41_R + 2*PAD)
#define CONV42_R ((CONV42_I - KSIZE + STRIDE)/STRIDE)
#define CONV43_I (CONV42_R + 2*PAD)
#define CONV43_R ((CONV43_I - KSIZE + STRIDE)/STRIDE)
#define POOL43_R (CONV43_R/2)
#define CONV51_I (POOL43_R + 2*PAD)
#define CONV51_R ((CONV51_I - KSIZE + STRIDE)/STRIDE)
#define CONV52_I (CONV51_R + 2*PAD)
#define CONV52_R ((CONV52_I - KSIZE + STRIDE)/STRIDE)
#define CONV53_I (CONV52_R + 2*PAD)
#define CONV53_R ((CONV53_I - KSIZE + STRIDE)/STRIDE)
#define POOL53_R (CONV53_R/2)

