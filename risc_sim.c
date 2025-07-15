#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MEM_SIZE 0x10000      //16 bits
#define NUM_REGISTERS 16    
#define STACK_START 0x8000    // SP inicial 

// Registradores
uint16_t registers[NUM_REGISTERS]; 
uint16_t IR = 0;                   
uint8_t FLAG_Z = 0;                // Zero
uint8_t FLAG_C = 0;                // Carry

// Memória 
uint16_t memory[MEM_SIZE];

// Funções auxiliares
void load_program(const char* filename);
void execute();
void print_state();
void alu_flags(uint16_t a, uint16_t b, uint16_t result);
uint16_t read_mem(uint16_t addr);
void write_mem(uint16_t addr, uint16_t value);

void load_program(const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Erro ao abrir arquivo");
        exit(1);
        }
   

    char line[32];
    while (fgets(line, sizeof(line), fp)) {
        unsigned int addr, value;
        if (sscanf(line, "%x : 0x%x", &addr, &value) == 2){
            if (addr < MEM_SIZE)
                memory[addr] = (uint16_t)value;
        }
    }
    fclose(fp);
   
         //apagar isso aqui dps
    printf("Programa carregado:\n"); 
    for (int i = 0; i < 10; i++) {
        printf("MEM[%04X] = 0x%04X\n", i, memory[i]);
        }
        //isso ai em cima é só pra ver se ta carregando certo
}

// Função que atualiza as flags Z e C
void alu_flags(uint16_t a, uint16_t b, uint16_t result) {
    FLAG_Z = (result == 0);
    FLAG_C = (result < a);
}

// Leitura simulada com mapeamento de E/S
uint16_t read_mem(uint16_t addr) {
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

// Escrita simulada com mapeamento de E/S
void write_mem(uint16_t addr, uint16_t value) {
    if (addr == 0xF002) {
        printf("%c", (char)value);
    } else if (addr == 0xF003) {
        printf("%d", value);
    } else if (addr < MEM_SIZE) {
        memory[addr] = value;
    }
}

// Loop principal de execução
void execute() {
    registers[14] = STACK_START; // SP
    registers[15] = 0;           // PC

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

// Impressão do estado ao final
void print_state() {
    printf("Registradores:\n");
    for (int i = 0; i < 16; i++) {
        printf("R%-2d: 0x%04X\n", i, registers[i]);
    }

    //trocar isso pra printar somente oq foi usado de fato
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

// Função principal
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_programa.txt>\n", argv[0]);
        return 1;
    }
    
    load_program(argv[1]);
    execute();
    print_state();
    return 0;
}
