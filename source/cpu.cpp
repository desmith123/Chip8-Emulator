#include "cpu.h"

bool instruction_jump = 0;

CPU::CPU()
{
  opcode_map[0x00] = &CPU::Opcode0xxx;
  opcode_map[0x01] = &CPU::Opcode1xxx;
  opcode_map[0x02] = &CPU::Opcode2xxx;
  opcode_map[0x03] = &CPU::Opcode3xxx;
  opcode_map[0x04] = &CPU::Opcode4xxx;
  opcode_map[0x05] = &CPU::Opcode5xxx;
  opcode_map[0x06] = &CPU::Opcode6xxx;
  opcode_map[0x07] = &CPU::Opcode7xxx;
  opcode_map[0x08] = &CPU::Opcode8xxx;
  opcode_map[0x09] = &CPU::Opcode9xxx;
  opcode_map[0x0A] = &CPU::OpcodeAxxx;
  opcode_map[0x0B] = &CPU::OpcodeBxxx;
  opcode_map[0x0C] = &CPU::OpcodeCxxx;
  opcode_map[0x0D] = &CPU::OpcodeDxxx;
  opcode_map[0x0E] = &CPU::OpcodeExxx;
  opcode_map[0x0F] = &CPU::OpcodeFxxx;

  // Set up modules
  memory = new Memory;
  keyboard = new Keyboard;
  display = new Display;
  registers = new RegisterModule;
  timers = new TimerModule;
  call_stack = new CallStack;
  clock = new Clock;
  sound_module = new SoundModule;
}


CPU::~CPU()
{
  delete memory;
  delete keyboard;
  delete display;
  delete registers;
  delete timers;

}

void CPU::Start()
{
  uint16 first_byte_location;
  uint16 second_byte_location;
  uint16 next_instruction;
  uint32 opcode;

  // Initiailize
  memory->LoadRom("PONG");
  keyboard->KeyboardInitialize();
  display->InitializeScreen();
  registers->SetInstructionPointer(512);
  display->ClearScreen();
  clock->StartClock(1);
  sound_module->InitializeSoundModule();

  // get first instruction
  while( true )
  //for(int i = 0; i < 6; i++)
  {
    //clock->WaitRisingEdge();
    first_byte_location = registers->ReadInstructionPointer();
    second_byte_location = first_byte_location + 1;
    next_instruction = memory->GetMemoryByte(first_byte_location);
    next_instruction = next_instruction << 8;
    next_instruction = next_instruction + memory->GetMemoryByte(second_byte_location);

    //std::cout<< "The next instruction is: " << std::hex << next_instruction << std::endl;
  
    opcode = (next_instruction & 0xf000) >> 12;
    //std::cout << "The opcode is " << std::hex << opcode << std::endl;
    (this->*opcode_map[opcode])(next_instruction);  

    display->RefreshScreen();
    
    // increment if opcodes didnt chnage, eg subroutine entry
    // replace with flag
    if ( !instruction_jump )
    {
      registers->SetInstructionPointer( registers->ReadInstructionPointer() + 2 );
    }
    instruction_jump = false;
    if ( timers->GetSoundTimerMs() == 0 )
    {
      sound_module->PlayBeepSound();
      //std::cout << "Playing Sound" << std::endl;
    }
  }
}

/* 
** 0NNN - Calls RCA 1802 program at address NNN.
** 00E0 - Clears the screen.
** 00EE - Returns from a subroutine.
*/
void CPU::Opcode0xxx(uint32 instruction)
{
  //std::cout << "Opcode0xxx" << std::endl;
  if ( instruction == 0x00E0 )
  {
    std::cout << "sub_opcode 0x00E0" << std::endl;
    display->ClearScreen();
  }
  else if ( instruction == 0x00EE )
  {
    std::cout << "sub_opcode 0x00EE" << std::endl;
    uint16 new_address = call_stack->PopAddress();
    //std::cout << "Popped address: " << std::hex << new_address << std::endl;
    registers->SetInstructionPointer(new_address);
    instruction_jump = true;
  }
  else
  {
    std::cout << "sub_opcode 0x0NNN" << std::endl;
    uint16 jump_address = instruction & 0x0fff;
    std::cout << "Opcode1xxx: address to jump to is: " << std::hex << jump_address << std::endl;
    registers->SetInstructionPointer(jump_address);
    instruction_jump = true;
  }
}

/*
** 1NNN - Jumps to address NNN.
*/ 
void CPU::Opcode1xxx(uint32 instruction)
{
  uint16 jump_address = instruction & 0x0fff;
  std::cout << "Opcode1xxx: address to jump to is: " << std::hex << jump_address << std::endl;
  registers->SetInstructionPointer(jump_address);
  instruction_jump = true;
}

/*
** 2NNN - Calls subroutine at NNN.
*/ 
void CPU::Opcode2xxx(uint32 instruction)
{
  std::cout << "Opcode2xxx" << std::endl;
  //std::cout << "Address before call_stack" << std::hex << registers->ReadInstructionPointer() <<  std::endl;
  call_stack->PushAddress(registers->ReadInstructionPointer() + 2);

  uint16 subroutine_address = instruction & 0x0fff;
  //std::cout << "Subroutine address is: " << std::hex << subroutine_address << std::endl;
  registers->SetInstructionPointer(subroutine_address);
  instruction_jump = true;
}

/*
** 3XNN - Skips the next instruction if VX equals NN.
*/ 
void CPU::Opcode3xxx(uint32 instruction)
{
  uint8 register_number = (instruction & 0x0f00) >> 8;
  uint8 comparison_value = (instruction & 0x00ff);
  std::cout << "Opcode3xxx: register: " << std::hex << (uint32)register_number << std::endl;
  uint8 register_value = registers->ReadRegister(register_number);
  std::cout << "Skipping if regval " << std::hex << (uint32)register_value << " equals " << (uint32)comparison_value << std::endl;
  if ( register_value == comparison_value )
  {
    registers->SetInstructionPointer(registers->ReadInstructionPointer() + 4);
    instruction_jump = true;
    std::cout << "Skipping next instruction" << std::endl;
  }
}

/*
** 4XNN - Skips the next instruction if VX doesn't equal NN.
*/ 
void CPU::Opcode4xxx(uint32 instruction)
{
  uint8 register_number = (instruction & 0x0f00) >> 8;
  uint8 register_value = registers->ReadRegister(register_number);
  uint8 compare_value = instruction & 0x00ff;
  std::cout << "Opcode4xxx, register number " << std::hex << (uint32)register_number << std::endl;
  //std::cout << "register value " << std::hex << register_value << " compare value " << compare_value << std::endl;

  if ( register_value != compare_value )
  {
    //std::cout << "Two values not equal, skipping instruction" << std::endl;
    registers->SetInstructionPointer( registers->ReadInstructionPointer() + 4 );
    instruction_jump = true;
  }
}

/*
** 5XY0 - Skips the next instruction if VX equals VY.
*/ 
void CPU::Opcode5xxx(uint32 instruction)
{
  uint32 first_register = ( instruction & 0x0f00 ) >> 8;
  uint32 second_register = ( instruction & 0x00f0 ) >> 4;
  std::cout << "Opcode5xxx, first reg " << std::hex << first_register << " second reg " << second_register << std::endl;
  uint32 first_register_value = registers->ReadRegister(first_register);
  uint32 second_register_value = registers->ReadRegister(second_register);

  if ( first_register_value == second_register_value )
  {
    registers->SetInstructionPointer( registers->ReadInstructionPointer() + 4 );
    instruction_jump = true;
  }
}

/*
** 6XNN - Sets VX to NN.
*/ 
void CPU::Opcode6xxx(uint32 instruction)
{
  uint8 register_number = ( instruction & 0x0f00 ) >> 8;
  uint8 data_value = ( instruction & 0x00ff );
  std::cout << "Opcode6xxx: Setting register: " << std::hex << (uint32)register_number << " to "  << std::endl;
  registers->SetRegister(register_number, data_value);
}

