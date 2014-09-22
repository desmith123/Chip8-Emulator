#ifndef KEYBOARD_INTERFACE_H_
#define KEYBOARD_INTERFACE_H_

#include "common_includes.hpp"

class KeyboardInterface
{
public:
  virtual ~KeyboardInterface() {};
  virtual uint8 PollKeyPressed() = 0;
  virtual void KeyboardInitialize() = 0;
};

#endif //KEYBOARD_INTERFACE_H_