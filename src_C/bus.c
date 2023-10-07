#include "../include/memory.h"
#include "../include/cache.h"
#include "../include/random_instruction.h"
#include "../include/cpu.h"
#include "../include/messages.h"
#include "../include/bus.h"

/**
 * Bus abstraction
*/

Cache get_core_cache(int cpu_id, struct bus *bus){
  return bus->cpus[cpu_id].cache;
}
   
void process_tasks(struct bus *bus, mqd_t mq, int busIsActive, int prot){
  // SIMULATE PROCESS DELAY
  //message = bus->channel;
  int access, core_id, block_id, data;
  CacheBlock block;
  Cache local_cache;

  while (busIsActive == 1)
  {

    if (mq_receive(mq, (char *)&message, sizeof(struct Message), NULL) == -1) {
            perror("mq_receive");
            break;
        }
    bus->channel = message;
    // Process the received message (e.g., trigger cache coherence actions)
    printf("Received message: id=%d, access=%d, address=%s, block_id=%d\n",
        message.id, message.access, message.address, message.block_id);

    access = message.access;

    // Get core cache
    core_id = message.id;
    local_cache = get_core_cache(core_id, bus);

    // Get info from cache

    block_id = message.block_id;
    block = local_cache.blocks[block_id];
    //state = block.state;
    data = block.data;

    if (access == WRITEMISS)
    {
      process_writemiss(core_id, block_id, bus, data);
    }
    else if (access == READMISS)
    {
      process_readmiss(core_id, block_id, bus, prot);

    }else if (access == INCREMENT)
    {
      process_increment(core_id, block_id, bus, prot);
    }
    
    else printf("Access Error - %d not found", access);
    
    //for(int i =0; i < MEM_SIZE ; i++){
    //  printf("Memoria Principal Address -> %s Data -> %d \n",bus->main_memory.entries[i].address , bus->main_memory.entries[i].data);
    //}
    for(int i =0; i < CACHE_SIZE ; i++){
      printf("Cache 0 Tag -> %d Address -> %s State -> %d Value -> %d \n",bus->cpus[0].cache.blocks[i].tag ,bus->cpus[0].cache.blocks[i].address ,bus->cpus[0].cache.blocks[i].state ,bus->cpus[0].cache.blocks[i].data);
    }
    for(int i =0; i < CACHE_SIZE ; i++){
      printf("Cache 1 Tag -> %d Address -> %s State -> %d Value -> %d \n",bus->cpus[1].cache.blocks[i].tag ,bus->cpus[1].cache.blocks[i].address ,bus->cpus[1].cache.blocks[i].state ,bus->cpus[1].cache.blocks[i].data);
    }
    for(int i =0; i < CACHE_SIZE ; i++){
      printf("Cache 2 Tag -> %d Address -> %s State -> %d Value -> %d \n",bus->cpus[2].cache.blocks[i].tag ,bus->cpus[2].cache.blocks[i].address ,bus->cpus[2].cache.blocks[i].state ,bus->cpus[2].cache.blocks[i].data);
    }
    

  }
  
}

void set_core_cache_block_state(int cpu_id, int block_index, struct bus *bus, int state){
  bus->cpus[cpu_id].cache.blocks[block_index].state = state;
}