/*
** 7XNN - Adds NN to VX.
*/ 
void CPU::Opcode7xxx(uint32 instruction)
{
  uint32 register_number = (instruction & 0x0f00) >> 8;
  uint32 data_value = (instruction & 0x00ff);
  std::cout << "Opcode7xxx, register is: " << std::hex << register_number << " Adding: " << data_value << std::endl;
  uint32 prev_register_value = registers->ReadRegister(register_number);
  //std::cout << "Previous register value is: " << std::hex << (uint32)prev_register_value << std::endl;
  registers->SetRegister(register_number, prev_register_value + data_value);
  //std::cout << "New register value is: " << std::hex << (uint32)registers->ReadRegister(register_number) << std::endl;
}

/*
** 8XY0 - Sets VX to the value of VY.
** 8XY1 - Sets VX to VX or VY.
** 8XY2 - Sets VX to VX and VY.
** 8XY3 - Sets VX to VX xor VY.
** 8XY4 - Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
** 8XY5 - VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
** 8XY6 - Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
** 8XY7 - Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
** 8XYE - Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
*/ 
void CPU::Opcode8xxx(uint32 instruction)
{
  uint32 first_register = ( instruction & 0x0f00 ) >> 8;
  uint32 second_register = ( instruction & 0x00f0 ) >> 4;
  std::cout << "Opcode8xxx, first reg " << std::hex << (uint32)first_register << " second reg " <<  (uint32)second_register << std::endl;
  uint32 first_register_value = registers->ReadRegister(first_register);
  uint32 second_register_value = registers->ReadRegister(second_register);
  //std::cout << "Opcode8xxx, first reg value " << std::hex << first_register_value << " second reg value " << second_register_value << std::endl;
  switch ( instruction & 0x000f )
  {
  case 0x0000:
    {
      std::cout << "Case 0x0000" << std::endl;
      first_register_value = second_register_value;
      registers->SetRegister(first_register, first_register_value);
      //std::cout << "Set register: " << std::hex << first_register << " to " << first_register_value << std::endl;
      break;
    }
  case 0x0001:
    {
      std::cout << "Case 0x0001" << std::endl;
      first_register_value = first_register_value | second_register_value;
      registers->SetRegister(first_register, first_register_value);
      //std::cout << "Set register: " << std::hex << first_register << " to " << first_register_value << std::endl;
      break;
    }
  case 0x0002:
    {
      std::cout << "Case 0x0002" << std::endl;
      first_register_value = first_register_value & second_register_value;
      registers->SetRegister(first_register, first_register_value);
      //std::cout << "Set register: " << std::hex << first_register << " to " << first_register_value << std::endl;
      break;
    }
  case 0x0003:
    {
      std::cout << "Case 0x0003" << std::endl;
      first_register_value = first_register_value ^ second_register_value;
      registers->SetRegister(first_register, first_register_value);
      //std::cout << "Set register: " << std::hex << first_register << " to " << first_register_value << std::endl;
      break;
    }
  case 0x0004:
    {
      std::cout << "Case 0x0004" << std::endl;

      if ( second_register_value > (0xff - first_register_value ))
      {
        registers->SetRegister(0xf, 1);
      }
      else
      {
        registers->SetRegister(0xf, 0);
      }

      first_register_value = ( first_register_value + second_register_value);
      registers->SetRegister(first_register, first_register_value);
      //std::cout << "Set register: " << std::hex << first_register << " to " << first_register_value << std::endl;
      break;
    }
  case 0x0005:
    {
      std::cout << "Case 0x0006" << std::endl;
      //std::cout << "First number is: " << std::hex << first_register_value << std::endl;
      //std::cout << "Second number is: " << std::hex << second_register_value << std::endl;

      if ( second_register_value > first_register_value )
      {
        registers->SetRegister(0xf, 0);
      }
      else
      {
        registers->SetRegister(0xf, 1);
      }
    
      first_register_value = first_register_value - second_register_value;
      registers->SetRegister(first_register, first_register_value);
      std::cout << "Set register: " << std::hex << first_register << " to " << first_register_value << std::endl;
      break;
    }
  case 0x0006:
    {
      std::cout << "Case 0x0006" << std::endl;
       
      std::cout << "First number is: " << std::hex << (uint32)first_register_value << std::endl;
      std::cout << "Second number is: " << std::hex << (uint32)second_register_value << std::endl;
      
      // set carry register to lSB before shift
      if ( first_register_value & 0x1 )
      {
        registers->SetRegister(0xf, 1);
        std::cout << "Set carry register to 1" << std::endl;
      }
      else
      {
        registers->SetRegister(0xf, 0);
        std::cout << "Set carry register to 0" << std::endl;
      }

      first_register_value = first_register_value >> 1;
      registers->SetRegister(first_register, first_register_value);
      std::cout << "Set register: " << std::hex << (uint32)first_register << " to " << (uint32)first_register_value << std::endl;
      break;
    }
  case 0x0007:
    {
      std::cout << "Case 0x0007" << std::endl;
      
      if ( second_register_value < first_register_value )
      {
        registers->SetRegister(0xf, 0);
      }
      else
      {
        registers->SetRegister(0xf, 1);
      }

      first_register_value = second_register_value - first_register_value;
      registers->SetRegister(first_register, first_register_value);
      //std::cout << "Set register: " << std::hex << first_register << " to " << first_register_value << std::endl;
      break;

    }
  case 0x000e:
    {
      std::cout << "Case 0x000e" << std::endl;

      // set carry register to MSB before shift
      if ( (first_register_value & 0x80) >> 7 )
      {
        registers->SetRegister(0xf, 1);
        //std::cout << "Set carry register to 1" << std::endl;
      }
      else
      {
        registers->SetRegister(0xf, 0);
        //std::cout << "Set carry register to 0" << std::endl;
      }

      first_register_value = (first_register_value << 1) & 0xff;
      registers->SetRegister(first_register, first_register_value);
      //std::cout << "Set register: " << std::hex << first_register << " to " << first_register_value << std::endl;
      break;
    }
  }
}

/*
** 9XY0 - Skips the next instruction if VX doesn't equal VY.
*/ 
void CPU::Opcode9xxx(uint32 instruction)
{
  uint32 first_register = ( instruction & 0x0f00 ) >> 8;
  uint32 second_register = ( instruction & 0x00f0 ) >> 4;
  std::cout << "Opcode9xxx, first reg " << std::hex << first_register << " second reg " << second_register << std::endl;
  uint32 first_register_value = registers->ReadRegister(first_register);
  uint32 second_register_value = registers->ReadRegister(second_register);
  
  if ( first_register_value != second_register_value )
  {
    registers->SetInstructionPointer( registers->ReadInstructionPointer() + 4 );
    instruction_jump = true;
  }
}

/*
** ANNN - Sets I to the address NNN.
*/ 
void CPU::OpcodeAxxx(uint32 instruction)
{
  uint32 data_value = instruction & 0x0fff;
  std::cout << "OpcodeAxxx: Setting address register to " << std::hex << data_value << std::endl;
  registers->SetAddressRegister(data_value);
}

/*
** BNNN - Jumps to the address NNN plus V0.
*/ 
void CPU::OpcodeBxxx(uint32 instruction)
{
  uint32 register_number = 0;
  std::cout << "OpcodeCxxx with register: " << std::hex << register_number << std::endl;
  uint32 data_value = instruction & 0x0fff;
  uint32 register_value = registers->ReadRegister(register_number);
  std::cout << "OpcodeBxxx setting IP to " << std::hex << data_value + register_value << std::endl;
  registers->SetInstructionPointer(data_value + register_value);
  instruction_jump = true;
}

