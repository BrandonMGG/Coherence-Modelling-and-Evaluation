#include <stdio.h>
#include <stdlib.h>

#define CACHE_SIZE 4

// Estados de los bloques de caché
typedef enum {
    INVALID,
    EXCLUSIVE,
    SHARED,
    MODIFIED,
    OWNED
} CacheState;

// Bloque de caché
typedef struct {
    CacheState state;
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
void writeCacheBlock(Cache* cache, int tag, int data) {
    int index = tag % CACHE_SIZE;
    if (cache->blocks[index].state == INVALID) {
        cache->blocks[index].state = MODIFIED;
        printf("Write to Cache: MISS, Tag: %d, Value: %d, State: MODIFIED\n", tag, data);
    }
    else {
        cache->blocks[index].state = MODIFIED;
        printf("Write to Cache: HIT, Tag: %d, Value: %d, State: MODIFIED\n", tag, data);
    }
    cache->blocks[index].tag = tag;
    cache->blocks[index].data = data;
}

// Lee un bloque de la caché
int readCacheBlock(Cache* cache, int tag) {
    int index = tag % CACHE_SIZE;
    if (cache->blocks[index].state != INVALID && cache->blocks[index].tag == tag) {
        if (cache->blocks[index].state == MODIFIED) {
            cache->blocks[index].state = SHARED;
            printf("Read from Cache: HIT, Tag: %d, Value: %d, State: SHARED\n", tag, cache->blocks[index].data);
        }
        else {
            printf("Read from Cache: HIT, Tag: %d, Value: %d, State: %s\n", tag, cache->blocks[index].data, cache->blocks[index].state == EXCLUSIVE ? "EXCLUSIVE" : "SHARED");
        }
        return cache->blocks[index].data; // Cache hit
    }
    else {
        printf("Read from Cache: MISS, Tag: %d, Value: -1, State: INVALID\n", tag);
        return -1; // Cache miss
    }
}

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