#ifndef MEMORY_INTERFACE_H_
#define MEMORY_INTERFACE_H_

#include "common_includes.hpp"

class MemoryInterface
{
public:
  virtual ~MemoryInterface() {}
  virtual void LoadRom(std::string rom_name) = 0;
  virtual uint8 GetMemoryByte(uint16 memory_address) = 0;
  virtual void SetMemoryByte(uint16 memory_address, uint8 write_data) = 0;
};

#endif //MEMORY_INTERFACE_H_