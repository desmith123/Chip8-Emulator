#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <map>
#include "SFML\Window.hpp"
#include "keyboard_interface.hpp"

class Keyboard : public KeyboardInterface
{
public:
  Keyboard();
  ~Keyboard();
  uint8 PollKeyPressed();
  void KeyboardInitialize();

private:
  std::map<sf::Keyboard::Key, char> key_map;
};

#endif //KEYBOARD_H_