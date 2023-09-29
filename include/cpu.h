#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Estados de los bloques de caché
#define CACHE_SIZE 4

// Definir una estructura para representar una CPU
struct CPU {
    int id; // Identificador de la CPU (por ejemplo, 'A', 'B', 'C', ...)
    Cache cache;//estructura de cache
};


// Función para ejecutar una instrucción en la CPU
void execute_instruction(struct CPU *cpu, struct Instruction *instr);
