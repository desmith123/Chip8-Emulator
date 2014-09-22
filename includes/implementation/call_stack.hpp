#ifndef CALL_STACK_H_
#define CALL_STACK_H_

#include <stack>
#include "call_stack_interface.hpp"

class CallStack : public CallStackInterface
{
public:
  CallStack();
  ~CallStack();
  void PushAddress(uint16 address);
  uint16 PopAddress();

private:
   std::stack<uint16> call_stack;
};

#endif //CALL_STACK_H_