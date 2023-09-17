#pragma once

#include <stdio.h>
#include "type.h"
#include "hashmap.h"

#define MAX_STR_SIZE 256

char ** LinkedListToArray(struct LinkedList *list);
HashTable *ReadIntoHashtable(char *file, bool fnv32Mode);
HashPair *ReadIntoPair(char *file, bool fnv32Mode);
struct LinkedList *ReadTokens(char *file);