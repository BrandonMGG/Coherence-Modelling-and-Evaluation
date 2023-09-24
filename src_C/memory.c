#include "../include/memory.h"

void memory_init(struct memory *memory) {
  for (int i = 0; i < MEM_SIZE; i++) {
    sprintf(memory->entries[i].address, "%01X", i);
    memory->entries[i].data = 0x0;
  }
}

int get_data_from_memory(struct memory *memory, char *address) {
  int i;

  for (i = 0; i < MEM_SIZE; i++) {
    if (strcmp(memory->entries[i].address, address) == 0) {
      return memory->entries[i].data;
    }
  }

  return -1;
}

void set_data_in_memory(struct memory *mem, char *address, int data) {
  int i;

  for (i = 0; i < MEM_SIZE; i++) {
    if (strcmp(mem->entries[i].address, address) == 0) {
      mem->entries[i].data = data;
      return;
    }
  }

  printf("Memory address not found.\n");
}

int main() {
  struct memory main_memory;

  memory_init(&main_memory);
  for (int i = 0; i < MEM_SIZE; i++) {
    printf("Address: %s, Data: %d \n", main_memory.entries[i].address, main_memory.entries[i].data);
  }
  
  // Memory ops testing
  main_memory.entries[15].data = 80;
  int data = get_data_from_memory(&main_memory, "F");
  printf("Data at address 0xf: %d\n", data);
  set_data_in_memory(&main_memory, "F", 100);
  data = get_data_from_memory(&main_memory, "F");
  printf("Data at address 0xf: %d\n", data);

  return 0;
}