#include "../include/random_instruction.h"
#include "../include/cache.h"
#include "../include/cpu.h"

char read_char[10] = "READ";
char write_char[10] = "WRITE";
char incr_char[10] = "INCR";

// Función para ejecutar una instrucción en la CPU
void execute_instruction(struct CPU *cpu, struct Instruction *instr) {
    
    printf("CPU %d - Ejecutando instrucción:\n", cpu->id);
    printf("  Operación: %s\n", instr->op);
    printf("  Dirección: %s\n", instr->address);
    printf("  Datos: %d\n", instr->data);
    //printf("\n");

    if (strcmp(instr->op, read_char) == 0){
        readCacheBlock(&cpu->cache,instr->address, cpu->id);
        
    }else if (strcmp(instr->op, write_char) == 0){
        writeCacheBlock(&cpu->cache,instr->address,instr->data,cpu->id);
    }
    else if (strcmp(instr->op, incr_char) == 0){
        //Leer dato, sumar 1 y escribir dato en memoria 
    }
}

// int main() {
//     // Inicializar la semilla aleatoria
//     srand(time(NULL));

//     // Crear una instancia de CPU
//     struct CPU cpu;
//     cpu.id = 1; // Puedes asignar cualquier identificador que desees
//     initializeCache(&cpu.cache);

//     // Crear una estructura para almacenar la instrucción generada aleatoriamente
//     Instruction instr;

//     // Generar y ejecutar instrucciones aleatorias
//     for (int i = 0; i < 10; i++) {
//         get_random_instruction(&instr); // Generar una instrucción aleatoria
//         execute_instruction(&cpu, &instr); // Ejecutar la instrucción en la CPU
//     }

//     return 0;
// }