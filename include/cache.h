#include <mqueue.h>
#define CACHE_SIZE 4
#define SIZE 4

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
    char address[CACHE_SIZE];
    int tag;
    int data;
} CacheBlock;

// Caché
typedef struct {
    CacheBlock blocks[CACHE_SIZE];
} Cache;


// Inicializa la caché
void initializeCache(Cache* cache);
// Escribe un bloque en la caché
int writeCacheBlock(Cache* cache, char address[CACHE_SIZE], int data ,int id, mqd_t mq);

// Lee un bloque de la caché
int readCacheBlock(Cache* cache, char address[CACHE_SIZE], int id, mqd_t mq);

// Función para seleccionar un bloque de caché con política write-back
int getBlockIdWithWriteBackPolicy(Cache* cache);

// Obtiene información de un bloque de caché dado su tag
int getCacheBlock_by_address(Cache* cache, char address[CACHE_SIZE]);