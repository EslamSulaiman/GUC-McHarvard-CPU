#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

typedef struct {
    int opcode;
    int r1;
    int r2;
    int immediate;
    int pc;
    int valid;
} DecodedInstruction;

/* Implemented in Execute and ALU.c — returns 1 if PC changed (branch/jump taken) */
int execute(DecodedInstruction inst, uint16_t *PC);

#endif
