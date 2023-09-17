#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "fnv.h"
#include "type.h"
#include "utils.h"
#include "hashmap.h"

HashPair *prefixes;
HashTable *suffixTable;

char *tokenFile = NULL;
int maxDepth = 0, charSetIndex = -1;

bool fnv32Mode = false;
bool tokenize = false;
char **tokenList = NULL;

#define CHARSET_LEN 9
char *charset[CHARSET_LEN] = {
    /*0*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    /*1*/ "abcdefghijklmnopqrstuvwxyz",
    /*2*/ "0123456789",
    /*3*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
    /*4*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    /*5*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_",
    /*6*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_",
    /*7*/ "abcdefghijklmnopqrstuvwxyz0123456789_",
    /*8*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
};

#define CHECKEMPTY(x) (x == NULL ? "" : x)

void check_32(char *gen) {
    uint64_t hash = 0;
    int i = 0;
    while(prefixes[i].hash){
        hash = FNVA1Hash_32(prefixes[i].hash, gen);
        if(map_has(suffixTable, hash)){
            char *v = map_get(suffixTable, hash);
            char buffer[MAX_STR_SIZE] = {};
            snprintf(buffer, MAX_STR_SIZE, "%s%s%s\n", CHECKEMPTY(prefixes[i].string), CHECKEMPTY(gen), CHECKEMPTY(v));
            fputs(buffer, stdout);
            FILE *fp = fopen("log_32.txt", "a");
            fwrite(buffer, strlen(buffer), 1, fp);
            fclose(fp);
            map_remove(suffixTable, hash); //increase odds of O(1)
        }
        
        i++;
    }
    return;
}

void check(char *gen) {
    uint64_t hash = 0;
    int i = 0;
    while(prefixes[i].hash){
        hash = FNVA1Hash(prefixes[i].hash, gen);
        if(map_has(suffixTable, hash)){
            char *v = map_get(suffixTable, hash);
            char buffer[MAX_STR_SIZE] = {};
            snprintf(buffer, MAX_STR_SIZE, "%s%s%s\n", CHECKEMPTY(prefixes[i].string), CHECKEMPTY(gen), CHECKEMPTY(v));
            fputs(buffer, stdout);
            FILE *fp = fopen("log.txt", "a");
            fwrite(buffer, strlen(buffer), 1, fp);
            fclose(fp);
            map_remove(suffixTable, hash); //increase odds of O(1)
        }
        
        i++;
    }
    return;
}

void BruteForce (char * genStr, char * insertion, size_t maxlen, char * alphabet, void (* callback)(char *)) {
    callback(genStr);
    if (!maxlen) return;
    insertion[1] = 0;
    for (char * p = alphabet; *p; p++) {
        *insertion = *p;
        BruteForce(genStr, insertion + 1, maxlen - 1, alphabet, callback);
    }
    *insertion = 0;
    return;
}

void BruteForceToken(char *genStr, char *insertion, size_t maxlen, struct LinkedList *tokens, void (*callback)(char *)) {
    callback(genStr);
    if (!maxlen) return;
    
    for (struct LinkedList *t = tokens; t->next != NULL; t = t->next) {
        strcpy(insertion, t->string);
        BruteForceToken(genStr, insertion + strlen(t->string), maxlen - 1, tokens, callback);
    }
    *insertion = 0;
    return;
}

//Parse arguments
void ParseArgs(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        switch(argv[i][0]) {
            case '-': 
            {
                if(!strcmp(argv[i], "-fnv32")) fnv32Mode = true;
                if(!strcmp(argv[i], "-t")) {
                    tokenize = true;
                    tokenFile = argv[++i];
                }
                if(!strcmp(argv[i], "-b")) {
                    tokenize = false;
                    charSetIndex = atoi(argv[++i]);
                }
                break;
            }
            default:
            {
                if(i == argc-1){
                    maxDepth = atoi(argv[i]);
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    if(argc < 4) {
        printf("Usage: %s [options] <depth>\n", argv[0]);
        printf(
            "Options:\n"
            "\t-b <charset>\t| brute force\n"
            "\t-t <token file>\t| tokize\n"
            "\t-fnv32\t\t| 32bit mode\n"
            "\nCharsets:\n"
            );
        for(int i = 0; i < CHARSET_LEN; i++)
            printf("\t%d - %s\n", i, charset[i]);
        return 1;
    }

    ParseArgs(argc, argv);
    
    
    if(!tokenize && charSetIndex < 0) {
        fputs("Error in arguments: pick charset or tokenize\n", stdout);
        return 1;
    }
    
    fputs("Running with options:\n", stdout);

    if(fnv32Mode) 
        fputs("FNV32 mode\n", stdout);
    if(tokenize) 
        printf("Tokenized with file\n%s\n", tokenFile);
    else 
        printf("Brute force with char set:\n%s\n", charset[charSetIndex]);
    
    //load tables/lists
    struct LinkedList* tokenLinkList = NULL;
    if(tokenize) {
        tokenLinkList = ReadTokens(tokenFile);
    }
    
    prefixes = ReadIntoPair("prefixes.bin", fnv32Mode);
    suffixTable = ReadIntoHashtable("suffixes.bin", fnv32Mode);
    
    printf("--------------------------\n");

    char *genStr = calloc(1, MAX_STR_SIZE);
    if(tokenize)
        BruteForceToken(genStr, genStr, maxDepth, tokenLinkList, fnv32Mode ? check_32 : check);
    else
        BruteForce(genStr, genStr, maxDepth, charset[charSetIndex], fnv32Mode ? check_32 : check);
    
    map_free(suffixTable);
    
    return 0;
}