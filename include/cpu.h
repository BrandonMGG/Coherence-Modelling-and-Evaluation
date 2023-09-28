// Estados de los bloques de caché
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
    char address[CACHE_SIZE];
    int tag;
    int data;
} CacheBlock;

// Caché
typedef struct {
    CacheBlock blocks[CACHE_SIZE];
} Cache;

struct CPU {
    int id; 
    Cache cache;
};

