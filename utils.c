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
HashTable *ReadIntoHashtable(char *file) {
    printf("Reading table %s into RAM\n", file);
    
    FILE *fp = fopen(file, "rb");
    HashTable *map = NULL;
    
    if (fp) {
        uint32_t count = 0;
        
        fread(&count, sizeof(uint32_t), 1, fp);
        struct TblEntry *ents = (struct TblEntry*) calloc(count, sizeof(struct TblEntry));
        map = map_create(ents, count);
        for(uint32_t i = 0; i < count; i++){
            uint64_t hash = 0;
            uint8_t len = 0;
            fread(&hash, sizeof(uint64_t), 1, fp);
            fread(&len, 1, 1, fp);
            char *str = "";
            if(len > 0) 
            {
                str = (char*)malloc(len+1);
                fread(str, len, 1, fp);
            }
            map_put(map, hash, str);
        }
        
        fclose(fp);
        printf("Loaded %d entries\n", count);
    } else {
        printf("Couldnt open file\n");
        map = NULL;
    }
    
    return map;
}

//Read into array
HashPair *ReadIntoPair(char *file, int *cnt) {
    printf("Reading table %s into RAM\n", file);
    
    FILE *fp = fopen(file, "rb");
    HashPair *pair = NULL;
    
    if (fp) {        
        fread(cnt, sizeof(uint32_t), 1, fp);
        pair = calloc(*cnt + 1, sizeof(HashPair));
        for(int i = 0; i < *cnt; i++){
            uint64_t hash = 0;
            uint8_t len = 0;
            fread(&hash, sizeof(uint64_t), 1, fp);
            fread(&len, 1, 1, fp);
            char *str = "";
            if(len > 0) 
            {
                str = (char*)malloc(len+1);
                fread(str, len, 1, fp);
            }
            HashPair p;
            p.hash = hash;
            p.string = str;
            pair[i] = p;
        }
        
        fclose(fp);
        printf("Loaded %d entries\n", *cnt);
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
    int i = 0;
    char buf[MAX_STR_SIZE] = {};
    struct LinkedList *newList = rootlist;
    while(fgets(buf, MAX_STR_SIZE, fp)){
        int sz = strlen(buf);
        for(int j = 0; j < sz; j++)
            if(buf[j] == '\n' || buf[j] == '\r') buf[j] = 0;
        char *str = malloc(strlen(buf));
        strcpy(str, buf);
        newList->string = str;
        newList->next = calloc(1, sizeof(struct LinkedList));
        newList = newList->next;
    }
    printf("Loaded %d tokens\n", i);
    return rootlist;
}

void FreeTokens(struct LinkedList *list) {
    struct LinkedList *next;
    do{
        if(list->string)
            free(list->string);
        next = list->next;
        free(list);
        list = next;
    } while(next != NULL);
}
