#include "timer_module.hpp"

TimerModule::TimerModule()
{
  delay_timer_start = 0;
  sound_timer_start = 0;
  sound_timer_set = 0;
}


TimerModule::~TimerModule()
{

}


void TimerModule::SetDelayTimerMs(uint32 milliseconds)
{
  delay_timer.restart();
  //Ajust for 60hz clock
  delay_timer_start = milliseconds / 60;
  //std::cout << "timer: Setting delay timer to: " << std::hex << delay_timer_start << std::endl;
}


uint32 TimerModule::GetDelayTimerMs()
{
  //std::cout << "Elapsed time delay timer since start is: " << std::hex << delay_timer.getElapsedTime().asMilliseconds() << std::endl;
  uint32 elasped = delay_timer.getElapsedTime().asMilliseconds();
  //std::cout << "Time remaining is: " << std::hex << delay_timer_start - delay_timer.getElapsedTime().asMilliseconds() << std::endl;

  if ( delay_timer.getElapsedTime().asMilliseconds() > delay_timer_start )
  {
    return 0;
  }
  else
  {
    return (delay_timer_start - delay_timer.getElapsedTime().asMilliseconds());
  }
}


void TimerModule::SetSoundTimerMs(uint32 milliseconds)
{
  sound_timer.restart();
  //Ajust for 60hz clock
  sound_timer_start = milliseconds / 60;
  sound_timer_set = 1;
  //std::cout << "timer: Setting sound timer to: " << std::hex << sound_timer_start << std::endl;
}


uint32 TimerModule::GetSoundTimerMs()
{
  //std::cout << "Elapsed time sound timer since start is: " << std::hex << sound_timer.getElapsedTime().asMilliseconds() << std::endl;
  uint32 elasped = sound_timer.getElapsedTime().asMilliseconds();
  //std::cout << "Time remaining is: " << std::hex << sound_timer_start - sound_timer.getElapsedTime().asMilliseconds() << std::endl;

  if ( (sound_timer.getElapsedTime().asMilliseconds() > sound_timer_start) && sound_timer_set )
  {
    sound_timer_set = 0;
    return 0;
  }
  else
  {
    return 0xff;
  }
}