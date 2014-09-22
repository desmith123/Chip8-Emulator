#include "clock.hpp"

Clock::Clock()
{

}


Clock::~Clock()
{

}


void Clock::StartClock(uint32 clock_frequency)
{
  clock_period_milliseconds= 1000 / clock_frequency;
  //std::cout << "The clock period is set to: " << clock_period_milliseconds << std::endl;
}


void Clock::WaitRisingEdge()
{
  while ( cpu_clock.getElapsedTime().asMilliseconds() < clock_period_milliseconds )
  {
    // wait
  }
  cpu_clock.restart();
  //std::cout << "Clock rising edge" << std::endl;
}