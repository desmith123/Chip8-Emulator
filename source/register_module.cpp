#include "register_module.hpp"

RegisterModule::RegisterModule()
{
  instruction_pointer = 0;
  address_register = 0;
  num_general_registers = 16;
  general_register_block = new uint8[num_general_registers];
  memset(general_register_block, 0, sizeof(general_register_block));
}


RegisterModule::~RegisterModule()
{
  delete general_register_block;
}


uint8 RegisterModule::ReadRegister(int register_number)
{
  if ( register_number < 0 || register_number > num_general_registers )
  {
    return 0x00;
  }

  return general_register_block[register_number];
}


void RegisterModule::SetRegister(int register_number, uint8 write_value)
{
  if ( register_number < 0 || register_number > num_general_registers )
  {
    return;
  }

  general_register_block[register_number] = write_value;
}


uint16 RegisterModule::ReadAddressRegister()
{
  return address_register;
}


void RegisterModule::SetAddressRegister(uint16 write_value)
{
  address_register = write_value;
}


uint16 RegisterModule::ReadInstructionPointer()
{
  return instruction_pointer;
}


void RegisterModule::SetInstructionPointer(uint16 write_value)
{
  instruction_pointer = write_value;
}