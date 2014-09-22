#include "sound_module.hpp"

SoundModule::SoundModule()
{

}


SoundModule::~SoundModule()
{

}


void SoundModule::InitializeSoundModule()
{
  if (!sound_buffer.loadFromFile("audio/beep.wav"))
  {
    //std::cout << "Error reading sound file" << std::endl;
    return;
  }
  sound.setBuffer(sound_buffer);
}


void SoundModule::PlayBeepSound()
{
  sound.play();
}

void SoundModule::StopBeepSound()
{
  sound.stop();
}

