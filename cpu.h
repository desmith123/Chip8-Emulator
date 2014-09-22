#ifndef CPU_H_
#define CPU_H_

#include "common_includes.hpp"
#include "call_stack.hpp"
#include "keyboard.hpp"
#include "display.hpp"
#include "memory.hpp"
#include "register_module.hpp"
#include "timer_module.hpp"
#include "sound_module.hpp"
#include "clock.hpp"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <map>

class CPU 
{
public:
  CPU();
  ~CPU();
  void Start();
  void Initialize();
  void CpuTest();
  void CpuReset();

private:
  void (CPU::*op_code)(uint32);
  std::map<unsigned char, void (CPU::*)(uint32)> opcode_map;

  void DrawSprite(uint32 x_cord, uint32 y_cord, uint32 height);

  /* CHIP8 Opcodes */
  void Opcode0xxx(uint32 instruction);
  void Opcode1xxx(uint32 instruction);
  void Opcode2xxx(uint32 instruction);
  void Opcode3xxx(uint32 instruction);
  void Opcode4xxx(uint32 instruction);
  void Opcode5xxx(uint32 instruction);
  void Opcode6xxx(uint32 instruction);
  void Opcode7xxx(uint32 instruction);
  void Opcode8xxx(uint32 instruction);
  void Opcode9xxx(uint32 instruction);
  void OpcodeAxxx(uint32 instruction);
  void OpcodeBxxx(uint32 instruction);
  void OpcodeCxxx(uint32 instruction);
  void OpcodeDxxx(uint32 instruction);
  void OpcodeExxx(uint32 instruction);
  void OpcodeFxxx(uint32 instruction);

  /* Various Modules */
  // Sound Module
  SoundModuleInterface *sound_module;
  // Memory
  MemoryInterface *memory;
  // Keyboard
  KeyboardInterface *keyboard;
  // Call Stack
  CallStackInterface *call_stack;
  // Register Module
  RegisterModuleInterface *registers;
  // Display
  DisplayInterface *display;
  // Timers
  TimerModuleInterface *timers;
  // CPU clock
  ClockInterface *clock;

};

#endif //CPU_H_