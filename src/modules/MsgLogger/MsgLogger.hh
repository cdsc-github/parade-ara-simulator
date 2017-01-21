#ifndef MSG_LOGGER_H
#define MSG_LOGGER_H

#include <iostream>
#include "modules/Common/mf_api.hh"

// std::cerr << std::dec << GetSystemTime() << ": " << x << ": " << y << std::endl;

#define ML_LOG(x, y)				\
{ \
  std::cerr << "[obj:\"" << x << "\"] [tick:" << std::dec << GetSystemTime() << "] " << y << std::endl; \
}
#endif
