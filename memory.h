#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define MEM_SIZE 0x10000

extern uint16_t memory[MEM_SIZE];

uint16_t read_mem(uint16_t addr);
void write_mem(uint16_t addr, uint16_t value);

#endif