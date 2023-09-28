#include <stdio.h>
#include <stdlib.h>

#define CACHE_SIZE 4

// Estados de los bloques de caché
typedef enum {
    INVALID,
    EXCLUSIVE,
    SHARED,
    MODIFIED,
    OWNED,
    UNKNOWN
} CacheState;

// Bloque de caché
typedef struct {
    CacheState state;
    int address;
    int tag;
    int data;
} CacheBlock;

// Caché
typedef struct {
    CacheBlock blocks[CACHE_SIZE];
} Cache;

// Inicializa la caché
void initializeCache(Cache* cache) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache->blocks[i].state = INVALID;
        cache->blocks[i].tag = -1;
        cache->blocks[i].data = 0;
    }
}

// Escribe un bloque en la caché
void writeCacheBlock(Cache* cache, int address, int data) {
    int index = 0;
    char message[10] = " ";
    for(int i=0; i<CACHE_SIZE; i++){
        //Ver si existe la direccion de memoria en cache
        if(cache->blocks[i].address == address){
            index = cache->blocks[i].tag;
            strcpy((char*)message, "Write Cache Hit");
        }else{
            //no existe en cache
            /*enviar mensaje de message = {
                    "id": core_name,
                    "access": AccessType.writemiss,
                    "address": address,
                    "value": value,
                    "block_id": self._get_block_id_with_writing_policy(address),
                }*/
            strcpy((char*)message, "Write Cache Miss");
        }
        
    }

    if (message == "Write Cache Hit"){
        if(cache->blocks[index].state != MODIFIED || cache->blocks[index].state != EXCLUSIVE){
            /*enviar mensaje de message = {
                    "id": core_name,
                    "access": AccessType.writemiss,
                    "address": address,
                    "value": value,
                    "block_id": block.get_id(),
                }*/
            strcpy((char*)message, "Write Cache Miss");
        }else{
            cache->blocks[index].state = MODIFIED;
            cache->blocks[index].data = data;
            strcpy((char*)message, "Write Cache Hit");
        }
        
    }
    
}

// Lee un bloque de la caché
int readCacheBlock(Cache* cache, int address) {
    int index = 0;
    char message[10] = " ";
    for(int i=0; i<CACHE_SIZE; i++){
        //Ver si existe la direccion de memoria en cache
        if(cache->blocks[i].address == address){
            index = cache->blocks[i].tag;
            if(cache->blocks[index].state != INVALID){
                //retorna valor de cache
                strcpy((char*)message, "Read Cache Hit");
            }else{
                //si existe pero esta en Invalid
                /*enviar mensaje de message = {
                        "id": core_name,
                        "access": AccessType.readmiss,
                        "address": address,
                        "value": value,
                        "block_id": block_id,
                    }*/
                strcpy((char*)message, "Read Cache Miss");
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
            strcpy((char*)message, "Read Cache Miss");
        }
        
    }
}

// Función para seleccionar un bloque de caché con política write-back
int getBlockIdWithWriteBackPolicy(Cache* cache, int cacheSize) {
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

// Obtiene información de un bloque de caché dado su tag
void getCacheBlockInfo(Cache* cache, int tag) {
    int index = tag % CACHE_SIZE;
    CacheBlock block = cache->blocks[index];
    
    const char* state_str = (block.state == EXCLUSIVE) ? "EXCLUSIVE" : 
                            (block.state == SHARED) ? "SHARED" : 
                            (block.state == MODIFIED) ? "MODIFIED" : 
                            (block.state == INVALID) ? "INVALID" :
                            (block.state == INVALID) ? "OWNED":

    printf("Cache Block Info: Tag: %d, State: %s, Value: %d\n", tag, state_str, block.data);
}*/
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