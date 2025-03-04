#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "pflush.h"

#define KEY_LEN 16                        // The maximum length of a key
#define VALUE_LEN 15                      // The maximum length of a value

typedef struct log_entry{                
    uint8_t key[KEY_LEN];
    uint8_t value[VALUE_LEN];
    uint8_t flag;
} log_entry;

// The log entry used for movements in insert operations
typedef struct log_entry_insert{                
    uint64_t flag: 1;
    uint64_t level: 1;
    uint64_t slot: 6;
    uint64_t bucket: 56;
} log_entry_insert;            // cache-line aligned

typedef struct level_log
{ 
    uint64_t log_length;
    log_entry* entry;
    uint64_t current;

    log_entry_insert* entry_insert;
    uint64_t current_insert;
}level_log;

level_log* log_create(uint64_t log_length);

void log_write(level_log *log, uint8_t *key, uint8_t *value);

void log_clean(level_log *log);

void log_insert_write(level_log *log, log_entry_insert entry);

void log_insert_clean(level_log *log);
