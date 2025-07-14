## Descrição
Simulador RISC (Reduced Instruction Set Computer) desenvolvido em C como trabalho final da disciplina de Arquitetura de Computadores da UFC Quixadá.

## O que é um simulador RISC?
Um simulador RISC emula o funcionamento de um processador RISC, permitindo executar e testar programas escritos para essa arquitetura, sem precisar de hardware real.

## Como funciona

### 1. Leitura do programa
O simulador lê um arquivo de entrada (geralmente texto ou binário) contendo as instruções do programa a ser executado.

### 2. Memória e registradores
Implementa uma memória (array em C) e um conjunto de registradores (outro array), simulando o hardware real.

### 3. Ciclo de execução
O simulador executa um ciclo de busca-decodificação-execução:
- **Busca:** Lê a próxima instrução da memória
- **Decodificação:** Interpreta qual operação deve ser feita (ex: ADD, SUB, LOAD, STORE)
- **Execução:** Realiza a operação, alterando registradores ou memória conforme necessário

### 4. Controle de fluxo
Instruções de salto (como JUMP ou BRANCH) alteram o fluxo do programa, mudando o endereço da próxima instrução.

### 5. Saída
O simulador pode mostrar o estado dos registradores, memória, ou o resultado final do programa.

## Estrutura básica em C

```c
#define MEM_SIZE 1024
int memory[MEM_SIZE];
int registers[32];
int pc = 0; // program counter

void fetch_decode_execute() {
    while (1) {
        int instr = memory[pc];
        // decodifica e executa instrucao
        // atualiza pc
        if (instr == HALT) break;
    }
}
```

## Objetivos educacionais
- Entender como um processador executa instruções em baixo nível
- Depurar programas e visualizar o que acontece em cada etapa
- Ferramenta didática para aprender sobre arquitetura de computadores

## Compilação e execução
```bash
gcc risc_sim.c -o simulador
./simulador programa.txt
```

---
**Trabalho desenvolvido para a disciplina de Arquitetura de Computadores - UFC Quixadá**