/*
** CXNN - Sets VX to a random number and NN.
*/ 
void CPU::OpcodeCxxx(uint32 instruction)
{
  uint32 register_number = (instruction & 0x0f00) >> 8;
  uint32 value = (instruction & 0x00ff);
  std::cout << "OpcodeCxxx with register: " << std::hex << register_number << std::endl;
  srand (time(NULL));
  uint32 random_number = rand() % 255 + 0;
  //std::cout << "random number is: " << std::hex << random_number << " NN is: " << value << std::endl;
  random_number = random_number & value;
  //std::cout << "storing: " << random_number << std::endl;
  registers->SetRegister(register_number, random_number);
}

/*
** DXYN - Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
**        Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change 
**        after the execution of this instruction. As described above, VF is set to 1 if any screen pixels 
**        are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen.
*/ 
void CPU::OpcodeDxxx(uint32 instruction)
{
  uint32 x_register = (instruction & 0x0f00) >> 8;
  uint32 y_register = (instruction & 0x00f0) >> 4;
  uint32 sprite_height = (instruction & 0x000f);
  uint32 sprite_xcord = registers->ReadRegister(x_register);
  uint32 sprite_ycord = registers->ReadRegister(y_register);

  std::cout << "OpcodeDxxx: xreg: " << std::hex << (uint32)x_register << " yreg " << (uint32)y_register << std::endl;
  std::cout << "sprite_xcord: " << std::hex << (uint32)sprite_xcord << " sprite_ycord " << (uint32)sprite_ycord << " height " << (uint32)sprite_height << std::endl;

  DrawSprite(sprite_xcord, sprite_ycord, sprite_height);

  if( display->PixelFlipped() )
  {
    registers->SetRegister(0xf, 1);
  }
  else
  {
    registers->SetRegister(0xf, 0);
  }

}


void CPU::DrawSprite(uint32 x_cord, uint32 y_cord, uint32 height)
{
  uint16 sprite_data_start = registers->ReadAddressRegister();

  //std::cout << "Data starts at: " << std::hex << sprite_data_start << std::endl;

  for( int i = 0; i < height; i++ )
  {
    uint32 pixel_data = memory->GetMemoryByte(sprite_data_start + i);
    //std::cout << "byte i = " << i << " is: " << std::hex << (unsigned int)memory->GetMemoryByte(sprite_data_start + i) << std::endl;

    bool flip_pixel = 0;
    for( int j = 7; j >=0 ; j--)
    {
      //std::cout << "j: " << std::hex << (uint32)j << std::endl; 
      if ( j == 0)
      {
        flip_pixel = pixel_data & 0x1;
      }
      else
      {
         flip_pixel = ( pixel_data & (0x1 << j) ) >> j;
      }

      if ( flip_pixel )
      {
        //std::cout << "flipping j=" << j << " height " << i << std::endl;
        //std::cout << "flipping xcord: " << x_cord << " ycord " << y_cord << std::endl;
        display->FlipPixel(x_cord + ( 7 - j  ), y_cord);
      }
    }

    //display->FlipPixel();
    y_cord++;
  }
}


/*
** EX9E - Skips the next instruction if the key stored in VX is pressed.
** EXA1 - Skips the next instruction if the key stored in VX isn't pressed.
*/ 
void CPU::OpcodeExxx(uint32 instruction)
{
  uint32 register_number = (instruction & 0x0f00) >> 8;
  std::cout << "OpcodeExxx: register - " << std::hex << register_number << std::endl;

  switch( instruction & 0x00ff )
  {
  case 0x009e:
    {
      std::cout << "case 0x009e, skipping if key in register is pressed" << std::endl;
      uint32 key = registers->ReadRegister(register_number);
      uint32 key_pressed = keyboard->PollKeyPressed();
      //std::cout << "Key pressed is: " << std::hex << key_pressed << " Checked key is: " << key << std::endl;
      
      if ( key_pressed == key )
      {
        //std::cout << "Skipping next instruction" << std::endl;
        registers->SetInstructionPointer(registers->ReadInstructionPointer() + 4 );
        instruction_jump = true;
      }
      break;
    }
  case 0x00a1:
    {
      std::cout << "case 0x00a1, skipping if key in register is not pressed" << std::endl;
      uint32 key = registers->ReadRegister(register_number);
      uint32 key_pressed = keyboard->PollKeyPressed();
      //std::cout << "Key pressed is: " << std::hex << key_pressed << " Checked key is: " << key << std::endl;
      
      if ( key_pressed != key )
      {
        //std::cout << "Skipping next instruction" << std::endl;
        registers->SetInstructionPointer(registers->ReadInstructionPointer() + 4 );
        instruction_jump = true;
      }
      break;
    }
  }
}

/*
** FX07 - Sets VX to the value of the delay timer.
** FX0A - A key press is awaited, and then stored in VX.
** FX15 - Sets the delay timer to VX.
** FX18 - Sets the sound timer to VX.
** FX1E - Adds VX to I.
** FX29 - Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
** FX33 - Stores the Binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle 
**        digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the 
**        hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
** FX55 - Stores V0 to VX in memory starting at address I.
** FX65 - Fills V0 to VX with values from memory starting at address I.
*/ 
void CPU::OpcodeFxxx(uint32 instruction)
{
  std::cout << "In OpcodeFxxx" << std::endl;
  switch( instruction & 0x00ff )
  {
  case 0x0007:
    {
      uint32 register_number = ( 0x0f00 & instruction ) >> 8;
      std::cout << "In case 0x0007 with register; " << std::hex << register_number << std::endl;
      uint32 delay_timer = timers->GetDelayTimerMs();
      if ( delay_timer < 255 )
      {
         registers->SetRegister(register_number, delay_timer);
      }
      else
      {
        registers->SetRegister(register_number, 255);
      }
      //std::cout << "set register to: " << std::hex << (uint32)registers->ReadRegister(register_number) << std::endl;
      break;
    }
  case 0x000a:
    {
      uint32 register_number = ( 0x0f00 & instruction ) >> 8;
      std::cout << "In case 0x000a with register; " << std::hex << register_number << std::endl;
      
      uint8 key_pressed = keyboard->PollKeyPressed();

      // wait for key press
      while ( key_pressed == 0xff )
      {
        key_pressed = keyboard->PollKeyPressed();
      }

      registers->SetRegister(register_number, key_pressed);
      break;
    }
  case 0x0015:
    {
      uint32 register_number = ( 0x0f00 & instruction ) >> 8;
      std::cout << "In case 0x0015 with register; " << std::hex << register_number << std::endl;
      uint32 timer_value = registers->ReadRegister(register_number);
      //std::cout << "cpu: Setting delay timer to: " << std::hex << (timer_value * 1000) << std::endl;
      // CLock frequency is 60Hz
      timers->SetDelayTimerMs(timer_value * 1000);
      //std::cout << "The delay timer is: " << std::hex << timers->GetDelayTimerMs() << std::endl;
      break;
    }
  case 0x0018:
    {
      uint32 register_number = ( 0x0f00 & instruction ) >> 8;
      uint32 register_value = registers->ReadRegister(register_number);
      std::cout << "In case 0x0018 with register: " << std::hex << register_number << std::endl;
      //std::cout << "Setting sound timer to: " << std::hex << register_value << std::endl;
      timers->SetSoundTimerMs(register_value * 1000);
      break;
    }
  case 0x001e:
    {
      uint32 register_number = ( 0x0f00 & instruction ) >> 8;
      std::cout << "In case 0x001e with register; " << std::hex << register_number << std::endl;
      uint32 register_value = registers->ReadRegister(register_number);
      uint16 prev_address_value = registers->ReadAddressRegister();
      registers->SetAddressRegister(prev_address_value + register_value);
      break;
    }
  case 0x0029:
    {
      uint32 register_number = ( 0x0f00 & instruction ) >> 8;
      std::cout << "In case 0x0029 with register; " << std::hex << register_number << std::endl;
      uint32 register_value = registers->ReadRegister(register_number);
      //std::cout << "The register value is: " << std::hex << register_value << std::endl;
      // Each font is 5 bytes long
      registers->SetAddressRegister(register_value * 5 );
      //std::cout << "Set address register to: " << std::hex << registers->ReadAddressRegister() << std::endl;
      //std::cout << "Data value at mem location is: " << std::hex << (uint32)memory->GetMemoryByte(registers->ReadAddressRegister()) << std::endl;
      break;
    }
  case 0x0033:
    {
      uint32 register_number = ( 0x0f00 & instruction ) >> 8;
      std::cout << "In case 0x0033 with register; " << std::hex << register_number << std::endl;
      uint32 register_value = registers->ReadRegister(register_number);
      uint16 memory_address = registers->ReadAddressRegister();
      //std::cout << "The register value is: " << std::hex << register_value << "The memory location is: " << memory_address << std::endl;

      uint32 hundreds_place = register_value / 100;
      uint32 tens_place = ( register_value / 10 ) % 10;
      uint32 ones_place = ( register_value % 100 ) % 10;
      memory->SetMemoryByte(memory_address, hundreds_place);
      memory->SetMemoryByte(memory_address + 1, tens_place);
      memory->SetMemoryByte(memory_address + 2, ones_place);
      //std::cout << "100 mem value: " << std::hex << (unsigned int)memory->GetMemoryByte(memory_address) << std::endl;
      //std::cout << "10 mem value: " << std::hex << (unsigned int)memory->GetMemoryByte(memory_address + 1) << std::endl;
      //std::cout << "1 mem value: " << std::hex << (unsigned int)memory->GetMemoryByte(memory_address + 2) << std::endl;
      break;
    }
  case 0x0055:
    {
      uint32 register_number = ( 0x0f00 & instruction ) >> 8;
      uint16 memory_address = registers->ReadAddressRegister();
      std::cout << "In case 0x0055 with register; " << std::hex << register_number << std::endl;
      for ( int i = 0; i <= register_number; i++ )
      {
        uint32 register_value = registers->ReadRegister(i);
        memory->SetMemoryByte(memory_address + i, register_value);
        //std::cout << "writing to register: " << std::hex << i << " the value: " << memory_value << std::endl;
      }
      break;
    }
  case 0x0065:
    {
      uint32 register_number = ( 0x0f00 & instruction ) >> 8;
      uint16 memory_address = registers->ReadAddressRegister();
      std::cout << "In case 0x0065 with register; " << std::hex << register_number << std::endl;
      for ( int i = 0; i <= register_number; i++ )
      {
        uint32 memory_value = memory->GetMemoryByte(memory_address + i);
        registers->SetRegister(i, memory_value);
        //std::cout << "writing to register: " << std::hex << i << " the value: " << memory_value << std::endl;
      }
      registers->SetAddressRegister(registers->ReadAddressRegister() + register_number + 1);
      break;
    }
  }
}


