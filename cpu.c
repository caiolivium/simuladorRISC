#include "cpu.h"
#include "memory.h"
#include "util.h"
#include <stdio.h>

uint16_t registers[NUM_REGISTERS];
uint16_t IR = 0;
uint8_t FLAG_Z = 0;
uint8_t FLAG_C = 0;

void ula_flags(uint16_t a, uint16_t b, uint16_t result) {
    FLAG_Z = (result == 0);

    uint8_t opcode = (IR >> 12) & 0xF;
    if (opcode == 0x7 || opcode == 0x8 || opcode == 0xD) {
        // SUB, SUBI, CMP
        FLAG_C = (a < b);
    } else if (opcode == 0x5 || opcode == 0x6) {
        // ADD, ADDI
        FLAG_C = (result < a);
    } else {
        FLAG_C = 0;
    }
}

typedef struct {
    uint8_t rd;
    uint8_t rm;
    uint8_t rn;
    uint8_t imm8;
    uint8_t imm4;
} InstrFields;

InstrFields decode_fields(uint16_t IR) {
    InstrFields f;
    f.rd   = (IR >> 8) & 0xF;
    f.rm   = (IR >> 4) & 0xF;
    f.rn   = IR & 0xF;
    f.imm8 = IR & 0xFF;
    f.imm4 = IR & 0xF;
    return f;
}

void execute() {
    registers[14] = STACK_START;
    registers[15] = 0;

    while (1) {
        uint16_t pc = registers[15];
        
        IR = read_mem(pc);
        registers[15]++;
        if (IR == 0xFFFF || IR == 0xFF00) break;

        uint16_t opcode = (IR >> 12) & 0xF;

        InstrFields f = decode_fields(IR);
        switch (opcode) {
            // JUMPs e Condicionais
            case 0x0: {
                uint16_t cond = (IR >> 10) & 0x3;
                int16_t offset = (int16_t)(IR & 0x03FF);
                if (offset & 0x0200) offset |= 0xFC00; // sinaliza

                switch ((IR >> 10) & 0x7) {
                    case 0x0: // JMP
                        if (offset == 0) print_state(); // NOP
                        registers[15] += offset;
                        break;
                    case 0x1: if (FLAG_Z) registers[15] += offset; break; // JEQ
                    case 0x2: if (!FLAG_Z) registers[15] += offset; break; // JNE
                    case 0x3: if (!FLAG_Z && FLAG_C) registers[15] += offset; break; // JLT
                    case 0x7: if (FLAG_Z || !FLAG_C) registers[15] += offset; break; // JGE
                }
                break;
            }

            // LDR: Rd = MEM[Rm]
            case 0x2: {
                uint16_t addr = registers[f.rm];
                registers[f.rd] = memory[addr];
                break;
            }
            // STR: MEM[Rm] = Rn
            case 0x3: {
                memory[registers[f.rm]] = registers[f.rn];
                break;
            }
            // MOV Rd, #Im
            case 0x4: {
                registers[f.rd] = f.imm8;
                break;
            }
            // ADD Rd = Rm + Rn
            case 0x5: {
                uint16_t res = registers[f.rm] + registers[f.rn];
                ula_flags(registers[f.rm], registers[f.rn], res);
                registers[f.rd] = res;
                break;
            }
            // ADDI Rd, Rm, #Im
            case 0x6: {
                uint16_t res = registers[f.rm] + f.imm4;
                ula_flags(registers[f.rm], f.imm4, res);
                registers[f.rd] = res;
                break;
            }
            // SUB Rd = Rm - Rn
            case 0x7: {
                uint16_t res = registers[f.rm] - registers[f.rn];
                ula_flags(registers[f.rm], registers[f.rn], res);
                registers[f.rd] = res;
                break;
            }
            // SUBI Rd, Rm, #Im
            case 0x8: {
                uint16_t res = registers[f.rm] - f.imm4;
                ula_flags(registers[f.rm], f.imm4, res);
                registers[f.rd] = res;
                break;
            }
            // AND
            case 0x9: {
                uint16_t res = registers[f.rm] & registers[f.rn];
                ula_flags(registers[f.rm], registers[f.rn], res);
                registers[f.rd] = res;
                break;
            }
            // OR
            case 0xA: {
                uint16_t res = registers[f.rm] | registers[f.rn];
                ula_flags(registers[f.rm], registers[f.rn], res);
                registers[f.rd] = res;
                break;
            }
            // SHR
            case 0xB: {
                uint16_t res = registers[f.rm] >> f.imm4;
                ula_flags(registers[f.rm], f.imm4, res);
                registers[f.rd] = res;
                break;
            }
            // SHL
            case 0xC: {
                uint16_t res = registers[f.rm] << f.imm4;
                ula_flags(registers[f.rm], f.imm4, res);
                registers[f.rd] = res;
                break;
            }
            // CMP Rm, Rn
            case 0xD: {
                uint16_t res = registers[f.rm] - registers[f.rn];
                ula_flags(registers[f.rm], registers[f.rn], res);
                break;
            }
            // PUSH Rn
            case 0xE: {
                registers[14]--;
                memory[registers[14]] = registers[f.rn];
                break;
            }
            // POP Rd
            case 0xF: {
                registers[f.rd] = memory[registers[14]];
                registers[14]++;
                break;
            }
        }
    }
}