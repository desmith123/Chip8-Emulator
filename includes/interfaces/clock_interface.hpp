#ifndef CLOCK_INTERFACE_H_
#define CLOCK_INTERFACE_H_

#include "common_includes.hpp"

class ClockInterface
{
public:
  virtual ~ClockInterface() {};
  virtual void StartClock(uint32 clock_frequency) = 0;
  virtual void WaitRisingEdge() = 0;
};

#endif //CLOCK_INTERFACE_H_