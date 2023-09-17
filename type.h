#pragma once

#include <stdint.h>

typedef struct {
    uint64_t hash;
    char *string;
} HashPair;

struct LinkedList{
    char *string;
    struct LinkedList *next;
};