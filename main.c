#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "processor.h"
#include "instruction.h"
#include "hazards.h"

/* ── Decode ──────────────────────────────────────────────────────────────────
   Instruction format (16 bits):
     bits 15-12 : opcode  (4 bits)
     bits 11-6  : r1      (6 bits)
     bits  5-0  : r2 / immediate (6 bits)
   Immediate is sign-extended for all opcodes except SCL(8) and SCR(9).
*/
static DecodedInstruction decode(int16_t instruction, uint16_t pc) {
    DecodedInstruction d;
    d.valid     = 1;
    d.pc        = (int)pc;
    d.opcode    = (instruction >> 12) & 0xF;
    d.r1        = (instruction >>  6) & 0x3F;
    d.r2        =  instruction        & 0x3F;

    int raw_imm = instruction & 0x3F;
    if (d.opcode == 8 || d.opcode == 9) {
        d.immediate = raw_imm;                              /* unsigned shift amount */
    } else {
        d.immediate = (raw_imm & 0x20) ? (raw_imm | (~0x3F)) : raw_imm; /* sign-extend */
    }
    return d;
}

/* ── Load program ────────────────────────────────────────────────────────────
   Parses an assembly text file and stores encoded instructions in instruction
   memory via writeInstructionMemory().  Returns the number of instructions loaded.

   TODO (Person 3/4): replace the stub below with real parse + encode logic.
   Each line should be parsed into a 16-bit encoded value and written with:
       writeInstructionMemory(i, encodedValue);
*/
static int loadProgram(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }

    char line[100];
    int  i = 0;
    while (fgets(line, sizeof(line), file) && i < INSTRUCTION_MEMORY_SIZE) {
        /* TODO: parse and encode 'line', then store:
               int16_t encoded = encode(line);
               writeInstructionMemory(i, encoded);           */
        i++;
    }
    fclose(file);
    return i;
}

/* ── Print final state ───────────────────────────────────────────────────── */
static void printFinalState(void) {
    printf("\n===== FINAL STATE =====\n");

    printf("\nRegisters:\n");
    for (int i = 0; i < NUM_REGISTERS; i++) {
        printf("  R%-2d = %d\n", i, readRegister(i));
    }

    printf("\nPC   = %u\n", getPC());
    printf("SREG = %u\n",  getSREG());

    printf("\nData Memory (non-zero entries):\n");
    for (int i = 0; i < DATA_MEMORY_SIZE; i++) {
        if (readDataMemory(i) != 0)
            printf("  [%d] = %d\n", i, readDataMemory(i));
    }

    printf("\nInstruction Memory:\n");
    for (int i = 0; i < INSTRUCTION_MEMORY_SIZE; i++) {
        if (readInstructionMemory(i) != 0)
            printf("  [%d] = 0x%04X\n", i, (uint16_t)readInstructionMemory(i));
    }
}

/* ── Pipeline simulation ─────────────────────────────────────────────────── */
static void runPipeline(int programSize) {
    IF_ID_Register IF_ID;
    ID_EX_Register ID_EX;

    /* Start with empty pipeline */
    flushIF_ID(&IF_ID);
    flushID_EX(&ID_EX);

    int cycle = 0;

    while (1) {
        cycle++;
        printf("\n=== Cycle %d ===\n", cycle);

        int doFetch = 1; /* default: fetch a new instruction at end of cycle */

        /* ── EX stage ──────────────────────────────────────────────────── */
        int pcChanged = 0;
        if (ID_EX.inst.valid) {
            uint16_t pc = getPC();
            pcChanged = execute(ID_EX.inst, &pc);
            setPC(pc);

            if (pcChanged) {
                printf("CONTROL HAZARD: flushing pipeline, new PC = %u\n", getPC());
                flushAfterControlHazard(&IF_ID, &ID_EX);
                doFetch = 0; /* next cycle will fetch from the new PC */
            }
        } else {
            printf("EX: bubble\n");
        }

        /* ── ID stage ──────────────────────────────────────────────────── */
        if (!pcChanged) {
            DecodedInstruction decoded = {0};

            if (IF_ID.valid) {
                decoded = decode(IF_ID.instruction, IF_ID.pc);
                printf("ID: opcode=%d r1=%d r2=%d imm=%d\n",
                       decoded.opcode, decoded.r1, decoded.r2, decoded.immediate);
            } else {
                printf("ID: bubble\n");
            }

            if (hasDataHazard(ID_EX.inst, decoded)) {
                printf("DATA HAZARD: stalling pipeline\n");
                insertBubble(&ID_EX);
                doFetch = 0; /* freeze IF_ID and PC */
            } else {
                ID_EX.inst  = decoded;
                ID_EX.valid = decoded.valid;
            }
        } else {
            printf("ID: flushed\n");
        }

        /* ── IF stage ──────────────────────────────────────────────────── */
        if (doFetch) {
            if (getPC() < (uint16_t)programSize) {
                IF_ID.instruction = readInstructionMemory(getPC());
                IF_ID.pc          = getPC();
                IF_ID.valid       = 1;
                printf("IF: fetched instruction at PC=%u\n", getPC());
                setPC(getPC() + 1);
            } else {
                IF_ID.valid = 0;
                printf("IF: no more instructions\n");
            }
        } else {
            printf("IF: stalled/flushed\n");
        }

        /* ── Stop when pipeline is drained and nothing left to fetch ───── */
        if (!IF_ID.valid && !ID_EX.inst.valid && getPC() >= (uint16_t)programSize) {
            break;
        }
    }

    printf("\nTotal cycles: %d\n", cycle);
}

/* ── Entry point ─────────────────────────────────────────────────────────── */
int main(void) {
    initializeProcessor();

    int programSize = loadProgram("program.txt");
    if (programSize == 0) {
        printf("No instructions loaded.\n");
        return 1;
    }

    printf("Loaded %d instructions.\n", programSize);
    runPipeline(programSize);
    printFinalState();

    return 0;
}
