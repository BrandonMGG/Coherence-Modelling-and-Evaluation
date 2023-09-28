#include "../include/memory.h"
#include "../include/bus.h"
#include "../include/cpu.h"
/**
 * Bus abstraction
*/

Cache get_core_cache(int cpu_id, struct bus *bus){
  return bus->cpus[cpu_id].cache;
}

void process_readmiss(){

}

void process_writemiss(){

}

void perform_wb(char * dirty_address, int dirty_data, struct bus *bus){
  //Needs delay to simulate latency
  set_data_in_memory(&(bus->main_memory), dirty_address, dirty_data);
}

int seek_owned(char * local_address, int requester_id, struct bus *bus){

}

int seek_shared(char * local_address, int requester_id, struct bus *bus){
  for (int core_id = 0; core_id < N_CPU; core_id++){
    if (core_id != requester_id){
      struct Cache *local_cache = get_core_cache(core_id, &bus);
      /**
       * IMPLEMENTAR EN CPU CONTROLLER
      */
      int block_index = get_block_by_index(&local_cache, local_address);
      struct CacheBlock *block;

      if (block_index == -1)
      {
        block = NULL;
      } else {
        block = &(local_cache->blocks[block_index]);
      }

     int state = block->state;
     char address[4] = block->address;
     if ((strcmp(local_address, address) == 0) &&
      (state == EXCLUSIVE || state == SHARED))
     {
      /* cache read delay, add RD transaction to stats */
      set_cache_block_state(core_id, &bus, SHARED);
      return block->data;
     }     

    }
  }
}

void seek_invalidate(char * local_address, int requester_id, struct bus * bus){
//search for this block in each core and invalidate it
//search for block that can gives the value - Owned or Modified
  for (int core_id = 0; core_id < N_CPU; core_id++){
    if (core_id != requester_id){
      struct Cache *local_cache = get_core_cache(core_id, &bus);
      /**
       * IMPLEMENTAR EN CPU CONTROLLER
      */
      struct CacheBlock *block = get_block_by_index(&local_cache, local_address);
      if (block == NULL)
      {
        continue;
      }
      if (&(block->state) == INVALID)
      {
        //add INV transaction to stats
        return;
      }

      /**
       * IMPLEMENTAR EN CPU CONTROLLER
      */
      set_cache_block_state(core_id, &bus, INVALID);
      //cache wr delay
      //animation highlight invalid

    }
  }
}


int main() {
  //main memory test
  //test();

  struct memory main_memory;
  struct bus my_bus;

  //Bus ops testing
  memory_init(&main_memory);
  my_bus.main_memory = main_memory; 

  perform_wb("F", 65, &my_bus);

  int data = get_data_from_memory(&(my_bus.main_memory), "F");
  printf("Data at address 0xf: %d\n", data);

  return 0;
}