void CPU::CpuTest()
{
  Initialize();
  display->RefreshScreen();

  bool test_status = false;

  /* 0NNN 	Calls RCA 1802 program at address NNN. */
//not implemented yet

/* 00E0 	Clears the screen. */
//not implemented yet

/* 00EE 	Returns from a subroutine. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 00EE TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 00EE: Return from subroutine" << std::endl;
  std::cout << "(CPU TEST)Pushing address 0x222 into the call stack" << std::endl;
  call_stack->PushAddress(0x222);
  std::cout << "(CPU TEST)Pushing address 0x333 into the call stack" << std::endl;
  call_stack->PushAddress(0x333);
  std::cout << "(CPU TEST)Calling cpu opcode 00EE" << std::endl;
  (this->*opcode_map[0x00])(0x00EE);
  if ( registers->ReadInstructionPointer() == 0x333 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 00EE set Instruction pointer to 0x333" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 00EE did not set Instruction pointer to 0x333" << std::endl;
  }
  (this->*opcode_map[0x00])(0x00EE);

  if ( registers->ReadInstructionPointer() == 0x222 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 00EE set Instruction pointer to 0x222" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 00EE did not set Instruction pointer to 0x222" << std::endl;
  }
  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 00EE Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 00EE Tests FAIL" << std::endl;
  }


/* 1NNN 	Jumps to address NNN. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 1NNN TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 1NNN: Jump to address NNN" << std::endl;
  std::cout << "(CPU TEST)Calling cpu opcode 1NNN to jump to address 0x111" << std::endl;
  (this->*opcode_map[0x01])(0x1111);
  if ( registers->ReadInstructionPointer() == 0x111 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 1NNN set Instruction pointer to 0x111" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 1NNN did not set Instruction pointer to 0x111" << std::endl;
  }
  std::cout << "(CPU TEST)Calling cpu opcode 1NNN to jump to address 0x222" << std::endl;
  (this->*opcode_map[0x01])(0x1222);
  if ( registers->ReadInstructionPointer() == 0x222 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 1NNN set Instruction pointer to 0x222" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 1NNN did not set Instruction pointer to 0x222" << std::endl;
  }
  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 1NNN Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 1NNN Tests FAIL" << std::endl;
  }

/* 2NNN 	Calls subroutine at NNN. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 2NNN TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 2NNN: Call subroutine at NNN" << std::endl;
  std::cout << "(CPU TEST)Setting Instruction Pointer to start at 0x200" << std::endl;
  registers->SetInstructionPointer(0x200);
  std::cout << "(CPU TEST)Calling cpu opcode 2NNN to go to subroutine at address 0x111" << std::endl;

  (this->*opcode_map[0x02])(0x2111);
  if ( registers->ReadInstructionPointer() == 0x111)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 2NNN set Instruction pointer to 0x111" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 2NNN did not set Instruction pointer to 0x111" << std::endl;
  }

  std::cout << "(CPU TEST)Calling cpu opcode 2NNN to go to subroutine at address 0x333" << std::endl;
  (this->*opcode_map[0x02])(0x2333);

  if ( registers->ReadInstructionPointer() == 0x333 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 2NNN set Instruction pointer to 0x333" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 2NNN did not set Instruction pointer to 0x333" << std::endl;
  }

  // Account for IP increment done to move IP before subroutine call
  if( call_stack->PopAddress() == (0x111 + 2) )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 2NNN pushed address 0x113 into the call stack" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 2NNN did not push address 0x113 into the call stack" << std::endl;
  }

  // Account for IP increment done to move IP before subroutine call
  if( call_stack->PopAddress() == (0x200 + 2) )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 2NNN pushed address 0x202 into the call stack" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 2NNN did not push address 0x202 into the call stack" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 2NNN Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 2NNN Tests FAIL" << std::endl;
  }

/* 3XNN 	Skips the next instruction if VX equals NN. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 3XNN TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 3XNN: Skip the next instruction if VX equals NN" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x0" << std::endl;
  registers->SetRegister(0x0,0x0);
  std::cout << "(CPU TEST)Setting Instruction Pointer to start at 0x200" << std::endl;
  registers->SetInstructionPointer(0x200);

  std::cout << "(CPU TEST)Calling opcode 3XNN with NN=0x1" << std::endl;
  (this->*opcode_map[0x03])(0x3001);
  if ( registers->ReadInstructionPointer() == 0x200)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 3XNN did not skip the next instruction" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 3XNN skipped the next instruction" << std::endl;
  }

  std::cout << "(CPU TEST)Setting register 0 with value 0x99" << std::endl;
  registers->SetRegister(0x0,0x99);
  std::cout << "(CPU TEST)Calling opcode 3XNN with NN=0x99" << std::endl;
  (this->*opcode_map[0x03])(0x3099);
  if ( registers->ReadInstructionPointer() == 0x204)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 3XNN skipped the next instruction" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 3XNN did not skip the next instruction" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 3XNN Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 3XNN Tests FAIL" << std::endl;
  }


/* 4XNN 	Skips the next instruction if VX doesn't equal NN. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 4XNN TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 4XNN: Skip the next instruction if VX doesn`t equal NN" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x0" << std::endl;
  registers->SetRegister(0x0,0x0);
  std::cout << "(CPU TEST)Setting Instruction Pointer to start at 0x200" << std::endl;
  registers->SetInstructionPointer(0x200);

  std::cout << "(CPU TEST)Calling opcode 4XNN with NN=0x1" << std::endl;
  (this->*opcode_map[0x04])(0x4001);
  if ( registers->ReadInstructionPointer() == 0x204)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 4XNN skipped the next instruction" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 4XNN did not skip the next instruction" << std::endl;
  }

  std::cout << "(CPU TEST)Setting Instruction Pointer to start at 0x200" << std::endl;
  registers->SetInstructionPointer(0x200);
  std::cout << "(CPU TEST)Setting register 0 with value 0x99" << std::endl;
  registers->SetRegister(0x0,0x99);
  std::cout << "(CPU TEST)Calling opcode 4XNN with NN=0x99" << std::endl;
  (this->*opcode_map[0x04])(0x4099);
  if ( registers->ReadInstructionPointer() == 0x200)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 4XNN did not skip the next instruction" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 4XNN skipped the next instruction" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 4XNN Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 4XNN Tests FAIL" << std::endl;
  }

/* 5XY0 	Skips the next instruction if VX equals VY. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 5XY0 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 5XY0: Skip the next instruction if VX equals VY" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x0" << std::endl;
  registers->SetRegister(0x0,0x0);
  std::cout << "(CPU TEST)Setting register 1 with value 0x1" << std::endl;
  registers->SetRegister(0x1,0x1);
  std::cout << "(CPU TEST)Setting Instruction Pointer to start at 0x200" << std::endl;
  registers->SetInstructionPointer(0x200);

  std::cout << "(CPU TEST)Calling opcode 5XY0 with registers 0 and 1" << std::endl;
  (this->*opcode_map[0x05])(0x5010);
  if ( registers->ReadInstructionPointer() == 0x200)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 5XY0 did not skip the next instruction" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 5XY0 skipped the next instruction" << std::endl;
  }

  std::cout << "(CPU TEST)Setting register 0 with value 0x99" << std::endl;
  registers->SetRegister(0x0,0x99);
  std::cout << "(CPU TEST)Setting register 1 with value 0x1" << std::endl;
  registers->SetRegister(0x1,0x99);
  std::cout << "(CPU TEST)Calling opcode 5XY0 with registers 0 and 1" << std::endl;
  (this->*opcode_map[0x05])(0x5010);
  if ( registers->ReadInstructionPointer() == 0x204)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 5XY0 skipped the next instruction" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 5XY0 did not skip the next instruction" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 5XY0 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 5XY0 Tests FAIL" << std::endl;
  }
/* 6XNN 	Sets VX to NN. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 6XNN TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 6XNN: Set VX to NN" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x0" << std::endl;
  registers->SetRegister(0x0,0x0);

  std::cout << "(CPU TEST)Calling opcode 6XNN with register 0 and NN=0x10" << std::endl;
  (this->*opcode_map[0x06])(0x6010);

  if ( registers->ReadRegister(0) == 0x10)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 6XNN set register 0 to 0x10" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 6XNN did not set register 0 to 0x10" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 6XNN Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 6XNN Tests FAIL" << std::endl;
  }

/* 7XNN 	Adds NN to VX. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 7XNN TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 7XNN: Add NN to VX" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x0" << std::endl;
  registers->SetRegister(0x0,0x0);

  std::cout << "(CPU TEST)Calling opcode 7XNN with register 0 and NN=0x10" << std::endl;
  (this->*opcode_map[0x07])(0x7010);

  if ( registers->ReadRegister(0) == 0x10)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 7XNN set register 0 to 0x10" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 7XNN did not set register 0 to 0x10" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 7XNN Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 7XNN Tests FAIL" << std::endl;
  }


/* 8XY0 	Sets VX to the value of VY. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 8XY0 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 8XY0: Set VX to the value of VY" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x0" << std::endl;
  registers->SetRegister(0x0,0x0);
  std::cout << "(CPU TEST)Setting register 1 with value 0x55" << std::endl;
  registers->SetRegister(0x1,0x55);

  std::cout << "(CPU TEST)Calling opcode 8XY0 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8010);

  if ( registers->ReadRegister(0) == 0x55)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY0 set register 0 to 0x55" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY0 did not set register 0 to 0x55" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 8XY0 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 8XY0 Tests FAIL" << std::endl;
  }


/* 8XY1 	Sets VX to VX or VY. */

  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 8XY1 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 8XY1: Set VX to the value or VY" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x0" << std::endl;
  registers->SetRegister(0x0,0x0);
  std::cout << "(CPU TEST)Setting register 1 with value 0x55" << std::endl;
  registers->SetRegister(0x1,0x55);

  std::cout << "(CPU TEST)Calling opcode 8XY1 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8011);

  if ( registers->ReadRegister(0) == 0x55)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY1 set register 0 to 0x55" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY1 did not set register 0 to 0x55" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 8XY1 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 8XY1 Tests FAIL" << std::endl;
  }

