#include "hashmap.h"

HashTable *map_create(struct TblEntry *ents, size_t c) {
    HashTable *tbl;
    tbl = malloc(sizeof(HashTable));
    if (tbl == NULL) {
        return NULL;
    }
    tbl->ents = ents;
    tbl->c = c;
    for (size_t i = 0; i < c; ++i) {
        map_entry_init(&ents[i], 0, NULL);
    }
    return tbl;
}

struct TblEntry *getTail(struct TblEntry *ent, uint64_t hashKey) {
    struct TblEntry *tail = ent;
    while (tail->next) {
        tail = (struct TblEntry*)tail->next;
        if (tail->hashKey == hashKey) {
            return NULL;
        }
    }
    return tail;
}

int map_put(HashTable *tbl, uint64_t hashKey, const char *value) {
    uint32_t h = hashKey % tbl->c;
    struct TblEntry *ents = tbl->ents;
    if (ents[h].hashKey != 0) {
        if (ents[h].hashKey == hashKey) {
            return 1;
        }
        struct TblEntry *tail = getTail(&ents[h], hashKey);
        if (!tail) {
            return 1;
        }
        tail->next = map_entry_create();
        map_entry_init((struct TblEntry*)tail->next, hashKey, value);
    } else {
        map_entry_init(&ents[h], hashKey, value);
    }
    return 0;
}

char *map_get(HashTable *tbl, uint64_t hashKey) {
    struct TblEntry *ents = tbl->ents;
    uint32_t h = hashKey % tbl->c;
    if (ents[h].hashKey == 0) {
        return NULL;
    }
    if (ents[h].hashKey == hashKey) {
        return ents[h].value;
    } else {
        struct TblEntry *tail = &ents[h];
        while (tail->next) {
            tail = (struct TblEntry*)tail->next;
            if (tail->hashKey == hashKey) {
                return tail->value;
            }
        }
    }
    return NULL;
}

bool map_has(HashTable *tbl, uint64_t hashKey) {
    struct TblEntry *ents = tbl->ents;
    uint32_t h = hashKey % tbl->c;
    if (ents[h].hashKey == 0) return false;
    if (ents[h].hashKey == hashKey) return true;
    else {
        struct TblEntry *tail = &ents[h];
        while (tail->next) {
            tail = (struct TblEntry*)tail->next;
            if (tail->hashKey == hashKey) return true;
        }
    }
    return false;
}

void map_remove(HashTable *tbl, uint64_t hashKey) {
    uint32_t h = hashKey % tbl->c;
    struct TblEntry *tail = &tbl->ents[h];
    if (!tail || tail->hashKey == 0) return;
    if (tail->hashKey == hashKey) {
        tail->hashKey = 0;
        tail->value = NULL;
        if (tail->next) {
            struct TblEntry *new_next = ((struct TblEntry*)tail->next)->next;
            tail->hashKey = ((struct TblEntry*)tail->next)->hashKey;
            tail->value = ((struct TblEntry*)tail->next)->value;
            if (tail->next != NULL) {
                free(tail->next);
                tail->next = NULL;
            } 
            tail->next = new_next;
        }
    } else {
        while (tail->next) {
            struct TblEntry *current = tail;
            tail = tail->next;
            if (tail->hashKey == hashKey) {
                struct TblEntry *next;
                next = tail->next;
                if (tail != NULL) {
                    free(tail);
                    tail = NULL;
                } 
                if (next) {
                    current->next = next;
                } else {
                    current->next = NULL;
                }
                return;
            }
        }
        return;
    }
    return;
}

struct TblEntry *map_entry_create() {
    struct TblEntry *ent;
    ent = malloc(sizeof(struct TblEntry));
    if (ent == NULL) return NULL;
    return ent;
}

void map_entry_init(struct TblEntry *ent, uint64_t hashKey, const char *value) {
    ent->hashKey  = hashKey;
    ent->value  = (char *)value;
    ent->next = NULL;
}

void map_free(HashTable *tbl) {
    struct TblEntry *ents = tbl->ents;
    size_t c = tbl->c;
    for (size_t i = 0; i < c; ++i) {
        struct TblEntry *tail = (struct TblEntry*)ents[i].next;
        while (tail) {
            struct TblEntry *current = tail;
            tail = (struct TblEntry *)tail->next;
            if (current != NULL) {
                free(current);
                current = NULL;
            } 
        }
    }
    if (tbl != NULL) {
        free(tbl);
        tbl = NULL;
    } 
}