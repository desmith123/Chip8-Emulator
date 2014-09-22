#ifndef TIMER_MODULE_H_
#define TIMER_MODULE_H_

#include "SFML\System\Clock.hpp"
#include "timer_module_interface.hpp"

class TimerModule : public TimerModuleInterface
{
public:
  TimerModule();
  ~TimerModule();
  void SetDelayTimerMs(uint32 milliseconds);
  uint32 GetDelayTimerMs();
  void SetSoundTimerMs(uint32 milliseconds);
  uint32 GetSoundTimerMs();

private:
  sf::Clock delay_timer;
  sf::Clock sound_timer;
  uint32 delay_timer_start;
  uint32 sound_timer_start;
  uint32 sound_timer_set;
};

#endif //TIMER_MODULE_H_