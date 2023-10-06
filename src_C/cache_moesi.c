#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
//#include "../include/messages.h"
#include "../include/cache.h"
#include <mqueue.h>
#include "../include/messages.h"

// Inicializa la caché
void initializeCache(Cache* cache) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache->blocks[i].state = INVALID;
        cache->blocks[i].tag = i;
        cache->blocks[i].data = 0;
        snprintf(cache->blocks[i].address, sizeof(cache->blocks[i].address), "-1");
    }
}

// Escribe un bloque en la caché
int writeCacheBlock(Cache* cache, char address[SIZE], int data, int id, mqd_t mq) {
    //mqd_t mq;
    //mq = create_message_queue();
    //srand(time(NULL));
    struct Message message;
    
    int index = 0;
    char message1[20] = " ";
    for(int i=0; i<CACHE_SIZE; i++){
        //Ver si existe la direccion de memoria en cache
        if(strcmp(cache->blocks[i].address, address) == 0){
            index = cache->blocks[i].tag;
            strcpy((char*)message1, "Write Cache Hit");
            printf("State: %d, Cache: %s\n", cache->blocks[index].state, cache->blocks[0].address);
            if((cache->blocks[index].state != MODIFIED) && (cache->blocks[index].state != EXCLUSIVE)){
                /*enviar mensaje de message = {
                        "id": core_name,
                        "access": AccessType.writemiss,
                        "address": address,
                        "value": value,
                        "block_id": block.get_id(),
                    }*/

                //envio del mensaje
                message.id = id;
                message.access = 1;
                snprintf(message.address, sizeof(message.address), "%s", address);
                message.value = data;
                message.block_id = index;
                printf("Write esta en cach[e pero no en Modified o Exclusive\n");
                // Send the message to the queue
                //send_message(mq, &message);
                // Sleep briefly to simulate processing time
                sleep(4); // Sleep for 2 s

                return -1;
                
                
            }else{
                cache->blocks[index].state = MODIFIED;
                cache->blocks[index].data = data;
                
                //strcpy((char*)message, "Write Cache Hit");
                printf("Write si estaba en cache y se modifico\n");
                return 1;
                
            }
            //break;
        }else{
            //no existe en cache
            /*enviar mensaje de message = {
                    "id": core_name,
                    "access": AccessType.writemiss,
                    "address": address,
                    "value": value,
                    "block_id": self._get_block_id_with_writing_policy(address),
                }*/
            //envio del mensaje
            message.id = id;
            message.access = 1;
            snprintf(message.address, sizeof(message.address), "%s", address);
            message.block_id = getBlockIdWithWriteBackPolicy(cache);
            message.value = data;
            printf("Write no esta en cache\n");
            // Send the message to the queue
            //send_message(mq, &message);
            // Sleep briefly to simulate processing time
            sleep(4); // Sleep for 2 s

            
            
            strcpy((char*)message1, "Write Cache Miss");
            //break;
            return -1;
        }
        
    }
    return -1;
}

// Lee un bloque de la caché
int readCacheBlock(Cache* cache, char address[SIZE], int id, mqd_t mq) {
    int index = 0;
    char message2[20] = " ";
    struct Message message;
    //printf("Address: %s, Cache: %s\n", address, cache->blocks[0].address);
    for(int i=0; i<CACHE_SIZE; i++){
        //Ver si existe la direccion de memoria en cache
        if( strcmp(cache->blocks[i].address, address) == 0){  
            index = cache->blocks[i].tag;
            if(cache->blocks[index].state != INVALID){
                //retorna valor de cache
                //strcpy((char*)message2, "Read Cache Hit");
                printf("Read esta en cache\n");
                break;
            }else{
                //si existe pero esta en Invalid
                /*enviar mensaje de message = {
                        "id": core_name,
                        "access": AccessType.readmiss,
                        "address": address,
                        "value": value,
                        "block_id": block_id,
                    }*/
                //envio del mensaje
                message.id = id;
                message.access = 0;
                snprintf(message.address, sizeof(message.address), "%s", address);
                message.block_id = cache->blocks[index].tag;
                printf("Read esta en cache pero en Invalid\n");
                // Send the message to the queue
                //send_message(mq, &message);
                // Sleep briefly to simulate processing time
                //sleep(4); // Sleep for 2 s
                //strcpy((char*)message2, "Read Cache Miss");
                
                break;
            }
        }else{
            //no existe en cache
            /*enviar mensaje de message = {
                    "id": core_name,
                    "access": AccessType.readmiss,
                    "address": address,
                    "value": value,
                    "block_id": self._get_block_id_with_writing_policy(address),
                }*/
            //envio del mensaje
            message.id = id;
            message.access = 0;
            snprintf(message.address, sizeof(message.address), "%s", address);
            message.block_id = getBlockIdWithWriteBackPolicy(cache);
            printf("Read no esta en cache write back\n");
            // Send the message to the queue
            //send_message(mq, &message);
            // Sleep briefly to simulate processing time
            //sleep(4); // Sleep for 2 s

            //strcpy((char*)message2, "Read Cache Miss write back");
            
            break;
        }
        
    }
    return 0;
}