void process_readmiss(int cpu_id, int block_index, struct bus *bus ,int prot){
  //If I do read and if I am writing on M or O I must do WB
  if(bus->cpus[cpu_id].cache.blocks[block_index].state == MODIFIED || bus->cpus[cpu_id].cache.blocks[block_index].state == OWNED){
    perform_wb(bus->channel.address,bus->cpus[cpu_id].cache.blocks[block_index].data, bus);
    //printf("Address : %s \n",bus->cpus[cpu_id].cache.blocks[block_index].address);
    //printf("cpu : %d , block index: %d ,state : %d \n", cpu_id, block_index , bus->cpus[cpu_id].cache.blocks[block_index].state);
    printf("Readmiss Modified o Owned  Address: %s \n", bus->channel.address);
  }
  int owned_data = 0;
  if( prot == MOESI){
    // Now ask check if other cores has the block
    owned_data = seek_owned(bus->channel.address,cpu_id,bus);
    printf("*****************************************************************************************\n");
  }
  else{
    owned_data = -1; 
  }
  
  if( strcmp(bus->cpus[cpu_id].cache.blocks[block_index].address, bus->channel.address) == 0){
    if(bus->cpus[cpu_id].cache.blocks[block_index].state == INVALID){
      int mem_data = get_data_from_memory(&bus->main_memory,bus->channel.address);
      bus->cpus[cpu_id].cache.blocks[block_index].state = EXCLUSIVE;
      bus->cpus[cpu_id].cache.blocks[block_index].data = mem_data;
      snprintf(bus->cpus[cpu_id].cache.blocks[block_index].address, sizeof(bus->cpus[cpu_id].cache.blocks[block_index].address),"%s", bus->channel.address);
      printf("Readmiss Trae de memoria Address: %s\n", bus->channel.address);
    }
  }
  // If sponsors state is O or M then set block state to S
  else if(owned_data != -1){
    bus->cpus[cpu_id].cache.blocks[block_index].state = SHARED;
    bus->cpus[cpu_id].cache.blocks[block_index].data = owned_data; 
    snprintf(bus->cpus[cpu_id].cache.blocks[block_index].address, sizeof(bus->cpus[cpu_id].cache.blocks[block_index].address), "%s",bus->channel.address);
    printf("Readmiss Si hay otro bloque en estado Owned Address: %s \n", bus->channel.address);
  //If no sponsors
  }else {
    // Check if address is E or S in another node
    //and save that value to the local cache in S state
    int shared_data = seek_shared(bus->channel.address,cpu_id,bus);
    if(shared_data != -1){
      bus->cpus[cpu_id].cache.blocks[block_index].state = SHARED;
      bus->cpus[cpu_id].cache.blocks[block_index].data = shared_data;
      snprintf(bus->cpus[cpu_id].cache.blocks[block_index].address, sizeof(bus->cpus[cpu_id].cache.blocks[block_index].address),"%s", bus->channel.address);
      printf("Readmiss NO hay otro bloque en Owned pero si en Shared  Address: %s\n", bus->channel.address);
    }else{
      //Then no one have the value, read from memory then set block state to E
      int mem_data = get_data_from_memory(&bus->main_memory,bus->channel.address);
      bus->cpus[cpu_id].cache.blocks[block_index].state = EXCLUSIVE;
      bus->cpus[cpu_id].cache.blocks[block_index].data = mem_data;
      snprintf(bus->cpus[cpu_id].cache.blocks[block_index].address, sizeof(bus->cpus[cpu_id].cache.blocks[block_index].address),"%s", bus->channel.address);
      printf("Readmiss Trae de memoria Address: %s\n", bus->channel.address);
    }
  }
}

void process_writemiss(int cpu_id, int block_index, struct bus *bus, int value){

  //If I do read and if I am writing on M or O I must do WB
  if(bus->cpus[cpu_id].cache.blocks[block_index].state == MODIFIED || bus->cpus[cpu_id].cache.blocks[block_index].state == OWNED){
    perform_wb(bus->channel.address,bus->cpus[cpu_id].cache.blocks[block_index].data, bus);
    printf("Writemiss Guardar de memoria Address: %s\n", bus->channel.address);
  }
  bus->cpus[cpu_id].cache.blocks[block_index].state = MODIFIED;
  bus->cpus[cpu_id].cache.blocks[block_index].data = bus->channel.value;
  snprintf(bus->cpus[cpu_id].cache.blocks[block_index].address, sizeof(bus->cpus[cpu_id].cache.blocks[block_index].address),"%s", bus->channel.address);
  seek_invalidate(bus->channel.address,cpu_id,bus);
  bus->cpus[cpu_id].stats.INV++;
  printf("Writemiss Invalidate  Address: %s\n",bus->channel.address);
}


