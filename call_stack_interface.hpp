#ifndef CALL_STACK_INTERFACE_H_
#define CALL_STACK_INTERFACE_H_

#include "common_includes.hpp"

class CallStackInterface
{
public:
  virtual ~CallStackInterface() {};
  virtual void PushAddress(uint16 address) = 0;
  virtual uint16 PopAddress() = 0;
};

#endif //CALL_STACK_INTERFACE_H_