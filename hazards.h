#ifndef HAZARDS_H
#define HAZARDS_H

#include <stdint.h>
#include "processor.h"
#include "instruction.h"

typedef struct {
    int valid;
    int16_t instruction;
    uint16_t pc;
} IF_ID_Register;

typedef struct {
    int valid;
    DecodedInstruction inst;
} ID_EX_Register;

/* Control hazard functions */
void flushIF_ID(IF_ID_Register *if_id);
void flushID_EX(ID_EX_Register *id_ex);
void flushAfterControlHazard(IF_ID_Register *if_id, ID_EX_Register *id_ex);

/* Data hazard functions */
int  writesToRegister(DecodedInstruction inst);
int  getDestinationRegister(DecodedInstruction inst);
int  usesRegister(DecodedInstruction inst, int reg);
int  hasDataHazard(DecodedInstruction exInst, DecodedInstruction idInst);
void insertBubble(ID_EX_Register *id_ex);

#endif
