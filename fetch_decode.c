#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "processor.h"
#include "main.c" // azon should be .h 
#include "fetch_decode.h"
short int instruction;
Instruction inst;
void fetch(){
    if(getPC() >= getInstructionCount()){
        printf("Error: PC out of bounds\n");
        return;
    }

    instruction = readInstructionMemory(getPC());
    inst.pc = getPC();
    setPC(getPC() + 1);
    inst.valid = 1;
}
void decode(){
    
    inst.opcode = (instruction >> 12) & 0xF;
    inst.r1 = (instruction >> 6) & 0x3F;
   if(inst.opcode == 0 || inst.opcode == 1 || inst.opcode == 2 || inst.opcode == 6 || inst.opcode == 7) {
    inst.immediate = 0;
    inst.r2 = instruction & 0x3F;

} else {
    inst.r2 = 0;
    inst.immediate = instruction & 0x3F;
    if(inst.immediate & 0x20) { 
    inst.immediate |= 0xC0; 
}
}

}
Instruction getInstruction(){
    return inst;
}