/* 8XY2 	Sets VX to VX and VY. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 8XY2 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 8XY2: Set VX to the value and VY" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x0" << std::endl;
  registers->SetRegister(0x0,0x0);
  std::cout << "(CPU TEST)Setting register 1 with value 0x55" << std::endl;
  registers->SetRegister(0x1,0x55);

  std::cout << "(CPU TEST)Calling opcode 8XY2 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8012);

  if ( registers->ReadRegister(0) == 0x0)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY2 set register 0 to 0x0" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY2 did not set register 0 to 0x0" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 8XY2 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 8XY2 Tests FAIL" << std::endl;
  }

/* 8XY3 	Sets VX to VX xor VY. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 8XY3 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 8XY3: Set VX to the value xor VY" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x0" << std::endl;
  registers->SetRegister(0x0,0x0);
  std::cout << "(CPU TEST)Setting register 1 with value 0x55" << std::endl;
  registers->SetRegister(0x1,0x55);

  std::cout << "(CPU TEST)Calling opcode 8XY3 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8013);

  if ( registers->ReadRegister(0) == 0x55)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY3 set register 0 to 0x55" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY3 did not set register 0 to 0x55" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 8XY3 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 8XY3 Tests FAIL" << std::endl;
  }

/* 8XY4 	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 8XY4 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 8XY4: Adds VY to VX" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x0" << std::endl;
  registers->SetRegister(0x0,0x0);
  std::cout << "(CPU TEST)Setting register 1 with value 0x55" << std::endl;
  registers->SetRegister(0x1,0x55);

  std::cout << "(CPU TEST)Calling opcode 8XY4 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8014);

  if ( registers->ReadRegister(0) == 0x55)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY4 set register 0 to 0x55" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY4 did not set register 0 to 0x55" << std::endl;
  }

  if ( registers->ReadRegister(0xf) == 0)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY4 set carry register to 0" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY4 set carry register to 1" << std::endl;
  }  

  std::cout << "(CPU TEST)Setting register 0 with value 0xee" << std::endl;
  registers->SetRegister(0x0,0xee);
  std::cout << "(CPU TEST)Setting register 1 with value 0xee" << std::endl;
  registers->SetRegister(0x1,0xee);

  std::cout << "(CPU TEST)Calling opcode 8XY4 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8014);
  if ( registers->ReadRegister(0) == 0xdc)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY4 set register 0 to 0xdd" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY4 did not set register 0 to 0xdd" << std::endl;
  }

  if ( registers->ReadRegister(0xf) == 1)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY4 set carry register to 1" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY4 set carry register to 0" << std::endl;
  }  

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 8XY4 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 8XY4 Tests FAIL" << std::endl;
  }
/* 8XY5 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 8XY5 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 8XY5: VY is subtracted from VX" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x0" << std::endl;
  registers->SetRegister(0x0,0x0);
  std::cout << "(CPU TEST)Setting register 1 with value 0x55" << std::endl;
  registers->SetRegister(0x1,0x55);

  std::cout << "(CPU TEST)Calling opcode 8XY5 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8015);
  std::cout << "reg value is: " << std::hex << (uint32)registers->ReadRegister(0) << std::endl;
  if ( registers->ReadRegister(0) == 0xab)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY5 set register 0 to 0xab" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY5 did not set register 0 to 0xab" << std::endl;
  }

  if ( registers->ReadRegister(0xf) == 0)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY5 set carry register to 0" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY5 set carry register to 1" << std::endl;
  }  

  std::cout << "(CPU TEST)Setting register 0 with value 0xee" << std::endl;
  registers->SetRegister(0x0,0x5);
  std::cout << "(CPU TEST)Setting register 1 with value 0xee" << std::endl;
  registers->SetRegister(0x1,0x1);
    std::cout << "reg value is: " << std::hex << (uint32)registers->ReadRegister(0) << std::endl;
  std::cout << "(CPU TEST)Calling opcode 8XY5 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8015);
  if ( registers->ReadRegister(0) == 0x4)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY5 set register 0 to 0x4" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY5 did not set register 0 to 0x4" << std::endl;
  }

  if ( registers->ReadRegister(0xf) == 1)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY5 set carry register to 1" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY5 set carry register to 0" << std::endl;
  }  

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 8XY5 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 8XY5 Tests FAIL" << std::endl;
  }


/* 8XY6 	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 8XY6 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 8XY6: Shift VX right by one" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x55" << std::endl;
  registers->SetRegister(0x0,0x55);
  std::cout << "(CPU TEST)Setting register 1 with value 0x55" << std::endl;
  registers->SetRegister(0x1,0x55);

  std::cout << "(CPU TEST)Calling opcode 8XY6 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8016);
  std::cout << "reg value is: " << std::hex << (uint32)registers->ReadRegister(0) << std::endl;
  if ( registers->ReadRegister(0) == 0x2a)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY6 set register 0 to 0x2a" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY6 did not set register 0 to 0x2a" << std::endl;
  }
  if ( registers->ReadRegister(0xf) == 1)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY6 set carry register to 1" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY6 set carry register to 0" << std::endl;
  }  

  std::cout << "(CPU TEST)Setting register 0 with value 0x04" << std::endl;
  registers->SetRegister(0x0,0x04);
  std::cout << "(CPU TEST)Setting register 1 with value 0x55" << std::endl;
  registers->SetRegister(0x1,0x55);

  std::cout << "(CPU TEST)Calling opcode 8XY6 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8016);
  std::cout << "reg value is: " << std::hex << (uint32)registers->ReadRegister(0) << std::endl;
  if ( registers->ReadRegister(0) == 0x02)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY6 set register 0 to 0x02" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY6 did not set register 0 to 0x02" << std::endl;
  }
  if ( registers->ReadRegister(0xf) == 0)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY6 set carry register to 0" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY6 set carry register to 1" << std::endl;
  }  

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 8XY6 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 8XY6 Tests FAIL" << std::endl;
  }
/* 8XY7 	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 8XY7 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 8XY7: Set VX to VY minus VX" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x55" << std::endl;
  registers->SetRegister(0x0,0x55);
  std::cout << "(CPU TEST)Setting register 1 with value 0x0" << std::endl;
  registers->SetRegister(0x1,0x0);
  
  std::cout << "(CPU TEST)Calling opcode 8XY7 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8017);
  std::cout << "reg value is: " << std::hex << (uint32)registers->ReadRegister(0) << std::endl;
  if ( registers->ReadRegister(0) == 0xab)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY7 set register 0 to 0xab" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY7 did not set register 0 to 0xab" << std::endl;
  }
  if ( registers->ReadRegister(0xf) == 0)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY7 set carry register to 0" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY7 set carry register to 1" << std::endl;
  }

  std::cout << "(CPU TEST)Setting register 0 with value 0x1" << std::endl;
  registers->SetRegister(0x0,0x1);
  std::cout << "(CPU TEST)Setting register 1 with value 0x5" << std::endl;
  registers->SetRegister(0x1,0x5);
  
  std::cout << "(CPU TEST)Calling opcode 8XY7 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x8017);
  std::cout << "reg value is: " << std::hex << (uint32)registers->ReadRegister(0) << std::endl;
  if ( registers->ReadRegister(0) == 0x4)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY7 set register 0 to 0x4" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY7 did not set register 0 to 0x4" << std::endl;
  }
  if ( registers->ReadRegister(0xf) == 1)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XY7 set carry register to 1" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XY7 set carry register to 0" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 8XY7 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 8XY7 Tests FAIL" << std::endl;
  }

/* 8XYE 	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 8XYE TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 8XYE: Shift VX left by one" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x55" << std::endl;
  registers->SetRegister(0x0,0x55);
  std::cout << "(CPU TEST)Setting register 1 with value 0x0" << std::endl;
  registers->SetRegister(0x1,0x0);
  
  std::cout << "(CPU TEST)Calling opcode 8XYE with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x801E);
  std::cout << "reg value is: " << std::hex << (uint32)registers->ReadRegister(0) << std::endl;
  if ( registers->ReadRegister(0) == 0xaa)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XYE set register 0 to 0xaa" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XYE did not set register 0 to 0xaa" << std::endl;
  }
  if ( registers->ReadRegister(0xf) == 0)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XYE set carry register to 0" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XYE set carry register to 1" << std::endl;
  }

  std::cout << "(CPU TEST)Setting register 0 with value 0x85" << std::endl;
  registers->SetRegister(0x0,0x85);
  std::cout << "(CPU TEST)Setting register 1 with value 0x0" << std::endl;
  registers->SetRegister(0x1,0x0);
  
  std::cout << "(CPU TEST)Calling opcode 8XYE with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x801E);
  std::cout << "reg value is: " << std::hex << (uint32)registers->ReadRegister(0) << std::endl;
  if ( registers->ReadRegister(0) == 0x0a)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XYE set register 0 to 0x0a" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XYE did not set register 0 to 0x0a" << std::endl;
  }
  if ( registers->ReadRegister(0xf) == 1)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 8XYE set carry register to 1" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 8XYE set carry register to 0" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 8XYE Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 8XYE Tests FAIL" << std::endl;
  }

/* 9XY0 	Skips the next instruction if VX doesn't equal VY. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE 9XY0 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode 9XY0: Skip the next instruction if VX doesn't equal VY" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x05" << std::endl;
  registers->SetRegister(0x0,0x05);
  std::cout << "(CPU TEST)Setting register 1 with value 0x05" << std::endl;
  registers->SetRegister(0x1,0x05);
  std::cout << "(CPU TEST)Setting Instruction Pointer to 0x200" << std::endl;
  registers->SetInstructionPointer(0x200);
  std::cout << "(CPU TEST)Calling opcode 9XY0 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x08])(0x9010);

  if ( registers->ReadInstructionPointer() == 0x200 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 9XY0 did not set Instruction pointer to skip next instruction" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 9XY0 did set Instruction pointer to skip next instruction" << std::endl;
  }

  std::cout << "(CPU TEST)Setting register 0 with value 0x01" << std::endl;
  registers->SetRegister(0x0,0x01);
  std::cout << "(CPU TEST)Setting register 1 with value 0x05" << std::endl;
  registers->SetRegister(0x1,0x05);
  std::cout << "(CPU TEST)Setting Instruction Pointer to 0x200" << std::endl;
  registers->SetInstructionPointer(0x200);
  std::cout << "(CPU TEST)Calling opcode 9XY0 with register 0 and register 1" << std::endl;
  (this->*opcode_map[0x09])(0x9010);
  
  if ( registers->ReadInstructionPointer() == 0x204 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode 9XY0 did set Instruction pointer to skip next instruction" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode 9XY0 did not set Instruction pointer to skip next instruction" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode 9XY0 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode 9XY0 Tests FAIL" << std::endl;
  }

/* ANNN 	Sets I to the address NNN. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE ANNN TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode ANNN: Set I to the address NNN" << std::endl;
  std::cout << "(CPU TEST)Calling opcode ANNN with NNN=0x123" << std::endl;
  (this->*opcode_map[0x0A])(0xA123);

  if( registers->ReadAddressRegister() == 0x123 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode ANNN set Address Register to 0x123" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode ANNN did not set Address Register to 0x123" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode ANNN Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode ANNN Tests FAIL" << std::endl;
  }

/* BNNN 	Jumps to the address NNN plus V0. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE BNNN TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode BNNN: Jump to the address NNN plus V0" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x01" << std::endl;
  registers->SetRegister(0x0,0x01);
  std::cout << "(CPU TEST)Calling opcode BNNN with NNN=0x123" << std::endl;
  (this->*opcode_map[0x0B])(0xB123);

  if( registers->ReadInstructionPointer() == 0x124 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode BNNN set Instruction Pointer to 0x124" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode BNNN did not set Instruction Pointer to 0x124" << std::endl;
  }
  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode BNNN Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode BNNN Tests FAIL" << std::endl;
  }

/* CXNN 	Sets VX to a random number and NN. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE CXNN TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode CXNN: Set VX to a random number and NN" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x01" << std::endl;
  registers->SetRegister(0x0,0x00);
  std::cout << "(CPU TEST)Calling opcode CXNN with NN=0xff" << std::endl;
  (this->*opcode_map[0x0C])(0xC0ff);
  std::cout << "reg value is: " << std::hex << (uint32)registers->ReadRegister(0) << std::endl;
  // Note may fail depending on random number, less than 1% chance.
  if( registers->ReadRegister(0) != 0x0 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode CXNN changed value in register 0" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode CXNN did not change value in register 0" << std::endl;
  }
  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode CXNN Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode CXNN Tests FAIL" << std::endl;
  }


/* DXYN 	Sprites stored in memory at location in index register (I), maximum 8bits wide. Wraps around the screen. 
If when drawn, clears a pixel, register VF is set to 1 otherwise it is zero. All drawing is XOR drawing (e.g. 
it toggles the screen pixels) */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE DXYN TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode DXYN: Draws data on screen" << std::endl;

  std::cout << "(CPU TEST)Writing to memory at byte 0 and 1 the value 0x80" << std::endl;
  memory->SetMemoryByte(0, 0xff);
  memory->SetMemoryByte(1, 0xff);
  std::cout << "(CPU TEST)Setting address register to 0" << std::endl;
  registers->SetAddressRegister(0);

  std::cout << "(CPU TEST)Setting register 0 with value 0x03" << std::endl;
  registers->SetRegister(0x0,0x38);
  std::cout << "(CPU TEST)Setting register 1 with value 0x06" << std::endl;
  registers->SetRegister(0x1,0x1e);
  std::cout << "(CPU TEST)Calling opcode DXYN with register 0 and 1, height 2" << std::endl;
  (this->*opcode_map[0x0D])(0xD012);

  if(registers->ReadRegister(0xf) == 0)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode DXYN set the carry register to 0" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode DXYN set the carry register to 1" << std::endl;
  }
  display->RefreshScreen();

  std::cout << "(CPU TEST)Calling opcode DXYN with register 0 and 1, height 2" << std::endl;
  (this->*opcode_map[0x0D])(0xD012);

  if(registers->ReadRegister(0xf) == 1)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode DXYN set the carry register to 1" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode DXYN set the carry register to 0" << std::endl;
  }
  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode DXYN Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode DXYN Tests FAIL" << std::endl;
  }
  display->RefreshScreen();

  std::cout << "(CPU TEST)Calling opcode DXYN with register 0 and 1, height 2" << std::endl;
  (this->*opcode_map[0x0D])(0xD012);

  if(registers->ReadRegister(0xf) == 0)
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode DXYN set the carry register to 0" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode DXYN set the carry register to 1" << std::endl;
  }
  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode DXYN Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode DXYN Tests FAIL" << std::endl;
  }
  display->RefreshScreen();