// Función para seleccionar un bloque de caché con política write-back
int getBlockIdWithWriteBackPolicy(Cache *cache) {
    // Primero, busca bloques en estado 'Invalid' 
    
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache->blocks[i].state == INVALID) {
            return cache->blocks[i].tag;
        }
    }

    // Si no hay bloques en estado 'I', busca bloques en estado 'M' (Modifield)
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache->blocks[i].state == MODIFIED) {
            return cache->blocks[i].tag;
        }
    }

    // Si no hay bloques en estado 'M' ni 'E', selecciona el primer bloque en la caché
    if (CACHE_SIZE > 0) {
        return cache->blocks[0].tag;
    }
    
    // Error - Política de escritura write-back
    printf("Error - Write-Back Policy\n");
    return -1;
}


/*
// Establece el estado de un bloque de caché dado su tag
void setCacheBlockState(Cache* cache, int tag, CacheState state) {
    int index = tag % CACHE_SIZE;
    cache->blocks[index].state = state;
    printf("Set Cache Block State: Tag: %d, State: %s\n", tag, state == EXCLUSIVE ? "EXCLUSIVE" : (state == SHARED ? "SHARED" : "MODIFIED"));
}
*/
// Obtiene información de un bloque de caché dado su tag
int getCacheBlock_by_address(Cache* cache, char address[SIZE]) {
    int index = 0;
    for(int i = 0; i<CACHE_SIZE; i++){
        //Ver si existe la direccion de memoria en cache
        if(strcmp(cache->blocks[i].address, address) == 0){
            index = cache->blocks[i].tag;
            return index;
        }        
    }
    return -1;
}

