#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint16_t instructionMemory[1024]; 
uint8_t dataMemory[2048];        
uint8_t registers[64];           
uint16_t PC = 0;                 
uint8_t SREG = 0;               

// New attribute to track loaded instructions
int instructionCount = 0;

// Getter for instruction count
int getInstructionCount() {
    return instructionCount;
}

int getOpcode(char *mnemonic) {
    if (strcmp(mnemonic, "ADD") == 0) return 0;
    if (strcmp(mnemonic, "SUB") == 0) return 1;
    if (strcmp(mnemonic, "MUL") == 0) return 2;
    if (strcmp(mnemonic, "LDI") == 0) return 3;
    if (strcmp(mnemonic, "BEQZ") == 0) return 4;
    if (strcmp(mnemonic, "AND") == 0) return 5;
    if (strcmp(mnemonic, "OR") == 0) return 6;
    if (strcmp(mnemonic, "JR") == 0) return 7;
    if (strcmp(mnemonic, "SAL") == 0) return 8;
    if (strcmp(mnemonic, "SAR") == 0) return 9;
    if (strcmp(mnemonic, "LB") == 0) return 10;
    if (strcmp(mnemonic, "SB") == 0) return 11;
    return -1;
}

uint16_t encodeInstruction(int opcode, int r1, int r2_imm) {
    uint16_t instruction = 0;
    instruction |= (opcode & 0x0F) << 12; 
    instruction |= (r1 & 0x3F) << 6;     
    instruction |= (r2_imm & 0x3F);      
    return instruction;
}

void loadProgram(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open %s\n", filename);
        return;
    }

    char line[100];
    instructionCount = 0; // Reset count before loading

    while (fgets(line, sizeof(line), file) && instructionCount < 1024) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char *mnemonic = strtok(line, " ");
        char *r1_str = strtok(NULL, " ");
        char *r2_imm_str = strtok(NULL, " ");

        if (mnemonic != NULL && r1_str != NULL && r2_imm_str != NULL) {
            int opcode = getOpcode(mnemonic);
            if (opcode == -1) continue; // Skip unknown instructions

            int r1 = atoi(r1_str + 1); 
            int r2_imm_val;

            if (r2_imm_str[0] == 'R') {
                r2_imm_val = atoi(r2_imm_str + 1); 
            } else {
                r2_imm_val = atoi(r2_imm_str);    
            }

            instructionMemory[instructionCount] = encodeInstruction(opcode, r1, r2_imm_val);
            
            printf("Loaded Line %d: %s (0x%04X)\n", instructionCount, mnemonic, instructionMemory[instructionCount]);
            instructionCount++; // Increment the global counter
        }
    }
    fclose(file);
    printf("Successfully loaded %d instructions.\n", instructionCount);
}

int main() {
    // Example usage
    // loadProgram("program.txt");
    // printf("Total instructions to execute: %d\n", getInstructionCount());
    return 0;
}