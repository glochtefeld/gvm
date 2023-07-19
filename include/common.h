#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    HALT,
    LOAD,
    ADD,
    SUB,
    MUL,
    DIV,
    JMP,
    JMPF,
    JMPB,
    EQ,
    NEQ,
    GT,
    LT,
    GTE,
    LTE,
    JEQ,
    IGL,
} OpCode;

typedef struct {
    OpCode opcode;
} Instruction;
Instruction make_instr(OpCode code);

typedef struct {
    uint32_t registers[32];

    uint32_t pc;
    uint8_t *program;
    uint32_t prog_len;

    uint32_t remainder;
    bool equal_flag;
    bool running;
} VM;

void initVM(VM *vm);
void runVM(VM *vm, uint8_t *prog, uint32_t prog_len);
void loadVM(VM *vm, uint8_t *prog, uint32_t prog_len);
void stepVM(VM *vm);
void clearVM(VM *vm);
#endif
