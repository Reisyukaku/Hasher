#pragma once

#include "type.h"
#include "hashmap.h"

#define MAX_STR_SIZE 256

#define R_FAIL(err, msg, ...) if(err != 0){ printf(msg, ##__VA_ARGS__); return -1; }

char ** LinkedListToArray(struct LinkedList *list);
HashTable *ReadIntoHashtable(char *file);
HashPair *ReadIntoPair(char *file, int *cnt);
struct LinkedList *ReadTokens(char *file);
void FreeTokens(struct LinkedList *list);
int atoi(char *p);