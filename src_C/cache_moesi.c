#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "../include/messages.h"
#include "../include/cache.h"



// Inicializa la caché
void initializeCache(Cache* cache) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache->blocks[i].state = INVALID;
        cache->blocks[i].tag = i;
        cache->blocks[i].data = 0;
    }
}

// Escribe un bloque en la caché
void writeCacheBlock(Cache* cache, char address[CACHE_SIZE], int data, int id) {
    //mqd_t mq;
    //mq = create_message_queue();
    //srand(time(NULL));
    struct Message message;
    
    int index = 0;
    char message1[20] = " ";
    for(int i=0; i<CACHE_SIZE; i++){
        //Ver si existe la direccion de memoria en cache
        if(cache->blocks[i].address == address){
            index = cache->blocks[i].tag;
            strcpy((char*)message1, "Write Cache Hit");
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
            message.access = 0;
            snprintf(message.address, sizeof(message.address), "%s", address);
            message.block_id = getBlockIdWithWriteBackPolicy(cache);
            message.value = data;

            printf("Write no esta en cache\n");
            break;
            strcpy((char*)message1, "Write Cache Miss");
        }
        
    }

    if (message1 == "Write Cache Hit"){
        if(cache->blocks[index].state != MODIFIED || cache->blocks[index].state != EXCLUSIVE){
            /*enviar mensaje de message = {
                    "id": core_name,
                    "access": AccessType.writemiss,
                    "address": address,
                    "value": value,
                    "block_id": block.get_id(),
                }*/

            //envio del mensaje
            message.id = id;
            message.access = 0;
            snprintf(message.address, sizeof(message.address), "%s", address);
            message.value = data;
            message.block_id = index;

            strcpy((char*)message1, "Write Cache Miss");
            printf("Write no esta en Modified o Exclusive\n");
        }else{
            cache->blocks[index].state = MODIFIED;
            cache->blocks[index].data = data;
            //strcpy((char*)message, "Write Cache Hit");
            printf("Write si estaba en cache y se modifico\n");
        }
        
    }
    
}

// Lee un bloque de la caché
int readCacheBlock(Cache* cache, char address[CACHE_SIZE], int id) {
    int index = 0;
    char message2[20] = " ";
    struct Message message;
    for(int i=0; i<CACHE_SIZE; i++){
        //Ver si existe la direccion de memoria en cache
        if(cache->blocks[i].address == address){
            index = cache->blocks[i].tag;
            if(cache->blocks[index].state != INVALID){
                //retorna valor de cache
                strcpy((char*)message2, "Read Cache Hit");
                printf("Read esta en cache\n");
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
            message.access = 1;
            snprintf(message.address, sizeof(message.address), "%s", address);
            message.block_id = id;

                strcpy((char*)message2, "Read Cache Miss");
                printf("Read esta en cache pero en INvalid\n");
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
            message.access = 1;
            snprintf(message.address, sizeof(message.address), "%s", address);
            message.block_id = getBlockIdWithWriteBackPolicy(cache);
            
            strcpy((char*)message2, "Read Cache Miss");
            printf("Read no esta en cache\n");
            break;
        }
        
    }
}

// Función para seleccionar un bloque de caché con política write-back
int getBlockIdWithWriteBackPolicy(Cache* cache) {
    // Primero, busca bloques en estado 'M' (Modified)
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].blocks->state == MODIFIED) {
            return cache[i].blocks->tag;
        }
    }

    // Si no hay bloques en estado 'M', busca bloques en estado 'E' (Exclusive)
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].blocks->state == EXCLUSIVE) {
            return cache[i].blocks->tag;
        }
    }

    // Si no hay bloques en estado 'M' ni 'E', selecciona el primer bloque en la caché
    if (CACHE_SIZE > 0) {
        return cache[0].blocks->tag;
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
int getCacheBlock_by_address(Cache* cache, char address[CACHE_SIZE]) {
    int index =0;
    for(int i=0; i<CACHE_SIZE; i++){
        //Ver si existe la direccion de memoria en cache
        if(cache->blocks[i].address == address){
            index = cache->blocks[i].tag;
            return index;
        }else{
            return (-1);
        }
        
    }
}
/*
int main() {
    Cache cache;
    initializeCache(&cache);

    int tag = 1;
    int data = 45;

    writeCacheBlock(&cache, tag, data);

    readCacheBlock(&cache, tag);

    writeCacheBlock(&cache, 2, 40);

    setCacheBlockState(&cache, 2, EXCLUSIVE);

    getCacheBlockInfo(&cache, 2);

    return 0;
}
*/