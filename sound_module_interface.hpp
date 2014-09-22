#ifndef SOUND_MODULE_INTERFACE_H_
#define SOUND_MODULE_INTERFACE_H_

#include "common_includes.hpp"

class SoundModuleInterface
{
public:
  virtual ~SoundModuleInterface() {};
  virtual void InitializeSoundModule() = 0;
  virtual void PlayBeepSound() = 0;
  virtual void StopBeepSound() = 0;
};

#endif //SOUND_MODULE_INTERFACE_H_