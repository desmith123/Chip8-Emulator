#ifndef SOUND_MODULE_H_
#define SOUND_MODULE_H_

#include "SFML\Audio.hpp"
#include "sound_module_interface.hpp"

class SoundModule : public SoundModuleInterface
{
public:
  SoundModule();
  ~SoundModule();
  void InitializeSoundModule();
  void PlayBeepSound();
  void StopBeepSound();
private:
  sf::SoundBuffer sound_buffer;
  sf::Sound sound;
};

#endif //SOUND_MODULE_H_