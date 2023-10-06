#include <stdio.h>
#include <stdlib.h>
#define N_CPU 3

struct bus {
  struct memory main_memory;
  struct Message channel;
  struct CPU cpus[N_CPU];
};

typedef enum {
    READMISS,
    WRITEMISS
} AccessType;


typedef enum {
    MOESI,
    MESI
} Protocol;

struct Message message;
/*
if (cpu->state == MODIFIED || cpu->state == OWNED ){
    //WRITE MEMORIA preform_wb wb bus.py
}
if //Revisar si las otras caches tienen el address en su cache 
    //Si no hay colocar estado en SHARED
else revisar si el address en otra cache tienen estado EXCLUSIVE o SHARED
    if si leer en memoria 
    else set estado SHARED
*/

Cache get_core_cache(int cpu_id, struct bus *bus);

void set_core_cache_block_state(int core_id,  int block_index, struct bus *bus, int state);

void process_tasks(struct bus *bus, mqd_t mq, int busIsActive, int prot);

void process_readmiss(int cpu_id, int block_index, struct bus *bus, int prot);

    /*
    if state == MODIFIED || state == OWNED write en memoria perform_wb en bus.py
    State O guardar dato en cache y camnbiar estado a MODIFIED
    Invalidad address a todas las caches
    
    */
void process_writemiss(int cpu_id, int block_index, struct bus *bus, int state);

void perform_wb(char * dirty_address, int dirty_data, struct bus *bus);

int seek_owned(char * local_address, int requester_id, struct bus *bus);

int seek_shared(char * local_address, int requester_id, struct bus *bus);

void seek_invalidate(char * local_address, int requester_id, struct bus *bus);