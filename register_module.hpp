#ifndef REGISTER_MODULE_H_
#define REGISTER_MODULE_H_

#include "register_module_interface.hpp"

class RegisterModule : public RegisterModuleInterface
{
public:
  RegisterModule();
  ~RegisterModule();
  uint8 ReadRegister(int register_number);
  void SetRegister(int register_number, uint8 write_value);
  uint16 ReadAddressRegister();
  void SetAddressRegister(uint16 write_value);
  uint16 ReadInstructionPointer();
  void SetInstructionPointer(uint16 write_value);
private:
  uint16 instruction_pointer;
  uint16 address_register;
  uint8 *general_register_block;
  int num_general_registers;
};

#endif //REGISTER_MODULE_H_