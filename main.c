#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Package 4 Memory Architecture
uint16_t instructionMemory[1024]; // [cite: 287, 288]

void loadProgram(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    char line[100];
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        // Here you will call your parse and encode functions
        // and store the result in instructionMemory[i]
        i++;
    }
    fclose(file);
}

int main() {
    loadProgram("program.txt");
    return 0;
}