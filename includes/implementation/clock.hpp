#ifndef CLOCK_H_
#define CLOCK_H_

#include "clock_interface.hpp"
#include "SFML/System/Clock.hpp"

class Clock : public ClockInterface
{
public:
  Clock();
  ~Clock();
  void StartClock(uint32 clock_frequency);
  void WaitRisingEdge();
private:
  sf::Clock cpu_clock;
  uint32 clock_period_milliseconds;
};

#endif //CLOCK_H_