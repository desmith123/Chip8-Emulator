#ifndef REGISTER_MODULE_INTERFACE_H_
#define REGISTER_MODULE_INTERFACE_H_

#include "common_includes.hpp"

class RegisterModuleInterface
{
public:
  virtual ~RegisterModuleInterface() {}
  virtual uint8 ReadRegister(int register_number) = 0;
  virtual void SetRegister(int register_number, uint8 write_value) = 0;
  virtual uint16 ReadAddressRegister() = 0;
  virtual void SetAddressRegister(uint16 write_value) = 0;
  virtual uint16 ReadInstructionPointer() = 0;
  virtual void SetInstructionPointer(uint16 write_value) = 0;
};

#endif //REGISTER_MODULE_INTERFACE_H_