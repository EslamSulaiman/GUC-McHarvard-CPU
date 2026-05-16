#ifndef FETCH_DECODE_H
#define FETCH_DECODE_H

#include <stdint.h>

typedef struct{
    int opcode;
    int r1;
    int r2;
    int8_t immediate;
    uint16_t pc;
    int valid;
} Instruction;

void fetch();
void decode();
Instruction getInstruction();

#endif