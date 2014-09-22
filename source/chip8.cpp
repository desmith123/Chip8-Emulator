// chip8.cpp : Defines the entry point for the console application.
//
#include "cpu.h"

int main(int argc, char* argv[])
{
  CPU * cpu = new CPU;
  cpu->CpuTest();
  cpu->CpuReset();
  cpu->Start();
  getchar();
  return 0;
}
