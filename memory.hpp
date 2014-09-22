#ifndef MEMORY_H_
#define MEMORY_H_

#include "memory_interface.hpp"
#include <fstream>

//TODO: memory access out of bounds.

class Memory : public MemoryInterface
{
public:
  Memory();
  ~Memory();
  void LoadRom(std::string rom_name);
  uint8 GetMemoryByte(uint16 memory_address);
  void SetMemoryByte(uint16 memory_address, uint8 write_data);
private:
  uint8 *memory;
  int memory_size;

};

#endif //MEMORY_H_