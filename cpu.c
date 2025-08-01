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
    FLAG_C = (result < a);
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
        IR = read_mem(pc);  // <-- substituído
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

            case 0x5: { // ADD
                uint16_t res = registers[f.rm] + registers[f.rn];
                ula_flags(registers[f.rm], registers[f.rn], res);
                registers[f.rd] = res;
                break;
            }

            case 0x6: { // ADDI
                uint16_t res = registers[f.rm] + f.imm4;
                ula_flags(registers[f.rm], f.imm4, res);
                registers[f.rd] = res;
                break;
            }

            case 0x7: { // SUB
                uint16_t res = registers[f.rm] - registers[f.rn];
                ula_flags(registers[f.rm], registers[f.rn], res);
                registers[f.rd] = res;
                break;
            }

            case 0x8: { // SUBI
                uint16_t res = registers[f.rm] - f.imm4;
                ula_flags(registers[f.rm], f.imm4, res);
                registers[f.rd] = res;
                break;
            }

            case 0x4: { // MOV
                registers[f.rd] = f.imm8;
                break;
            }

            case 0x2: { // LDR: Rd = MEM[Rm]
                uint16_t addr = registers[f.rm];
                registers[f.rd] = read_mem(addr);  // <-- substituído
                break;
            }

            case 0x3: { // STR: MEM[Rm] = Rn
                uint16_t addr = registers[f.rm];
                write_mem(addr, registers[f.rn]);  // <-- substituído
                break;
            }

            case 0xE: { // PUSH Rn
                registers[14]--;
                write_mem(registers[14], registers[f.rn]);  // <-- substituído
                break;
            }

            case 0xF: { // POP Rd
                registers[f.rd] = read_mem(registers[14]);  // <-- substituído
                registers[14]++;
                break;
            }

            case 0xD: { // CMP
                uint16_t res = registers[f.rm] - registers[f.rn];
                ula_flags(registers[f.rm], registers[f.rn], res);
                break;
            }

            case 0xB: { // SHR
                uint16_t res = registers[f.rm] >> f.imm4;
                ula_flags(registers[f.rm], f.imm4, res);
                registers[f.rd] = res;
                break;
            }

            case 0xC: { // SHL
                uint16_t res = registers[f.rm] << f.imm4;
                ula_flags(registers[f.rm], f.imm4, res);
                registers[f.rd] = res;
                break;
            }

            case 0x9: { // AND
                uint16_t res = registers[f.rm] & registers[f.rn];
                ula_flags(registers[f.rm], registers[f.rn], res);
                registers[f.rd] = res;
                break;
            }

            case 0xA: { // OR
                uint16_t res = registers[f.rm] | registers[f.rn];
                ula_flags(registers[f.rm], registers[f.rn], res);
                registers[f.rd] = res;
                break;
            }
        }
    }
}