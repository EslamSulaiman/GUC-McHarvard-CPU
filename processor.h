#ifndef PROCESSOR_H
#define PROCESSOR_H
 
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
/* Constant Sizes for the memories and registers */
#define INSTRUCTION_MEMORY_SIZE 1024
#define DATA_MEMORY_SIZE 2048
#define NUM_REGISTERS 64

/* Struct combining all processor state */
typedef struct {
    int16_t  instructionMemory[INSTRUCTION_MEMORY_SIZE];
    int8_t   dataMemory[DATA_MEMORY_SIZE];
    int8_t   registers[NUM_REGISTERS];
    uint16_t PC;
    uint8_t  SREG;
} ProcessorState;
 
extern ProcessorState cpu;
 
/* Function Definitions implemented in the processor.c file*/
void    initializeProcessor();
 
int8_t  readRegister(int index);
void    writeRegister(int index, int8_t value);
 
int16_t readInstructionMemory(int address);
void    writeInstructionMemory(int address, int16_t value);
 
int8_t  readDataMemory(int address);
void    writeDataMemory(int address, int8_t value);

void     setPC(uint16_t value);
uint16_t getPC();

void    setSREG(uint8_t value);
uint8_t getSREG();
 
#endif