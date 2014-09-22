#include "keyboard.hpp"

Keyboard::Keyboard()
{

}


Keyboard::~Keyboard()
{

}


uint8 Keyboard::PollKeyPressed()
{

  std::map<sf::Keyboard::Key, char>::iterator it = key_map.begin();

  while ( it != key_map.end() )
  {
    if ( sf::Keyboard::isKeyPressed ( it->first ) )
    {
      //std::cout << "returning " << std::hex << (unsigned int)it->first << std::endl;
      return it->second;
    }
    it++;
  }

  //std::cout << "returning: " << std::hex << (unsigned int)0xff << std::endl;
  return 0xff;
}


void Keyboard::KeyboardInitialize()
{
  key_map[sf::Keyboard::Num1] = 0x1;
  key_map[sf::Keyboard::Num2] = 0x2;
  key_map[sf::Keyboard::Num3] = 0x3;
  key_map[sf::Keyboard::Num4] = 0xc;
  key_map[sf::Keyboard::Q] = 0x4;
  key_map[sf::Keyboard::W] = 0x5;
  key_map[sf::Keyboard::E] = 0x6;
  key_map[sf::Keyboard::R] = 0xd;
  key_map[sf::Keyboard::A] = 0x7;
  key_map[sf::Keyboard::S] = 0x8;
  key_map[sf::Keyboard::D] = 0x9;
  key_map[sf::Keyboard::F] = 0xe;
  key_map[sf::Keyboard::Z] = 0xa;
  key_map[sf::Keyboard::X] = 0x0;
  key_map[sf::Keyboard::C] = 0xb;
  key_map[sf::Keyboard::V] = 0xf;
}