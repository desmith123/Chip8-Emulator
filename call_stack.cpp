#include "call_stack.hpp"

CallStack::CallStack()
{

}


CallStack::~CallStack()
{

}


void CallStack::PushAddress(uint16 address)
{
  call_stack.push(address);
}


uint16 CallStack::PopAddress()
{
  uint16 next_address = call_stack.top();
  call_stack.pop();
  return next_address;
}