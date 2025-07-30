#include "cpu.h"
#include "memory.h"
#include "util.h"
#include <stdio.h>

uint16_t registers[NUM_REGISTERS];
uint16_t IR = 0;
uint8_t FLAG_Z = 0;
uint8_t FLAG_C = 0;

void alu_flags(uint16_t a, uint16_t b, uint16_t result) {
    FLAG_Z = (result == 0);
    FLAG_C = (result < a);
}

void execute() {
    registers[14] = STACK_START; // SP
    registers[15] = 0;          // PC

    while (1) {
        uint16_t pc = registers[15];
        IR = memory[pc];
        if (IR == 0xFFFF) break;
        registers[15]++;
        uint16_t opcode = (IR >> 12) & 0xF;

        switch (opcode) {
            // JUMPs e Condicionais
            case 0x0: {
                uint16_t cond = (IR >> 10) & 0x3;
                int16_t offset = (int16_t)((IR & 0x03FF));
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
                uint8_t rd = (IR >> 8) & 0xF;
                uint8_t rm = IR & 0xF;
                uint16_t addr = registers[rm];
                uint16_t val = read_mem(addr);

                //verifica se endereço é valido, apagar depois
                printf("LDR R%d <- MEM[R%d] (addr=0x%04X) = 0x%04X\n", rd, rm, addr, val);

                registers[rd] = val;
            break;
            }
            // STR: MEM[Rm] = Rn
            case 0x3: {
                uint8_t rn = IR & 0xF;
                uint8_t rm = (IR >> 4) & 0xF;
                write_mem(registers[rm], registers[rn]);
                break;
            }

            // MOV Rd, #Im
            case 0x4: {
                uint8_t rd = (IR >> 8) & 0xF;
                uint8_t imm = IR & 0xFF;
                registers[rd] = imm;

                printf("MOV R%d = %d (0x%04X)\n", rd, imm, imm);

                break;
            }

            // ADD Rd = Rm + Rn
            case 0x5: {
                uint8_t rd = (IR >> 8) & 0xF;
                uint8_t rm = (IR >> 4) & 0xF;
                uint8_t rn = IR & 0xF;
                uint16_t res = registers[rm] + registers[rn];
                alu_flags(registers[rm], registers[rn], res);
                registers[rd] = res;
                break;
            }

            // ADDI Rd, Rm, #Im
            case 0x6: {
                uint8_t rd = (IR >> 8) & 0xF;
                uint8_t rm = (IR >> 4) & 0xF;
                uint8_t imm = IR & 0xF;
                uint16_t res = registers[rm] + imm;
                alu_flags(registers[rm], imm, res);
                registers[rd] = res;

                //verifica se valor é valido, apagar depois
                printf("ADDI R%d = R%d (0x%04X) + %d → 0x%04X\n", rd, rm, registers[rm], imm, res);

                break;
            }

            // SUB Rd = Rm - Rn
            case 0x7: {
                uint8_t rd = (IR >> 8) & 0xF;
                uint8_t rm = (IR >> 4) & 0xF;
                uint8_t rn = IR & 0xF;
                uint16_t res = registers[rm] - registers[rn];
                alu_flags(registers[rm], registers[rn], res);
                registers[rd] = res;
                break;
            }

            // SUBI Rd, Rm, #Im
            case 0x8: {
                uint8_t rd = (IR >> 8) & 0xF;
                uint8_t rm = (IR >> 4) & 0xF;
                uint8_t imm = IR & 0xF;
                uint16_t res = registers[rm] - imm;
                alu_flags(registers[rm], imm, res);
                registers[rd] = res;
                break;
            }

            // AND
            case 0x9: {
                uint8_t rd = (IR >> 8) & 0xF;
                uint8_t rm = (IR >> 4) & 0xF;
                uint8_t rn = IR & 0xF;
                uint16_t res = registers[rm] & registers[rn];
                alu_flags(registers[rm], registers[rn], res);
                registers[rd] = res;
                break;
            }

            // OR
            case 0xA: {
                uint8_t rd = (IR >> 8) & 0xF;
                uint8_t rm = (IR >> 4) & 0xF;
                uint8_t rn = IR & 0xF;
                uint16_t res = registers[rm] | registers[rn];
                alu_flags(registers[rm], registers[rn], res);
                registers[rd] = res;
                break;
            }

            // SHR
            case 0xB: {
                uint8_t rd = (IR >> 8) & 0xF;
                uint8_t rm = (IR >> 4) & 0xF;
                uint8_t imm = IR & 0xF;
                uint16_t res = registers[rm] >> imm;
                alu_flags(registers[rm], imm, res);
                registers[rd] = res;
                break;
            }

            // SHL
            case 0xC: {
                uint8_t rd = (IR >> 8) & 0xF;
                uint8_t rm = (IR >> 4) & 0xF;
                uint8_t imm = IR & 0xF;
                uint16_t res = registers[rm] << imm;
                alu_flags(registers[rm], imm, res);
                registers[rd] = res;
                break;
            }

            // CMP Rm, Rn
            case 0xD: {
                uint8_t rm = (IR >> 4) & 0xF;
                uint8_t rn = IR & 0xF;
                alu_flags(registers[rm], registers[rn], registers[rm] - registers[rn]);
                break;
            }

            // PUSH Rn
            case 0xE: {
                uint8_t rn = IR & 0xF;
                registers[14]--; // SP--
                write_mem(registers[14], registers[rn]);
                break;
            }

            // POP Rd
            case 0xF: {
                uint8_t rd = (IR >> 8) & 0xF;
                registers[rd] = read_mem(registers[14]);
                registers[14]++;
                break;
            }
        }
    }
}