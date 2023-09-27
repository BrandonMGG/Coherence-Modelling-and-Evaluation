#include "../include/memory.h"
#include "../include/bus.h"

/**
 * Bus abstraction
*/

void process_readmiss(){

}

void process_writemiss(){

}

void perform_wb(char * dirty_address, int dirty_data, struct bus *bus){
  //Needs delay to simulate latency
  set_data_in_memory(&(bus->main_memory), dirty_address, dirty_data);
}

int seek_owned(char * local_address, int requester_id){

}

int seek_shared(char * local_address, int requester_id){

}

void seek_invalidate(char * local_address, int requester_id){

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