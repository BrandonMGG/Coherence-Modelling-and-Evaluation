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
   struct CPU cpu; 
   mqd_t mq;
};

struct bus_thread_args
{
   struct bus bus; 
   int isBusActive;
   mqd_t mq;
};

void* cpu_thread (void *args){
    struct cpu_thread_args* t_args = (struct cpu_thread_args*)args; 
    struct Instruction new_instruction;
    struct CPU cpu = t_args->cpu;
    mqd_t mq = t_args->mq;

    get_random_instruction(&new_instruction);

   printf("Executing instruction %s from core %d \n", new_instruction.op, cpu.id);

    execute_instruction(&cpu, &new_instruction, mq);
    return NULL;
}

void* bus_thread (void *args){
    struct bus_thread_args* t_args = (struct bus_thread_args*) args;
    struct bus bus = t_args->bus;
    mqd_t mq = t_args->mq;
    int isBusActive = t_args->isBusActive;

    process_tasks(&bus, mq, isBusActive);

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

    
    struct cpu_thread_args cpu_t_args;
    struct bus_thread_args bus_t_args;

    mq = create_message_queue();
    cpu_t_args.mq = mq;
    bus_t_args.bus = my_bus;
    bus_t_args.isBusActive = 1;
    bus_t_args.mq = mq;

    //Init and start CPU and bus threads
    for (int i = 0; i < N_CPU; i++)
    {   
        my_bus.cpus[i].id = i;
        initializeCache(&my_bus.cpus[i].cache);
        cpu_t_args.cpu = my_bus.cpus[i];
        pthread_create(&cpu_threads[i], NULL, cpu_thread, (void*)&cpu_t_args);
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