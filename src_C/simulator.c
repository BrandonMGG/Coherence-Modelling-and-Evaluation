#include "../include/memory.h"
#include "../include/cache.h"
#include "../include/random_instruction.h"
#include "../include/cpu.h"
#include "../include/messages.h"
#include "../include/bus.h"

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <mqueue.h>

struct cpu_thread_args
{
   struct CPU *cpu; 
   mqd_t mq;
};

struct bus_thread_args
{
   struct bus *bus; 
   int isBusActive;
   int protocolo;
   mqd_t mq;
};

void* cpu_thread (void *args){
    struct cpu_thread_args* t_args = (struct cpu_thread_args*)args; 
    struct Instruction new_instruction;
    struct CPU *cpu = t_args->cpu;
    mqd_t mq = t_args->mq;
    
    for(int i=0 ; i < 10 ; i++){
        get_random_instruction(&new_instruction);
        
        printf("Executing instruction %s from core %d \n", new_instruction.op, cpu->id);

        execute_instruction(cpu, &new_instruction, mq);

        printf("******CPU: %d ******INV: %d , READ: %d, WRITE: %d ************ \n",cpu->id ,cpu->stats.INV, cpu->stats.READ_REQ_RESP, cpu->stats.WRITE_REQ_RESP);
    }
    return NULL;
}

void* bus_thread (void *args){
    struct bus_thread_args* t_args = (struct bus_thread_args*) args;
    struct bus *bus = t_args->bus;
    mqd_t mq = t_args->mq;
    int isBusActive = t_args->isBusActive;
    int proto = t_args->protocolo;
    for(int i =0; i < CACHE_SIZE ; i++){
            printf("77777777---Cache x Tag -> %d Address -> %s State -> %d Value -> %d \n",bus->cpus[0].cache.blocks[i].tag ,bus->cpus[0].cache.blocks[i].address ,bus->cpus[0].cache.blocks[i].state ,bus->cpus[0].cache.blocks[i].data);
    }
    
    process_tasks(bus, mq, isBusActive, proto);

    return NULL;
}

int main(){

    struct memory main_memory;
    struct bus my_bus;
    pthread_t cpu_threads[N_CPU];
    pthread_t bus_t;
    mqd_t mq;

    //Bus ops testing
    memory_init(&main_memory);
    my_bus.main_memory = main_memory; 

    
    struct bus_thread_args bus_t_args;

    mq = create_message_queue();
    bus_t_args.bus = &my_bus;
    bus_t_args.isBusActive = 1;
    bus_t_args.protocolo = MESI;
    bus_t_args.mq = mq;

    struct cpu_thread_args cpu_thread_args_array[N_CPU];

    // Inicializar y lanzar las threads de CPU
    for (int i = 0; i < N_CPU; i++)
    {
        my_bus.cpus[i].id = i;
        initializeCache(&my_bus.cpus[i].cache);

        my_bus.cpus[i].stats.INV= 0;
        my_bus.cpus[i].stats.READ_REQ_RESP =0;
        my_bus.cpus[i].stats.WRITE_REQ_RESP= 0;
        

        cpu_thread_args_array[i].cpu = &my_bus.cpus[i];
        cpu_thread_args_array[i].mq = mq;
        
        /*
        for(int j =0; j < CACHE_SIZE ; j++){
            printf("-----------Cache x Tag -> %d Address -> %s State -> %d Value -> %d \n",my_bus.cpus[i].cache.blocks[j].tag ,my_bus.cpus[i].cache.blocks[j].address 
            ,my_bus.cpus[i].cache.blocks[j].state ,my_bus.cpus[i].cache.blocks[j].data);
        }
        for(int k =0; k < CACHE_SIZE ; k++){
            printf("888888888---Cache x Tag -> %d Address -> %s State -> %d Value -> %d \n",cpu_thread_args_array[i].cpu->cache.blocks[k].tag ,
            cpu_thread_args_array[i].cpu->cache.blocks[k].address ,cpu_thread_args_array[i].cpu->cache.blocks[k].state ,cpu_thread_args_array[i].cpu->cache.blocks[k].data);
        }
        */
        pthread_create(&cpu_threads[i], NULL, cpu_thread, (void*)&cpu_thread_args_array[i]);
    }
    
    pthread_create(&bus_t, NULL, bus_thread, (void *) & bus_t_args);

    // Join threads and its args data structures
    for (int i = 0; i < N_CPU; i++)
    {
        pthread_join(cpu_threads[i], NULL);
    }

    pthread_join(bus_t, NULL);
    

    cleanup_message_queue(mq);
    
    

    return 0;
    
}