#include "utils.h"

//Convert linked list to array
char ** LinkedListToArray(struct LinkedList *list) {
    int i = 0, j = 0;
    struct LinkedList *l;
    for(l = list; l->next != 0; l = l->next){
        i++;
    }
    char **arr = calloc(++i, sizeof(char*));
    for(l = list; l->next != 0; l = l->next){
        arr[j++] = l->string;
    }
    
    return arr;
}

//Read into hashmap
HashTable *ReadIntoHashtable(char *file, bool fnv32Mode) {
    printf("Reading table %s into RAM\n", file);
    
    FILE *fp = fopen(file, "rb");
    HashTable *map = NULL;
    
    if (fp) {
        uint32_t count = 0;
        
        fread(&count, sizeof(uint32_t), 1, fp);
        struct TblEntry *ents = calloc(count, sizeof(struct TblEntry));
        map = map_create(ents, count);
        for(uint32_t i = 0; i < count; i++){
            uint64_t hash = 0;
            uint8_t len = 0;
            fread(&hash, fnv32Mode ? sizeof(uint32_t) : sizeof(uint64_t), 1, fp);
            fread(&len, 1, 1, fp);
            char *str = NULL;
            if(len > 0) str = (char*)malloc(len+1);
            fread(str, len, 1, fp);
            map_put(map, hash, str);
        }
        
        fclose(fp);
        printf("Loaded %d entries\n", count);
    } else printf("Couldnt open file\n");
    
    return map;
}

//Read into array
HashPair *ReadIntoPair(char *file, bool fnv32Mode) {
    printf("Reading table %s into RAM\n", file);
    
    FILE *fp = fopen(file, "rb");
    HashPair *pair = NULL;
    
    if (fp) {
        uint32_t count = 0;
        
        fread(&count, sizeof(uint32_t), 1, fp);
        pair = calloc(count + 1, sizeof(HashPair));
        for(uint32_t i = 0; i < count; i++){
            uint64_t hash = 0;
            uint8_t len = 0;
            fread(&hash, fnv32Mode ? sizeof(uint32_t) : sizeof(uint64_t), 1, fp);
            fread(&len, 1, 1, fp);
            char *str = NULL;
            if(len > 0) str = (char*)malloc(len+1);
            fread(str, len, 1, fp);
            HashPair p;
            p.hash = hash;
            p.string = str;
            pair[i] = p;
        }
        
        fclose(fp);
        printf("Loaded %d entries\n", count);
    } else printf("Couldnt open file\n");
    
    return pair;
}

struct LinkedList *ReadTokens(char *file) {
    struct LinkedList *rootlist = calloc(1, sizeof(struct LinkedList));
    FILE *fp = fopen(file, "r");
    if(!fp) {
        printf("Couldnt open token file %s\n", file);
        return NULL;
    }
    char buf[MAX_STR_SIZE];
    struct LinkedList *newlist = rootlist;
    while(fgets(buf, MAX_STR_SIZE, fp)){
        int sz = strlen(buf);
        for(int j = 0; j < sz; j++)
            if(buf[j] == '\n' || buf[j] == '\r') buf[j] = 0;
        char *str = malloc(strlen(buf));
        strcpy(str, buf);
        newlist->string = str;
        newlist->next = calloc(1, sizeof(struct LinkedList));
        newlist = newlist->next;
    }
    return rootlist;
}