#ifndef TIMER_MODULE_INTERFACE_H_
#define TIMER_MODULE_INTERFACE_H_

#include "common_includes.hpp"

class TimerModuleInterface
{
public:
  virtual ~TimerModuleInterface() {};
  virtual void SetDelayTimerMs(uint32 milliseconds) = 0;
  virtual uint32 GetDelayTimerMs() = 0;
  virtual void SetSoundTimerMs(uint32 milliseconds) = 0;
  virtual uint32 GetSoundTimerMs() = 0;
};

#endif //TIMER_MODULE_INTERFACE_H_