/* EX9E 	Skips the next instruction if the key stored in VX is pressed. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE EX9E TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode EX9E: Skip the next instruction if the key stored in VX is pressed" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x03" << std::endl;
  registers->SetRegister(0x0,0x03);
  std::cout << "(CPU TEST)Setting Instruction Pointer to 0x200" << std::endl;
  registers->SetInstructionPointer(0x200);
  std::cout << "(CPU TEST)Calling opcode EX9E with register 0" << std::endl;
  (this->*opcode_map[0x0E])(0xE09E);

  if( registers->ReadInstructionPointer() == 0x200 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode EX9E did not set Instruction Pointer" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode EX9E incremented the Instruction Pointer" << std::endl;
  }
  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode EX9E Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode EX9E Tests FAIL" << std::endl;
  }

/* EXA1 	Skips the next instruction if the key stored in VX isn't pressed. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE EXA1 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode EX9E: Skip the next instruction if the key stored in VX isn't pressed" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x03" << std::endl;
  registers->SetRegister(0x0,0x03);
  std::cout << "(CPU TEST)Setting Instruction Pointer to 0x200" << std::endl;
  registers->SetInstructionPointer(0x200);
  std::cout << "(CPU TEST)Calling opcode EX9E with register 0" << std::endl;
  (this->*opcode_map[0x0E])(0xE0A1);

  if( registers->ReadInstructionPointer() == 0x204 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode EX9E incremented the Instruction Pointer" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode EX9E did not set the Instruction Pointer" << std::endl;
  }
  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode EX9E Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode EX9E Tests FAIL" << std::endl;
  }


/* FX07 	Sets VX to the value of the delay timer. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE FX07 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode FX07: Skip the next instruction if the key stored in VX isn't pressed" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x00" << std::endl;
  registers->SetRegister(0x0,0x00);
  std::cout << "(CPU TEST)Setting Delay Timer to 10" << std::endl; 
  timers->SetDelayTimerMs(1000);
  std::cout << "reg value is: " << std::hex << (uint32)registers->ReadRegister(0) << std::endl;
  std::cout << "(CPU TEST)Calling opcode FX07 with register 0" << std::endl;
  (this->*opcode_map[0x0F])(0xF007);

  if ( registers->ReadRegister(0) != 0x00 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX07 set register 0 with the Delay Timer value" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX07 did not set register 0 with the Delay Timer value" << std::endl;
  }
  
  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode FX07 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode FX07 Tests FAIL" << std::endl;
  }

/* FX0A 	A key press is awaited, and then stored in VX. */
//TODO