void process_increment(int cpu_id, int block_index, struct bus *bus, int prot){
  //If I do read and if I am writing on M or O I must do WB
  if(bus->cpus[cpu_id].cache.blocks[block_index].state == MODIFIED || bus->cpus[cpu_id].cache.blocks[block_index].state == OWNED){
    perform_wb(bus->channel.address,bus->cpus[cpu_id].cache.blocks[block_index].data, bus);
    //printf("Address : %s \n",bus->cpus[cpu_id].cache.blocks[block_index].address);
    //printf("cpu : %d , block index: %d ,state : %d \n", cpu_id, block_index , bus->cpus[cpu_id].cache.blocks[block_index].state);
    printf("Readmiss Modified o Owned  Address: %s \n", bus->channel.address);
  }
  int owned_data = 0;
  if( prot == MOESI){
    // Now ask check if other cores has the block
    owned_data = seek_owned(bus->channel.address,cpu_id,bus);
    printf("*****************************************************************************************\n");
  }
  else{
    owned_data = -1; 
  }
  if( strcmp(bus->cpus[cpu_id].cache.blocks[block_index].address, bus->channel.address) == 0){
    if(bus->cpus[cpu_id].cache.blocks[block_index].state == INVALID){
      int mem_data = get_data_from_memory(&bus->main_memory,bus->channel.address);
      bus->cpus[cpu_id].cache.blocks[block_index].state = EXCLUSIVE;
      bus->cpus[cpu_id].cache.blocks[block_index].data = mem_data+1;
      snprintf(bus->cpus[cpu_id].cache.blocks[block_index].address, sizeof(bus->cpus[cpu_id].cache.blocks[block_index].address),"%s", bus->channel.address);
      printf("Readmiss Trae de memoria Address: %s\n", bus->channel.address);
    }
  }
  
  // If sponsors state is O or M then set block state to S
  else if(owned_data != -1){
    bus->cpus[cpu_id].cache.blocks[block_index].state = MODIFIED;
    bus->cpus[cpu_id].cache.blocks[block_index].data = owned_data+1;
    snprintf(bus->cpus[cpu_id].cache.blocks[block_index].address, sizeof(bus->cpus[cpu_id].cache.blocks[block_index].address),"%s", bus->channel.address);
    seek_invalidate(bus->channel.address,cpu_id,bus);
    
    printf("Invrement Invalidate  Address: %s\n",bus->channel.address);
    
  //If no sponsors
  }else {
    // Check if address is E or S in another node
    //and save that value to the local cache in S state
    int shared_data = seek_shared(bus->channel.address,cpu_id,bus);
    if(shared_data != -1){
      bus->cpus[cpu_id].cache.blocks[block_index].state = MODIFIED;
      bus->cpus[cpu_id].cache.blocks[block_index].data = shared_data+1;
      snprintf(bus->cpus[cpu_id].cache.blocks[block_index].address, sizeof(bus->cpus[cpu_id].cache.blocks[block_index].address),"%s", bus->channel.address);
      seek_invalidate(bus->channel.address,cpu_id,bus);
      
      printf("Invrement Invalidate  Address: %s\n",bus->channel.address);
    }else{
      //Then no one have the value, read from memory then set block state to E
      int mem_data = get_data_from_memory(&bus->main_memory,bus->channel.address);
      bus->cpus[cpu_id].cache.blocks[block_index].state = MODIFIED;
      bus->cpus[cpu_id].cache.blocks[block_index].data = mem_data+1;
      snprintf(bus->cpus[cpu_id].cache.blocks[block_index].address, sizeof(bus->cpus[cpu_id].cache.blocks[block_index].address),"%s", bus->channel.address);
      seek_invalidate(bus->channel.address,cpu_id,bus);
      bus->cpus[cpu_id].stats.INV++;
    }
  }
  
  
}


