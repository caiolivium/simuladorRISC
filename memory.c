#include "memory.h"
#include <stdio.h>

uint16_t memory[MEM_SIZE];

uint16_t read_mem(uint16_t addr) {
    if (addr == 0xF000) {
        int x;
        scanf("%d", &x);
        printf("[INT IN] = %d\n", x);  // Adicionado para saída correta
        return (uint16_t)x;
    } else if (addr == 0xF001) {
        int x;
        scanf("%d", &x);
        printf("[INT IN] = %d\n", x);  // Também pode ocorrer leitura de inteiro aqui
        return (uint16_t)x;
    } else if (addr < MEM_SIZE) {
        return memory[addr];
    }
    return 0;
}

void write_mem(uint16_t addr, uint16_t value) {
    if (addr == 0xF002) {
        printf("%c", (char)value);
    } else if (addr == 0xF003) {
        printf("[INT OUT] = %d\n", value);  // Adicionado para saída correta
    } else if (addr < MEM_SIZE) {
        memory[addr] = value;
    }
}
