#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "processor.h"
#include "instruction.h"

void setZeroFlag(int8_t result);
void setNegativeFlag(int8_t result);
void setCarryFlag(int result);
void setOverflowAddFlag(int8_t value1 , int8_t value2 , int8_t result);
void setOverflowSubFlag(int8_t value1 , int8_t value2 , int8_t result);
void setSignFlag();
void printSREG();

int execute( DecodedInstruction inst , uint16_t *PC){
    if(inst.valid == 0){
        return 0;
    }

    switch(inst.opcode){
        
       case 0: { // ADD

            int8_t value1 = readRegister(inst.r1);
            int8_t value2 = readRegister(inst.r2);

            int resultFull = ((uint8_t)value1) + ((uint8_t)value2);
            int8_t result = value1 + value2;

            writeRegister(inst.r1, result);

            setCarryFlag(resultFull);
            setOverflowAddFlag(value1 , value2 , result);
            setZeroFlag(result);
            setNegativeFlag(result);
            setSignFlag();

            printf("EX: ADD R%d R%d\n", inst.r1, inst.r2);
            printf("R%d changed to %d\n", inst.r1, readRegister(inst.r1));
            printSREG();

            break;
        }

        case 1: { // SUB

            int8_t value1 = readRegister(inst.r1);
            int8_t value2 = readRegister(inst.r2);

            int8_t result = value1 - value2;

            writeRegister(inst.r1, result);

            setOverflowSubFlag(value1 , value2 , result);
            setZeroFlag(result);
            setNegativeFlag(result);
            setSignFlag();

            printf("EX: SUB R%d R%d\n", inst.r1, inst.r2);
            printf("R%d changed to %d\n", inst.r1, readRegister(inst.r1));
            printSREG();

            break;
        }


        case 2 : { //Multiply R1 R2

            int8_t result = readRegister(inst.r1) * readRegister(inst.r2);

            writeRegister(inst.r1, result);

            setZeroFlag(result);
            setNegativeFlag(result);

            printf("EX: MUL R%d R%d\n", inst.r1, inst.r2);
            printf("R%d changed to %d\n", inst.r1, readRegister(inst.r1));
            printSREG();

            break;
        }

        case 3 : //LDI r1 Imm
            writeRegister(inst.r1, inst.immediate);

            printf("EX: LDI R%d %d\n", inst.r1, inst.immediate);
            printf("R%d changed to %d\n", inst.r1, readRegister(inst.r1));

            break;

        case 4 :{ //Branch if equal zero R1 imm

            printf("EX: BEQZ R%d %d\n", inst.r1, inst.immediate);

            if(readRegister(inst.r1) == 0){
                *PC = inst.pc + 1 + inst.immediate;

                printf("Branch taken. PC changed to %d\n", *PC);

                return 1;
            }
            else{
                printf("Branch not taken.\n");
            }

            break;
        }    

        case 5 : { //AND R1 R2

            int8_t result = readRegister(inst.r1) & readRegister(inst.r2);

            writeRegister(inst.r1, result);

            setZeroFlag(result);
            setNegativeFlag(result);

            printf("EX: AND R%d R%d\n", inst.r1, inst.r2);
            printf("R%d changed to %d\n", inst.r1, readRegister(inst.r1));
            printSREG();

            break;
        }

        case 6 : { //OR R1 R2

            int8_t result = readRegister(inst.r1) | readRegister(inst.r2);

            writeRegister(inst.r1, result);

            setZeroFlag(result);
            setNegativeFlag(result);

            printf("EX: OR R%d R%d\n", inst.r1, inst.r2);
            printf("R%d changed to %d\n", inst.r1, readRegister(inst.r1));
            printSREG();

            break;
        }

        case 7 : {//JR R1 R2
            uint16_t high = ((uint8_t)readRegister(inst.r1)) << 8;
            uint16_t low = (uint8_t)readRegister(inst.r2);

            *PC = high | low;

            printf("EX: JR R%d R%d\n", inst.r1, inst.r2);
            printf("Jump taken. PC changed to %d\n", *PC);

            return 1;
        }


        case 8 : { // SCL R1 imm  (circular left shift)

            uint8_t val   = (uint8_t)readRegister(inst.r1);
            int     shift = inst.immediate % 8;
            int8_t  result = (shift == 0)
                ? (int8_t)val
                : (int8_t)((val << shift) | (val >> (8 - shift)));

            writeRegister(inst.r1, result);

            setZeroFlag(result);
            setNegativeFlag(result);

            printf("EX: SCL R%d %d\n", inst.r1, inst.immediate);
            printf("R%d changed to %d\n", inst.r1, readRegister(inst.r1));
            printSREG();

            break;
        }

        case 9 : { // SCR R1 imm  (circular right shift)

            uint8_t val   = (uint8_t)readRegister(inst.r1);
            int     shift = inst.immediate % 8;
            int8_t  result = (shift == 0)
                ? (int8_t)val
                : (int8_t)((val >> shift) | (val << (8 - shift)));

            writeRegister(inst.r1, result);

            setZeroFlag(result);
            setNegativeFlag(result);

            printf("EX: SCR R%d %d\n", inst.r1, inst.immediate);
            printf("R%d changed to %d\n", inst.r1, readRegister(inst.r1));
            printSREG();

            break;
        }

        case 10 : //LB R1 Address
            writeRegister(inst.r1, readDataMemory(inst.immediate));

            printf("EX: LB R%d %d\n", inst.r1, inst.immediate);
            printf("R%d changed to %d\n", inst.r1, readRegister(inst.r1));

            break;

        case 11 : //SB R1 Address
            writeDataMemory(inst.immediate, readRegister(inst.r1));

            printf("EX: SB R%d %d\n", inst.r1, inst.immediate);
            printf("Data Memory[%d] changed to %d\n", inst.immediate, readDataMemory(inst.immediate));

            break;


        default:
            break;

    }

    return 0;
}

void setZeroFlag(int8_t result){
    if(result == 0){
        setSREG(getSREG() | (1 << 0));
    }
    else{
        setSREG(getSREG() & ~(1 << 0));
    }
}

void setNegativeFlag(int8_t result){

    if(result < 0){
        setSREG(getSREG() | (1 << 2));
    }
    else{
        setSREG(getSREG() & ~(1 << 2));
    }
}

void setCarryFlag(int result){

    if((result & (1 << 8)) != 0){
        setSREG(getSREG() | (1 << 4));
    }
    else{
        setSREG(getSREG() & ~(1 << 4));
    }
}

void setOverflowAddFlag(int8_t value1 , int8_t value2 , int8_t result){

    if((value1 > 0 && value2 > 0 && result < 0) || (value1 < 0 && value2 < 0 && result >= 0)){
        setSREG(getSREG() | (1 << 3));
    }
    else{
        setSREG(getSREG() & ~(1 << 3));
    }
}

void setOverflowSubFlag(int8_t value1 , int8_t value2 , int8_t result){

    if((value1 > 0 && value2 < 0 && result < 0) || (value1 < 0 && value2 > 0 && result >= 0)){
        setSREG(getSREG() | (1 << 3));
    }
    else{
        setSREG(getSREG() & ~(1 << 3));
    }
}

void setSignFlag(){

    int negative = (getSREG() >> 2) & 1;
    int overflow = (getSREG() >> 3) & 1;

    if((negative ^ overflow) == 1){
        setSREG(getSREG() | (1 << 1));
    }
    else{
        setSREG(getSREG() & ~(1 << 1));
    }
}

void printSREG(){
    printf("SREG changed to %d\n", getSREG());
}