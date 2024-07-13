#pragma once

#include <malloc.h>
#include <stdint.h>
#include <stdbool.h>

#define  printf __builtin_printf
#define  strlen __builtin_strlen
#define  strcpy __builtin_strcpy
#define  strncpy __builtin_strncpy
#define  strcmp __builtin_strcmp

typedef struct {
    uint64_t hash;
    char *string;
} HashPair;

struct LinkedList{
    char *string;
    struct LinkedList *next;
};