void perform_wb(char * dirty_address, int dirty_data, struct bus *bus){
  //Needs delay to simulate latency
  set_data_in_memory(&(bus->main_memory), dirty_address, dirty_data);
}

int seek_owned(char * local_address, int requester_id, struct bus *bus){

  for (int core_id = 0; core_id < N_CPU; core_id++){
    if (core_id != requester_id){
      //printf("Core: %d \n" , core_id);
      Cache local_cache = get_core_cache(core_id, bus);
      //printf("Cache X Tag -> %d Address -> %s State -> %d Value -> %d \n",local_cache.blocks[2].tag ,local_cache.blocks[2].address ,local_cache.blocks[2].state ,local_cache.blocks[2].data);
      int block_index = getCacheBlock_by_address(&local_cache, local_address);
      //printf("Block index: %d , Local Address: %s \n" , block_index, local_address);
      CacheBlock block;

      // if block is no in current core, then check the next one
      if (block_index == -1)
        continue;

      else
      block = local_cache.blocks[block_index];
      int state = block.state;
      char *address = block.address;
      //printf("local Address: %s  Address: %s " ,local_address, address);
      if ((strcmp(local_address, address) == 0) && (state == MODIFIED || state == OWNED))
      {
        /* cache read delay, add RD transaction to stats */
        //bus->cpus[core_id].cache.blocks[block_index].state = OWNED;
        set_core_cache_block_state(core_id, block_index, bus, OWNED);
        return block.data;
      }     

    }
  }

  return -1;

}

int seek_shared(char * local_address, int requester_id, struct bus *bus){
  
  for (int core_id = 0; core_id < N_CPU; core_id++){
    if (core_id != requester_id){

      Cache local_cache = get_core_cache(core_id, bus);
      int block_index = getCacheBlock_by_address(&local_cache, local_address);
      CacheBlock block;

      // if block is no in current core, then check the next one
      if (block_index == -1)
        continue;

      else
        block = local_cache.blocks[block_index];

      int state = block.state;
      char *address = block.address;
      if ((strcmp(local_address, address) == 0) &&
        (state == EXCLUSIVE || state == SHARED))
      {
        /* cache read delay, add RD transaction to stats */
        set_core_cache_block_state(core_id, block_index, bus, SHARED);
        return block.data;
      }     

    }
  }
  return -1;
}

void seek_invalidate(char * local_address, int requester_id, struct bus * bus){

//search for this block in each core and invalidate it
  for (int core_id = 0; core_id < N_CPU; core_id++){
    if (core_id != requester_id){
      Cache local_cache = get_core_cache(core_id, bus);
      int block_index = getCacheBlock_by_address(&local_cache, local_address);
      CacheBlock block;
      
       // if block is no in current core, then check the next one
      if (block_index == -1) continue;

      else block = local_cache.blocks[block_index];

      if (block.state == INVALID)
      {
        //add INV transaction to stats
        return;
      }

      /**
       * IMPLEMENTAR EN CPU CONTROLLER
      */
      set_core_cache_block_state(core_id, block_index, bus, INVALID);
      //cache wr delay
      //animation highlight invalid

    }
  }
}



// int main() {
//   //main memory test
//   //test();

//   struct memory main_memory;
//   struct bus my_bus;

//   //Bus ops testing
//   memory_init(&main_memory);
//   my_bus.main_memory = main_memory; 

//   perform_wb("F", 65, &my_bus);

//   int data = get_data_from_memory(&(my_bus.main_memory), "F");
//   printf("Data at address 0xf: %d\n", data);


//   // Bus queue test
//   mqd_t mq;

//   // Open the message queue
//   mq = mq_open(MQ_NAME, O_RDONLY);
//   if (mq == (mqd_t)-1) {
//       perror("mq_open");
//       exit(1);
//   }

//   process_tasks(&my_bus, mq, 1);

//   // Close the message queue
//   mq_close(mq);

//   return 0;
// }