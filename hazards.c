#include "hazards.h"

/* ── Control Hazards ─────────────────────────────────────────── */

void flushIF_ID(IF_ID_Register *if_id) {
    if_id->valid       = 0;
    if_id->instruction = 0;
    if_id->pc          = -1;
}

void flushID_EX(ID_EX_Register *id_ex) {
    id_ex->valid            = 0;
    id_ex->inst.valid       = 0;
    id_ex->inst.opcode      = -1;
    id_ex->inst.r1          = 0;
    id_ex->inst.r2          = 0;
    id_ex->inst.immediate   = 0;
    id_ex->inst.pc          = -1;
}

void flushAfterControlHazard(IF_ID_Register *if_id, ID_EX_Register *id_ex) {
    flushIF_ID(if_id);
    flushID_EX(id_ex);
}

/* ── Data Hazards ────────────────────────────────────────────── */

/*
 * Returns 1 if inst writes its result to r1.
 * Opcodes that write: ADD(0), SUB(1), MUL(2), LDI(3),
 *                     AND(5), OR(6), SAL(8), SAR(9), LB(10)
 */
int writesToRegister(DecodedInstruction inst) {
    if (!inst.valid) return 0;
    switch (inst.opcode) {
        case 0: case 1: case 2: case 3:
        case 5: case 6: case 8: case 9: case 10:
            return 1;
        default:
            return 0;
    }
}

/* Returns the destination register index, or -1 if inst doesn't write. */
int getDestinationRegister(DecodedInstruction inst) {
    if (writesToRegister(inst)) return inst.r1;
    return -1;
}

/*
 * Returns 1 if inst reads the register at index reg.
 *
 * Read patterns:
 *   ADD(0), SUB(1), MUL(2), AND(5), OR(6)  → r1 and r2
 *   BEQZ(4), SAL(8), SAR(9), SB(11)        → r1 only
 *   JR(7)                                   → r1 and r2
 *   LDI(3), LB(10)                          → neither (write-only)
 */
int usesRegister(DecodedInstruction inst, int reg) {
    if (!inst.valid || reg < 0) return 0;
    switch (inst.opcode) {
        case 0: case 1: case 2:
        case 5: case 6:
        case 7:
            return (inst.r1 == reg || inst.r2 == reg);

        case 4: case 8: case 9: case 11:
            return (inst.r1 == reg);

        case 3: case 10:
            return 0;

        default:
            return 0;
    }
}

/*
 * Returns 1 if there is a RAW data hazard:
 * exInst (currently in EX) writes a register that idInst (in ID) needs to read.
 */
int hasDataHazard(DecodedInstruction exInst, DecodedInstruction idInst) {
    if (!exInst.valid || !idInst.valid) return 0;

    int dest = getDestinationRegister(exInst);
    if (dest == -1) return 0;

    return usesRegister(idInst, dest);
}

/* Inserts a bubble (NOP) into the ID/EX pipeline register to stall the pipeline. */
void insertBubble(ID_EX_Register *id_ex) {
    id_ex->valid       = 0;
    id_ex->inst.valid  = 0;
    id_ex->inst.opcode = -1;
}
