#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct TblEntry {
    uint64_t hashKey;
    char *value;
    struct TblEntry *next;
}; 

typedef struct {
    struct TblEntry *ents;
    size_t c;
} HashTable;

HashTable *map_create(struct TblEntry *ents, size_t c);

int map_put(HashTable *tbl, uint64_t hashKey, const char *value);
void map_remove(HashTable *tbl, uint64_t hashKey);
char *map_get(HashTable *tbl, uint64_t hashKey);

struct TblEntry *map_entry_create();
void map_entry_init(struct TblEntry *ent, uint64_t hashKey, const char *value);
bool map_has(HashTable *tbl, uint64_t hashKey);
void map_free(HashTable *tbl);