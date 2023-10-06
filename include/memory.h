#include <stdio.h>
#include <string.h>
#define MEM_SIZE 4 // 16 blocks of memory
#define DATA_SIZE 4 // each char value is 1 byte long, so data is 4 bytes long or 32-bit


struct memory_block {
  char address[DATA_SIZE];
  int data;
};
struct memory {
  struct memory_block entries[MEM_SIZE];
};


/**
 * Initializes the memory structure.
 *
 * @param mem A pointer to the memory structure to initialize.
 */
void memory_init(struct memory *memory);

/**
 * Gets the data from the specified memory address.
 *
 * @param memory A pointer to the memory structure.
 * @param address The address of the memory location from which to get the data.
 * @return The data at the specified memory location, or -1 if the memory location is not found.
 * Example: 
 * int data = get_data_from_memory(&memory, "A");
 */
int get_data_from_memory(struct memory *memory, char *address);

/**
 * Sets the data at the specified memory address.
 *
 * @param mem A pointer to the memory structure.
 * @param address The address of the memory location to set the data.
 * @param data The data to set the memory location.
 * @return Nothing.
 */
void set_data_in_memory(struct memory *mem, char *address, int data);