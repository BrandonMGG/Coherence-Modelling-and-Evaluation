#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 8
#define INST_ADDRESS_SIZE 4

// Estructura para representar una instrucción
struct Instruction{
    char op[10];
    char address[4];
    int data;
} ;

double random_m();

int randint(int min, int max);

int get_random_address();

int random_Hex();

int get_random_data();

void get_random_instruction(struct Instruction *instr);