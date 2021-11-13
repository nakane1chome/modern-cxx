Example bare metal coroutines in C++ for RISC-V.

DO NOT USE. This is a non-functional experiment.

The objective is to enter a main() function and enable a simple periodic ISR 
handler, while staying in machine mode.

Source Files:

- src/startup.cpp          : Entry point from reset. Set up C++ runtime environment.
- src/main.cpp             : Uses  timer interrupt to control 2 concurrent co-routines.
- src/timer.hpp            : Device independent C++ driver for the RISC-V machine mode timer.
- src/riscv-csr.hpp        : C++ class abstraction to access RISC-V CSRs (Generated file)
- src/riscv-interrupts.hpp : List of RISC-V machine mode interrupts.
- src/freestanding.cpp     : Quick & dirty replacement for stdlib functions. 

From GCC (GPL):

- src/div.S,riscv-asm.h    : Divide routines from: libgcc/config/riscv/div.S

Build Files:

- src/CMakeLists.txt       : CMake build file.
- Makefile                 : Makefile to configure and run CMake, and upload and debug on SiFive HiFive revb board.
- cmake/*                  : Cmake support files

Other Files:

- src/linker.lds           : Linker script for SiFive HiFive revb board (from the metal environment).
