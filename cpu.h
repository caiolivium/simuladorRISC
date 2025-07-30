#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#define NUM_REGISTERS 16
#define STACK_START 0x8000

extern uint16_t registers[NUM_REGISTERS];
extern uint16_t IR;
extern uint8_t FLAG_Z;
extern uint8_t FLAG_C;

void execute();
void alu_flags(uint16_t a, uint16_t b, uint16_t result);

#endif