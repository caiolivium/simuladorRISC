#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "memory.h"
#include "cpu.h"

void load_program(const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }

    char line[32];
    while (fgets(line, sizeof(line), fp)) {
        unsigned int addr, value;
        if (sscanf(line, "%x : 0x%x", &addr, &value) == 2) {
            if (addr < MEM_SIZE)
                memory[addr] = (uint16_t)value;
        }
    }
    fclose(fp);
}

void print_state() {
    printf("Registradores:\n");
    for (int i = 0; i < 16; i++) {
        printf("R%-2d: 0x%04X\n", i, registers[i]);
    }

    printf("\nMemória acessada:\n");
    for (int i = 0; i < MEM_SIZE; i++) {
        if (memory[i] != 0)
            printf("%04X: 0x%04X\n", i, memory[i]);
    }

    if (registers[14] != STACK_START) {
        printf("\nPilha:\n");
        for (uint16_t i = registers[14]; i < STACK_START; i++) {
            printf("%04X: 0x%04X\n", i, memory[i]);
        }
    }

    printf("\nFlags:\n");
    printf("Z = %d\n", FLAG_Z);
    printf("C = %d\n", FLAG_C);
}