/* FX15 	Sets the delay timer to VX. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE FX15 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode FX15: Set the delay timer to VX." << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x00" << std::endl;
  registers->SetRegister(0x0,0x0a);
  std::cout << "(CPU TEST)Calling opcode FX15 with register 0" << std::endl;
  (this->*opcode_map[0x0F])(0xF015);
  std::cout << "(CPU TEST)Value of Delay Timer is: " << std::hex << timers->GetDelayTimerMs() << std::endl;
  if ( timers->GetDelayTimerMs() != 0x00 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX15 set the Delay Timer" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX15 did not set the Delay Timer" << std::endl;
  }
  
  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode FX15 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode FX15 Tests FAIL" << std::endl;
  }

/* FX18 	Sets the sound timer to VX. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE FX18 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode FX18: Set the sound timer to VX." << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x00" << std::endl;
  registers->SetRegister(0x0,0x0a);
  std::cout << "(CPU TEST)Calling opcode FX18 with register 0" << std::endl;
  (this->*opcode_map[0x0F])(0xF018);
  std::cout << "(CPU TEST)Value of Sound Timer is: " << std::hex << timers->GetSoundTimerMs() << std::endl;
  if ( timers->GetSoundTimerMs() != 0x00 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX18 set the Sound Timer" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX18 did not set the Sound Timer" << std::endl;
  }
  
  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode FX18 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode FX18 Tests FAIL" << std::endl;
  }


/* FX1E 	Adds VX to I. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE FX1E TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode FX1E: Add VX to I" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x05" << std::endl;
  registers->SetRegister(0x0,0x05);
  std::cout << "(CPU TEST)Setting Address Register to 0x123" << std::endl;
  registers->SetAddressRegister(0x123);
  std::cout << "(CPU TEST)Calling opcode FX1E with register 0" << std::endl;
  (this->*opcode_map[0x0F])(0xF01E);

  if( registers->ReadAddressRegister() == 0x128 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX1E set address register to 0x128" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX1E did not set address register to 0x128" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode FX1E Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode FX1E Tests FAIL" << std::endl;
  }


/* FX29 	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE FX1E TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode FX29: Set I to the location of the sprite for the character in VX" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x00" << std::endl;
  registers->SetRegister(0x0,0x00);
  std::cout << "(CPU TEST)Setting Address Register to 0x123" << std::endl;
  registers->SetAddressRegister(0x123);
  std::cout << "(CPU TEST)Calling opcode FX29 with register 0" << std::endl;
  (this->*opcode_map[0x0F])(0xF029);

  if( registers->ReadAddressRegister() == 0x00 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX29 set address register to 0x00" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX29 did not set address register to 0x00" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode FX29 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode FX29 Tests FAIL" << std::endl;
  }

/* FX33 	Stores the Binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the 
middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, 
place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.) */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE FX33 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode FX33: Set I to the location of the sprite for the character in VX" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0xaa" << std::endl;
  registers->SetRegister(0x0,0xaa);
  std::cout << "(CPU TEST)Setting Address Register to 0x00" << std::endl;
  registers->SetAddressRegister(0x00);
  std::cout << "(CPU TEST)Calling opcode FX33 with register 0" << std::endl;
  (this->*opcode_map[0x0F])(0xF033);

  if( memory->GetMemoryByte(0) == 0x01 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX33 set memory at 0x0 to 0x01" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX33 did not set memory at 0x0 to 0x01" << std::endl;
  }

  if( memory->GetMemoryByte(1) == 0x07 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX33 set memory at 0x1 to 0x07" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX33 did not set memory at 0x1 to 0x07" << std::endl;
  }

  if( memory->GetMemoryByte(2) == 0x00 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX33 set memory at 0x2 to 0x00" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX33 did not set memory at 0x2 to 0x00" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode FX33 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode FX33 Tests FAIL" << std::endl;
  }

