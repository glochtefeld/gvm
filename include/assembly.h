#ifndef ASSEMBLY_H
#define ASSEMBLY_H
#include "linenoise/linenoise.h"
#include "common.h"

typedef enum {
    OPNAME,
    REG,
    INT,
} ATokType;

typedef struct {
    ATokType type;
    union {
        OpCode code;
        int value;
    } at;
} AToken;

void start_assembly_repl();

#endif // ASSEMBLY_H
