#include <stdio.h>
#include "util.h"
#include "cpu.h"

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
