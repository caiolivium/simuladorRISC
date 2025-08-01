#include "memory.h"
#include <stdio.h>

uint16_t memory[MEM_SIZE] = {0};
uint8_t memory_accessed[MEM_SIZE] = {0};

uint16_t read_mem(uint16_t addr) {
    memory_accessed[addr] = 1;

    if (addr == 0xF000) {
        char c;
        scanf(" %c", &c);
        return (uint16_t)c;
    } else if (addr == 0xF001) {
        int x;
        scanf("%d", &x);
        return (uint16_t)x;
    } else if (addr < MEM_SIZE) {
        return memory[addr];
    }
    return 0;
}

void write_mem(uint16_t addr, uint16_t value) {
    memory_accessed[addr] = 1;

    if (addr == 0xF002) {
        printf("%c", (char)value);
    } else if (addr == 0xF003) {
        printf("%d", value);
    } else if (addr < MEM_SIZE) {
        memory[addr] = value;
    }
}
