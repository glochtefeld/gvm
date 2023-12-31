#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include <stdio.h>
#include "painful.h"

void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == '.') {
        linenoiseAddCompletion(lc, ".execute");
        linenoiseAddCompletion(lc, ".program");
        linenoiseAddCompletion(lc, ".undo");
        linenoiseAddCompletion(lc, ".clear");
        linenoiseAddCompletion(lc, ".binary");
    }
}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf, "hello")) {
        *color = 35;
        *bold = 0;
        return " World";
    }
    return NULL;
}

void tryparse(const char *input, uint8_t **prog, int *len) {
    //printf("Parsing '%s'\n", input);
    uint8_t instr[4] = {0};

    int byte = 0;
    for (;;) {
        errno = 0;
        char *end;
        const long i = strtol(input, &end,16);
        if (input == end)
            break;
        const bool range_err = errno == ERANGE;
        // printf("Extracted '%.*s', strtol returned %ld.", (int)(end-input), input, i);
        input = end;

        if (byte < 4) {
            instr[byte] = (uint8_t)i;
        }
        byte++;

        if (range_err)
            printf("\n --> Range error occurred for '%.*s'\n", (int)(end-input), input);
    }

    /*for (int i=0;i<4;i++)
        printf("%d ",instr[i]);
    putchar('\n');*/

    int oldSize = *len;
    //printf("Old: %d\n", oldSize);
    if (oldSize == 0) {
        //printf("Growing to %lu (4 * %lu)\n", 4 * sizeof(uint8_t), sizeof(uint8_t));
        *prog = malloc(4 * sizeof(uint8_t));
        *len += 4;
        //printf("Now: %d\n", *len);
    } else {
        //printf("Growing to %lu (%d + 4 * %lu)\n", oldSize + 4 * sizeof(uint8_t), oldSize, sizeof(uint8_t));
        *prog = realloc(*prog, oldSize + (4*sizeof(uint8_t)));
        *len += 4;
        //printf("Now (resized): %d\n", *len);
    }
    for (int to=oldSize; to < oldSize + 4; to++) {
        (*prog)[to] = instr[to - oldSize];
    }
}

void start_painful_repl(VM *vm) {
    char *line;
    uint8_t *prog = NULL;
    int prog_len = 0;
    /* vm->program = malloc(4*sizeof(uint8_t));
    vm->prog_len = 4;
    tryparse("10 20 30 40", vm);
    for (int i=0; i<vm->prog_len; i++) {
        printf("%d\n", vm->program[i]);
    } */

    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);
    linenoiseSetMultiLine(1);
    linenoiseHistorySetMaxLen(1000);
    linenoiseHistoryLoad("painful_history.txt");

    while ((line = linenoise(">>> ")) != NULL) {
        if (line[0] != '\0' && line[0] != '.') {
            linenoiseHistoryAdd(line);
            linenoiseHistorySave("painful_history.txt");
            tryparse(line, &prog, &prog_len);
        } else if (!strncmp(line, ".program", 8)) {
            // List Program Instructions
            printf("Len: %d\n", prog_len);
            if (prog == NULL)
                continue;
            for (int i=0; i < prog_len; i+=4) {
                printf("[%d]: %d %d %d %d\n", i / 4, prog[i], prog[i+1], prog[i+2], prog[i+3]);
            }
        } else if (!strncmp(line, ".execute", 8)) {
            // Run Program
            runVM(vm, prog, prog_len);
        } else if (!strncmp(line, ".step", 5)) {
            if (!vm->running) {
                loadVM(vm, prog, prog_len);
                vm->running = true;
            }
            stepVM(vm);
        } else if (!strncmp(line, ".clear", 6)) {
            // Clear current program
            clearVM(vm);
        } else if (!strncmp(line, ".undo", 5)) {
            // Undo last instruction
            if (prog != NULL) {
                int newlen = prog_len - 4;
                prog = realloc(prog, newlen * sizeof(uint8_t));
                prog_len = newlen;
            }
        } else if (!strncmp(line, ".reg", 4)) {
            printf("PC: %d, Instruction: %d\n", vm->pc, vm->pc / 4);
            for (int i = 0; i < 32; i+=8) {
#define PRN_R(n) n, vm->registers[n]
                printf("[%02d]: %d\t[%02d]: %d\t[%02d]: %d\t[%02d]: %d\t[%02d]: %d\t[%02d]: %d\t[%02d]: %d\t[%02d]: %d\n",
                    PRN_R(i+0), PRN_R(i+1), PRN_R(i+2), PRN_R(i+3), PRN_R(i+4), PRN_R(i+5), PRN_R(i+6), PRN_R(i+7)); 
#undef PRN_R
            }
        }
        linenoiseFree(line);
    }
    printf("Goodbye!\n"); 
    return;
}

