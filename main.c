#include <pthread.h>
#include "fnv.h"
#include "type.h"
#include "utils.h"
#include "hashmap.h"

HashPair *prefixes;
HashTable *suffixTable;

char *tokenFile = NULL;
int maxDepth = 0, charSetIndex = -1;
unsigned startIndex = 0;
struct LinkedList *offsetList = NULL;
struct LinkedList *tokenLinkList = NULL;

bool tokenize = false;
char **tokenList = NULL;

pthread_mutex_t log_mutex;

#define CHARSET_LEN 10
char *charset[CHARSET_LEN] = {
    /*0*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    /*1*/ "abcdefghijklmnopqrstuvwxyz",
    /*2*/ "0123456789",
    /*3*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
    /*4*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    /*5*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_",
    /*6*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_",
    /*7*/ "abcdefghijklmnopqrstuvwxyz0123456789_",
    /*8*/ "ABCDEFGHIJKLMNOPQRSTUVWXYZ_",
    /*9*/ "SREAITONLDsreaitonld"
};

#define CHECKEMPTY(x) (x == NULL ? "" : x)

static inline void check(int pref, char *gen, uint64_t hash) {
    
    //printf("%d %s%s | %" PRIx64 "\n", pref, prefixes[pref].string, gen, hash);
    if(map_has(suffixTable, hash)){
        pthread_mutex_lock(&log_mutex);
        char *v = map_get(suffixTable, hash);
        char buffer[MAX_STR_SIZE] = {};
        snprintf(buffer, MAX_STR_SIZE, "%s%s%s\n", CHECKEMPTY(prefixes[pref].string), CHECKEMPTY(gen), CHECKEMPTY(v));
        fputs(buffer, stdout);
        FILE *fp = fopen("log.txt", "a");
        fwrite(buffer, strlen(buffer), 1, fp);
        fclose(fp);
        map_remove(suffixTable, hash); //increase odds of O(1)
        pthread_mutex_unlock(&log_mutex);
    }
    
    return;
}

void BruteForce (char * genStr, char * insertion, int pref, uint64_t hash, size_t maxlen, char * alphabet, void (* callback)(int, char *, uint64_t)) {
    callback(pref, genStr, hash);

    if (!maxlen) return;
    insertion[1] = 0;

    char *p = alphabet;
    uint64_t h = hash;

    if(startIndex) {
        p += startIndex;
        startIndex = 0;
    }
    
    for (; *p; p++) {
        *insertion = *p;
        h = (hash ^ *p) * FNV64_PRIME;
        BruteForce(genStr, insertion + 1, pref, h, maxlen - 1, alphabet, callback);
    }
    *insertion = 0;
    return;
}

void BruteForceToken(char *genStr, char *insertion, int pref, uint64_t hash, size_t maxlen, struct LinkedList *tokens, void (*callback)(int, char *, uint64_t)) {
    callback(pref, genStr, hash);
    if (!maxlen) return;

    struct LinkedList *t = tokens;
    uint64_t h = hash;
    
    if(startIndex) {
        for(unsigned i = 0; i < startIndex; i++) 
            t = t->next;
        startIndex = 0;
    }
    
    for (; t->next != NULL; t = t->next) {
        size_t len = strlen(t->string);
        strncpy(insertion, t->string, len);
        insertion[len] = 0;
        h = FNVA1Hash(hash, t->string);
        BruteForceToken(genStr, insertion + len, pref, h, maxlen - 1, tokens, callback);
    }
    *insertion = 0;
    return;
}
#include <assert.h>
void *DoWork(void *args)
{
    int i = *(int*)args;
    free(args);

    

    char *genStr = (char *)calloc(1, MAX_STR_SIZE);

    if(tokenize)
        BruteForceToken(genStr, genStr, i, FNVA1Hash(FNV64_BASIS, prefixes[i].string), maxDepth, tokenLinkList, check);
    else
        BruteForce(genStr, genStr, i, FNVA1Hash(FNV64_BASIS, prefixes[i].string), maxDepth, charset[charSetIndex], check);

    free(genStr);

    pthread_exit(NULL);
}

//Parse arguments
void ParseArgs(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        switch(argv[i][0]) {
            case '-': 
            {
                if(!strcmp(argv[i], "-t")) {
                    tokenize = true;
                    tokenFile = argv[++i];
                }
                if(!strcmp(argv[i], "-b")) {
                    tokenize = false;
                    charSetIndex = atoi(argv[++i]);
                }
                if(!strcmp(argv[i], "-s")) {
                    startIndex = atoi(argv[++i]);
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
            "\t-b <charset>\t\t| brute force\n"
            "\t-t <token file>\t\t| tokize\n"
            "\t-s <start index>\t| Index in the dataset to start\n"
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

    if(tokenize) 
        printf("Tokenized with file\n%s\n", tokenFile);
    else 
        printf("Brute force with char set:\n%s\n", charset[charSetIndex]);
    
    //load tables/lists
    if(tokenize)
        tokenLinkList = ReadTokens(tokenFile);

    int prefixCnt = 0;
    prefixes = ReadIntoPair("prefixes.bin", &prefixCnt);
    if(!prefixes) return -1;

    suffixTable = ReadIntoHashtable("suffixes.bin");
    if(!suffixTable) return -1;
    
    pthread_mutex_init(&log_mutex, NULL);

    //Create threads
    int i = 0;
    pthread_t threads[prefixCnt];
    for (i = 0; i < prefixCnt; i++)
    {
        int *arg = malloc(sizeof(int*));
        *arg = i;
        pthread_create(&threads[i], NULL, DoWork, arg);
    }
    printf("Created %d threads!\n", i);

    printf("--------------------------\n");

    //Sync
    for (i = 0; i < prefixCnt; i++)
    {
        pthread_join(threads[i], NULL);
    }

    map_free(suffixTable);
    free(prefixes);
    if(tokenize)
        FreeTokens(tokenLinkList);

    pthread_mutex_destroy(&log_mutex);
    
    return 0;
}