/*
int main() {
    Cache cache;
    initializeCache(&cache);
    printf("-------------------------------------------Inicializa la Caché----------------------------------------\n");
    
    for(int i =0; i < CACHE_SIZE ; i++){
      printf("Cache 0 Tag -> %d Address -> %s State -> %d Value -> %d \n",cache.blocks[i].tag ,cache.blocks[i].address ,cache.blocks[i].state ,cache.blocks[i].data);
    }
    printf("-------------------------------------------Inicializa la Caché----------------------------------------\n");

    int tag = 1;
    int data = 45;
    char address[SIZE]= "0xB";
    int id =1;
    mqd_t mq;
    /*
    printf("-------------------------------------------Write Caso 1----------------------------------------\n");
    printf("Datos de entrada -> Address: %s ,Dato: %d ,ID core: %d \n",address,data,id);
    writeCacheBlock( &cache,  address, data, id,  mq);
    for(int i =0; i < CACHE_SIZE ; i++){
      printf("Cache 0 Tag -> %d Address -> %s State -> %d Value -> %d \n",cache.blocks[i].tag ,cache.blocks[i].address ,cache.blocks[i].state ,cache.blocks[i].data);
    }
    printf("-------------------------------------------Write Caso 1----------------------------------------\n");

    
    snprintf(cache.blocks[0].address, sizeof(cache.blocks[0].address), "0xB");
    printf("-------------------------------------------Write Caso 2----------------------------------------\n");
    printf("Se agrego el address 0xB al bloque 0\n");
    printf("Datos de entrada -> Address: %s ,Dato: %d ,ID core: %d \n",address,data,id);
    writeCacheBlock( &cache,  address, data, id,  mq);
    for(int i =0; i < CACHE_SIZE ; i++){
      printf("Cache 0 Tag -> %d Address -> %s State -> %d Value -> %d \n",cache.blocks[i].tag ,cache.blocks[i].address ,cache.blocks[i].state ,cache.blocks[i].data);
    }
    printf("-------------------------------------------Write Caso 2----------------------------------------\n");

    cache.blocks[0].state= MODIFIED;
    printf("-------------------------------------------Write Caso 3----------------------------------------\n");
    printf("Se agrego el address 0xB al bloque 0\n");
    printf("Se agrego el estado MODIFIELD o 3 al bloque 0\n");
    printf("Datos de entrada -> Address: %s ,Dato: %d ,ID core: %d \n",address,data,id);
    writeCacheBlock( &cache,  address, data, id,  mq);
    for(int i =0; i < CACHE_SIZE ; i++){
      printf("Cache 0 Tag -> %d Address -> %s State -> %d Value -> %d \n",cache.blocks[i].tag ,cache.blocks[i].address ,cache.blocks[i].state ,cache.blocks[i].data);
    }
    printf("-------------------------------------------Write Caso 3----------------------------------------\n");
    
    printf("-------------------------------------------Read Caso 1----------------------------------------\n");
    printf("Datos de entrada -> Address: %s ,Dato: %d ,ID core: %d \n",address,data,id);
    readCacheBlock(&cache, address,  id, mq);
    for(int i =0; i < CACHE_SIZE ; i++){
      printf("Cache 0 Tag -> %d Address -> %s State -> %d Value -> %d \n",cache.blocks[i].tag ,cache.blocks[i].address ,cache.blocks[i].state ,cache.blocks[i].data);
    }
    printf("-------------------------------------------Read Caso 1----------------------------------------\n");

    snprintf(cache.blocks[0].address, sizeof(cache.blocks[0].address), "0xB");
    cache.blocks[0].data =9;
    printf("-------------------------------------------Read Caso 2----------------------------------------\n");
    printf("Se agrego el address 0xB al bloque 0, con dato = 9\n");
    printf("Datos de entrada -> Address: %s ,Dato: %d ,ID core: %d \n",address,data,id);
    readCacheBlock(&cache, address,  id, mq);
    for(int i =0; i < CACHE_SIZE ; i++){
      printf("Cache 0 Tag -> %d Address -> %s State -> %d Value -> %d \n",cache.blocks[i].tag ,cache.blocks[i].address ,cache.blocks[i].state ,cache.blocks[i].data);
    }
    printf("-------------------------------------------Read Caso 2----------------------------------------\n");

    
    cache.blocks[0].state= MODIFIED;
    printf("-------------------------------------------Read Caso 3----------------------------------------\n");
    printf("Se agrego el address 0xB al bloque 0, con dato = 9\n");
    printf("Se agrego el estado MODIFIELD o 3 al bloque 0\n");
    printf("Datos de entrada -> Address: %s ,Dato: %d ,ID core: %d \n",address,data,id);
    readCacheBlock(&cache, address,  id, mq);
    for(int i =0; i < CACHE_SIZE ; i++){
      printf("Cache 0 Tag -> %d Address -> %s State -> %d Value -> %d \n",cache.blocks[i].tag ,cache.blocks[i].address ,cache.blocks[i].state ,cache.blocks[i].data);
    }
    printf("-------------------------------------------Read Caso 3----------------------------------------\n");
    
    snprintf(cache.blocks[0].address, sizeof(cache.blocks[0].address), "0xA");
    cache.blocks[0].data =9;
    cache.blocks[0].state= SHARED;

    snprintf(cache.blocks[1].address, sizeof(cache.blocks[1].address), "0xB");
    cache.blocks[1].data =11;
    cache.blocks[1].state= OWNED;
    
    snprintf(cache.blocks[2].address, sizeof(cache.blocks[2].address), "0xC");
    cache.blocks[2].data =23;
    cache.blocks[2].state= MODIFIED;

    snprintf(cache.blocks[3].address, sizeof(cache.blocks[3].address), "0xD");
    cache.blocks[3].data =4;
    cache.blocks[3].state= EXCLUSIVE;

    for(int i =0; i < CACHE_SIZE ; i++){
      printf("Cache 0 Tag -> %d Address -> %s State -> %d Value -> %d \n",cache.blocks[i].tag ,cache.blocks[i].address ,cache.blocks[i].state ,cache.blocks[i].data);
    }

    printf("Resultado de Write-Back = %d \n",getBlockIdWithWriteBackPolicy(&cache));

    printf("Resultado de Pedir tag de caché por Address 0xB = %d \n",getCacheBlock_by_address(&cache,"0xB"));

    return 0;
}
*/