/* FX55 	Stores V0 to VX in memory starting at address I. */
  test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE FX55 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode FX55: Set I to the location of the sprite for the character in VX" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x01" << std::endl;
  registers->SetRegister(0x0,0x01);
  std::cout << "(CPU TEST)Setting register 1 with value 0x02" << std::endl;
  registers->SetRegister(0x1,0x02);
  std::cout << "(CPU TEST)Setting register 2 with value 0x03" << std::endl;
  registers->SetRegister(0x2,0x03);
  std::cout << "(CPU TEST)Setting Address Register to 0x00" << std::endl;
  registers->SetAddressRegister(0x00);
  std::cout << "(CPU TEST)Calling opcode FX55 with register 0" << std::endl;
  (this->*opcode_map[0x0F])(0xF255);

  if( memory->GetMemoryByte(0) == 0x01 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX55 set memory at 0x0 to 0x01" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX55 did not set memory at 0x0 to 0x01" << std::endl;
  }

  if( memory->GetMemoryByte(1) == 0x02 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX55 set memory at 0x1 to 0x02" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX55 did not set memory at 0x1 to 0x02" << std::endl;
  }

  if( memory->GetMemoryByte(2) == 0x03 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX55 set memory at 0x2 to 0x03" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX55 did not set memory at 0x2 to 0x03" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode FX55 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode FX55 Tests FAIL" << std::endl;
  }

/* FX65 	Fills V0 to VX with values from memory starting at address I. */
    test_status = true;
  std::cout << "(CPU TEST)************ START OPCODE FX65 TEST ************" << std::endl;
  std::cout << "(CPU TEST)Testing opcode FX65: Set I to the location of the sprite for the character in VX" << std::endl;
  std::cout << "(CPU TEST)Setting register 0 with value 0x00" << std::endl;
  registers->SetRegister(0x0,0x00);
  std::cout << "(CPU TEST)Setting register 1 with value 0x00" << std::endl;
  registers->SetRegister(0x0,0x00);
  std::cout << "(CPU TEST)Setting register 2 with value 0x00" << std::endl;
  registers->SetRegister(0x0,0x00);
  std::cout << "(CPU TEST)Setting Address Register to 0x00" << std::endl;
  registers->SetAddressRegister(0x00);
  std::cout << "(CPU TEST)Writing memory at 0x0 to 0x01" << std::endl;
  memory->SetMemoryByte(0, 0x1);
  std::cout << "(CPU TEST)Writing memory at 0x0 to 0x02" << std::endl;
  memory->SetMemoryByte(1, 0x2);
  std::cout << "(CPU TEST)Writing memory at 0x0 to 0x03" << std::endl;
  memory->SetMemoryByte(2, 0x3);
  std::cout << "(CPU TEST)Calling opcode FX65 with register 0" << std::endl;
  (this->*opcode_map[0x0F])(0xF265);
  std::cout << "register 0 value is: " << std::hex << (uint32)registers->ReadRegister(0) << std::endl;
  std::cout << "register 1 value is: " << std::hex << (uint32)registers->ReadRegister(1) << std::endl;
  std::cout << "register 2 value is: " << std::hex << (uint32)registers->ReadRegister(2) << std::endl;
  if( registers->ReadRegister(0) == 0x01 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX65 set register 0 to 0x01" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX65 did not set register 0 to 0x01" << std::endl;
  }

  if( registers->ReadRegister(1) == 0x02 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX65 set register 1 to 0x02" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX65 did not set register 1 to 0x02" << std::endl;
  }

  if( registers->ReadRegister(2) == 0x03 )
  {
    std::cout << "(CPU TEST)TEST PASSED: Opcode FX65 set register 2 to 0x03" << std::endl;
  }
  else
  {
    test_status = false;
    std::cout << "(CPU TEST)TEST FAILED: Opcode FX65 did not set register 2 to 0x03" << std::endl;
  }

  if ( test_status )
  {
    std::cout << "(CPU TEST)Opcode FX65 Tests PASSED" << std::endl;
  }
  else
  {
    std::cout << "(CPU TEST)Opcode FX65 Tests FAIL" << std::endl;
  }


  /* END OF TEST CASES */

}

void CPU::CpuReset()
{
  delete memory;
  delete keyboard;
  delete display;
  delete registers;
  delete timers;
  delete clock;
  delete call_stack;

  memory = new Memory;
  keyboard = new Keyboard;
  display = new Display;
  registers = new RegisterModule;
  timers = new TimerModule;
  call_stack = new CallStack;
  clock = new Clock;
}

void CPU::Initialize()
{
  // Initiailize
  memory->LoadRom("PONG");
  keyboard->KeyboardInitialize();
  display->InitializeScreen();
  registers->SetInstructionPointer(512);
  display->ClearScreen();
  clock->StartClock(11);
}