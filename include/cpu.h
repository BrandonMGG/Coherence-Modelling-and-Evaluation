#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>


// Estados de los bloques de caché
#define CACHE_SIZE 4

struct Stats{
    int INV;
    int READ_REQ_RESP;
    int WRITE_REQ_RESP;
    int INCR_REQ_RESP;
    double avg_exec;
};

// Definir una estructura para representar una CPU
struct CPU {
    int id; // Identificador de la CPU (por ejemplo, 'A', 'B', 'C', ...)
    Cache cache;//estructura de cache
    struct Stats stats;
    struct Instruction instruction;

};

// Función para ejecutar una instrucción en la CPU
void execute_instruction(struct CPU *cpu, struct Instruction *instr, mqd_t mq);
