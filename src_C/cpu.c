#include "../include/random_instruction.h"
#include "../include/cache.h"
#include "../include/cpu.h"
#include "../include/messages.h"

#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <mqueue.h>

char read_char[10] = "READ";
char write_char[10] = "WRITE";
char incr_char[10] = "INCR";

// Función para ejecutar una instrucción en la CPU
void execute_instruction(struct CPU *cpu, struct Instruction *instr, mqd_t mq) {
    
    printf("CPU %d - Ejecutando instrucción:\n", cpu->id);
    printf("  Operación: %s\n", instr->op);
    printf("  Dirección: %s\n", instr->address);
    printf("  Datos: %d\n", instr->data);
    //printf("\n");
    
    if (strcmp(instr->op, read_char) == 0){
        readCacheBlock(&cpu->cache,instr->address, cpu->id, mq);
        printf("Instruccion Recibida = Read \n");
    }else if (strcmp(instr->op, write_char) == 0){
        if(writeCacheBlock(&cpu->cache,instr->address,instr->data,cpu->id, mq) == 1){
            cpu->stats.WRITE_REQ_RESP++;
        }
        printf("Instruccion Recibida = Write \n");
    }
    else if (strcmp(instr->op, incr_char) == 0){
        //Leer dato, sumar 1 y escribir dato en memoria 
        printf("Instruccion Recibida = Incremento \n");
    }
    printf("---------------------------------------------------------------------- \n");
}

int main() {
    mqd_t mq ;
    struct mq_attr attr;
     // Set message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = sizeof(struct Message);
    attr.mq_curmsgs = 0;

     // Create and open the message queue
    mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0666, &attr);

      // Inicializar la semilla aleatoria
    srand(time(NULL));

      // Crear una instancia de CPU
    struct CPU cpu;
    cpu.id = 1; // Puedes asignar cualquier identificador que desees
    initializeCache(&cpu.cache);

    struct Instruction instr;
    //for (int i = 0; i < 10; i++) {
    //    get_random_instruction(&instr); // Generar una instrucción aleatoria
    //    printf("Instruccion generada Op= %s , Address: %s ,Valor= %d \n", instr.op,instr.address, instr.data);
    //    execute_instruction(&cpu, &instr,mq); // Ejecutar la instrucción en la CPU
    //}
      

     
      //cpu.cache.blocks[1].address[1] = '\0'; // Inicializa el campo address
     
     //------------------------------------------------------------Pruebas para el READ--------------------------------------------------------------
     // Pruebas para el READ
      // Usamos sprintf para asignar un valor
      //sprintf(cpu.cache.blocks[0].address, "0x2");
      //cpu.cache.blocks[0].state= INVALID;

      //cpu.cache.blocks[3].state= MODIFIED;

      // Imprimimos el contenido
      //printf("El valor es: %s\n", cpu.cache.blocks[0].address);
      //printf("El valor es: %d\n", cpu.cache.blocks[0].tag);
      // Crear una estructura para almacenar la instrucción generada aleatoriamente
      //struct Instruction instr;

      // Generar y ejecutar instrucciones aleatorias
      //for (int i = 0; i < 10; i++) {
          //get_random_instruction(&instr); // Generar una instrucción aleatoria
         
      //    sprintf(instr.op, "WRITE");
       //   sprintf(instr.address, "0x4");
       //   instr.data = 0;
       //   execute_instruction(&cpu, &instr, mq); // Ejecutar la instrucción en la CPU

          //printf("El valor CACHE WRITE BACK es: %d\n", getBlockIdWithWriteBackPolicy(&cpu.cache));
      //}
     //---------------------------------------------------------------Pruebas para el READ-----------------------------------------------------------
    

     
     //----------------------------------------------------------Prueba de WRITE----------------------------------------------------------------
     // Prueba de WRITE
     // Usamos sprintf para asignar un valor
      sprintf(cpu.cache.blocks[0].address, "0x1");
      cpu.cache.blocks[0].state= MODIFIED;
      // Imprimimos el contenido
      printf("El valor es: %s\n", cpu.cache.blocks[0].address);
      // Crear una estructura para almacenar la instrucción generada aleatoriamente
      //struct Instruction instr;

      // Generar y ejecutar instrucciones aleatorias
      //for (int i = 0; i < 10; i++) {
          //get_random_instruction(&instr); // Generar una instrucción aleatoria
            //printf("Instruccion generada Op= %s , Address: %s ,Valor= %d \n", instr.op,instr.address, instr.data);
          sprintf(instr.op, "READ");
          sprintf(instr.address, "0x1");
          instr.data = 0;
          execute_instruction(&cpu, &instr,mq); // Ejecutar la instrucción en la CPU
     // }
     //-------------------------------------------------------Prueba de WRITE-------------------------------------------------------------------
     

     //-------------------------------------------------------Prueba mensaje--------------------------------------------------
     
     
    
     //-------------------------------------------------------Prueba mensaje--------------------------------------------------
    return 0;
}