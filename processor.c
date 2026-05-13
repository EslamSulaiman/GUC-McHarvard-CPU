#include "processor.h"

/*  Gama3a 3ashan testa5demo my part/function e3melo 
    fe awel el file #include "processor.h" w hate3rafo 
    testa5demo el functions ely hena 3ady */

/*  USE the functions here dont directly set 2y 7aga, fe setters w getters 
    lel instruction memory, data memory, 64 Registers, PC and SREG
    and make sure to use initializeProcessor at the start of running the program */

ProcessorState cpu;

/* Initializes the processor with all default values */
void initializeProcessor() {
    for(int i = 0; i < INSTRUCTION_MEMORY_SIZE; i++) {
        cpu.instructionMemory[i] = 0;
    }
    for(int i = 0; i < DATA_MEMORY_SIZE; i++) {
        cpu.dataMemory[i] = 0;
    }
    for(int i = 0; i < NUM_REGISTERS; i++) {
        cpu.registers[i] = 0;
    }
    cpu.PC = 0;
    cpu.SREG = 0;
}

/* Reading or writing from registers including PC and SREG functions */
int8_t readRegister(int index) {
    if (index < 0 || index >= NUM_REGISTERS) {
        printf("Error: Register index out of bounds\n");
        return 0;
    }

    return cpu.registers[index];
}
 
void writeRegister(int index, int8_t value) {
    if (index < 0 || index >= NUM_REGISTERS) {
        printf("Error: Register index out of bounds\n");
        return;
    }

    cpu.registers[index] = value;
}

void setPC(uint16_t value) {
    cpu.PC = value;
}

uint16_t getPC() {
    return cpu.PC;
}

void setSREG(uint8_t value) {
    cpu.SREG = value & 0x1F; // This is to make sure that only the last 5 bits are used, 000CVNSZ
}

uint8_t getSREG() {
    return cpu.SREG;
}
 
/* Reading or writing from instruction memory functions */
int16_t readInstructionMemory(int address) {
    if (address < 0 || address >= INSTRUCTION_MEMORY_SIZE) {
        printf("Error: Instruction memory address out of bounds\n");
        return 0;
    }

    return cpu.instructionMemory[address];
}
 
void writeInstructionMemory(int address, int16_t value) {
    if (address < 0 || address >= INSTRUCTION_MEMORY_SIZE) {
        printf("Error: Instruction memory address out of bounds\n");
        return;
    }

    cpu.instructionMemory[address] = value;
}
 
/* Reading or writing from data memory functions */
int8_t readDataMemory(int address) {
    if (address < 0 || address >= DATA_MEMORY_SIZE) {
        printf("Error: Data memory address out of bounds\n");
        return 0;
    }

    return cpu.dataMemory[address];
}
 
void writeDataMemory(int address, int8_t value) {
    if (address < 0 || address >= DATA_MEMORY_SIZE) {
        printf("Error: Data memory address out of bounds\n");
        return;
    }

    cpu.dataMemory[address] = value;
}