#include "common.h"
#include <string.h>
#include <stdio.h>

void initVM(VM *vm) {
    printf("Starting %lu register machine.\n", (sizeof vm->registers) / (sizeof(uint32_t)));
    memset(vm->registers, 0, sizeof vm->registers);
    vm->pc = 0;
    vm->program = NULL;
    vm->prog_len = 0;
    vm->remainder = 0;
    vm-> equal_flag = false;
}

static OpCode decode_opcode(VM *vm) {
    OpCode code = (OpCode)vm->program[vm->pc];
    vm->pc++;
    return code;
}
static uint8_t next_8_bits(VM *vm) {
    uint8_t next = vm->program[vm->pc];
    vm->pc++;
    return next;
}
static uint16_t next_16_bits(VM *vm) {
    uint32_t next = (uint32_t)((vm->program[vm->pc] << 8) | (vm->program[vm->pc + 1]));
    vm->pc += 2;
    return next;
}

#define NEXT_BYTE_REG() vm->registers[next_8_bits(vm)]
#define SKIP_8() next_8_bits(vm)
#define SKIP_16() next_16_bits(vm)
#define SKIP_24() next_16_bits(vm); next_8_bits(vm)
void runVM(VM *vm, uint8_t *prog, uint32_t prog_len) {
    if (prog == NULL || prog_len == 0) {
        return;
    }
    vm->program = prog;
    vm->prog_len = prog_len;
    for (;;) {
        if (vm->pc > prog_len || vm->pc < 0) {
            printf("PC in unreachable state (%d vs. program length %d)\n", vm->pc, prog_len);
            break;
        }
        OpCode code = decode_opcode(vm);
        switch (code) {
            case HALT:
                printf("Halting machine.\n");
                return;
            case LOAD: {
                int reg_num = next_8_bits(vm);
                uint16_t num = next_16_bits(vm);
                vm->registers[reg_num] = (uint32_t)num;
                break;
            }
            case ADD: {
                int r1_val = NEXT_BYTE_REG();
                int r2_val = NEXT_BYTE_REG();
                int dest = next_8_bits(vm);
                vm->registers[dest] = r1_val + r2_val;
                break;
            }
            case SUB: {
                int r1_val = NEXT_BYTE_REG();
                int r2_val = NEXT_BYTE_REG();
                int dest = next_8_bits(vm);
                vm->registers[dest] = r1_val - r2_val;
                break;
            }
            case MUL: {
                int r1_val = NEXT_BYTE_REG();
                int r2_val = NEXT_BYTE_REG();
                int dest = next_8_bits(vm);
                vm->registers[dest] = r1_val * r2_val;
                break;
            }
            case DIV: {
                int r1_val = NEXT_BYTE_REG();
                int r2_val = NEXT_BYTE_REG();
                int dest = next_8_bits(vm);
                vm->registers[dest] = r1_val / r2_val;
                vm->remainder = (uint32_t)(r1_val % r2_val);
                break;
            }
            case JMP: {
                // Note: we don't need to skip bytes for jumps,
                // the final 16 bits are just padding. Except in the false
                // branch of JEQ, of course.
                int target = NEXT_BYTE_REG();
                vm->pc = target;
                break;
            }
            case JMPF: {
                int relative = NEXT_BYTE_REG();
                vm->pc += relative;
                break;
            }
            case JMPB: {
                int relative = NEXT_BYTE_REG();
                vm->pc -= relative;
                break;
            }
            case EQ: {
                int reg1_val = NEXT_BYTE_REG();
                int reg2_val = NEXT_BYTE_REG();
                vm->equal_flag = (bool)(reg1_val == reg2_val);
                SKIP_8();
                break;
            }
            case NEQ: {
                int reg1_val = NEXT_BYTE_REG();
                int reg2_val = NEXT_BYTE_REG();
                vm->equal_flag = (bool)(reg1_val != reg2_val);
                SKIP_8();
                break;
            }
            case GT: {
                int reg1_val = NEXT_BYTE_REG();
                int reg2_val = NEXT_BYTE_REG();
                vm->equal_flag = (bool)(reg1_val > reg2_val);
                SKIP_8();
                break;
            }
            case LT: {
                int reg1_val = NEXT_BYTE_REG();
                int reg2_val = NEXT_BYTE_REG();
                vm->equal_flag = (bool)(reg1_val < reg2_val);
                SKIP_8();
                break;
            }
            case GTE: {
                int reg1_val = NEXT_BYTE_REG();
                int reg2_val = NEXT_BYTE_REG();
                vm->equal_flag = (bool)(reg1_val >= reg2_val);
                SKIP_8();
                break;
            }
            case LTE: {
                int reg1_val = NEXT_BYTE_REG();
                int reg2_val = NEXT_BYTE_REG();
                vm->equal_flag = (bool)(reg1_val <= reg2_val);
                SKIP_8();
                break;
            }
            case JEQ: {
                int target = NEXT_BYTE_REG();
                if (vm->equal_flag) {
                    vm->pc = target;
                } else { // Ignore padding
                    SKIP_16();
                }
                break;
            }
            default:
                printf("Unknown opcode with point %d at PC=%d. Halting.\n", code, vm->pc);
                return;
        }
    }
}
#undef NEXT_8_REG_VAL
#undef SKIP_8
#undef SKIP_16
#undef SKIP_24

Instruction make_instr(OpCode code) {
    Instruction i;
    i.opcode = code;
    return i;
}

