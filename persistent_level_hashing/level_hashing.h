#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include "hash.h"
#include "log.h"

#define ASSOC_NUM 4                       // The number of slots in a bucket, should be smaller than 32

// set the n-th bit to 0 or 1
#define SET_BIT(token, n, bit) (bit ? (token|=(1<<n)) : (token&=~(1<<n)))

// get the n-th bit
#define GET_BIT(token, n)   (token & (1<<n))

typedef struct entry{                     // A slot storing a key-value item 
    uint8_t key[KEY_LEN];                 // KEY_LEN and VALUE_LEN are defined in log.h
    uint8_t value[VALUE_LEN];
} entry;

typedef struct level_bucket               // A bucket, put the tokens behind the items to ensure the token area is within the last bytes of the cache line 
{
    entry slot[ASSOC_NUM];
    uint32_t token;                       // each bit in the last ASSOC_NUM bits is used to indicate whether its corresponding slot is empty
} level_bucket;                           // 128 byte; one bucket should be cache-line-aligned

typedef struct level_hash {               // A Level hash table
    level_bucket *buckets[2];             // The top level and bottom level in the Level hash table
    level_bucket *interim_level_buckets;  // Used during resizing;
    uint64_t level_item_num[2];           // The numbers of items stored in the top and bottom levels respectively
    uint64_t addr_capacity;               // The number of buckets in the top level
    uint64_t total_capacity;              // The number of all buckets in the Level hash table    
    uint64_t level_size;                  // level_size = log2(addr_capacity)
    uint8_t level_expand_time;            // Indicate whether the Level hash table was expanded, ">1 or =1": Yes, "0": No;
    uint8_t resize_state;                 // Indicate the resizing state of the level hash table, ‘0’ means the hash table is not during resizing; 
                                          // ‘1’ means the hash table is being expanded; ‘2’ means the hash table is being shrunk.
    uint64_t f_seed;
    uint64_t s_seed;                      // Two randomized seeds for hash functions

    level_log *log;                       // The log
} level_hash;

level_hash *level_init(const char*, uint64_t level_size);     

uint8_t level_insert(level_hash *level, uint8_t *key, uint8_t *value);          

uint8_t* level_static_query(level_hash *level, uint8_t *key);

uint8_t* level_dynamic_query(level_hash *level, uint8_t *key);

uint8_t level_delete(level_hash *level, uint8_t*key);

uint8_t level_update(level_hash *level, uint8_t *key, uint8_t *new_value);

void level_expand(level_hash *level);

void level_shrink(level_hash *level);

uint8_t try_movement(level_hash *level, uint64_t idx, uint64_t level_num, uint8_t *key, uint8_t *value);

int b2t_movement(level_hash *level, uint64_t idx);

void level_destroy(level_hash *level);