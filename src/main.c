#include "common.h"
#include "assembly.h"
#include <stdio.h>

int main() {
    printf("Hello, world!\n");
    VM v;
    initVM(&v);
    start_assembly_repl(&v);
    // runVM(&v, prog, sizeof prog);
    // printf("%d\n", v.pc / 4);
    return 0;
}
