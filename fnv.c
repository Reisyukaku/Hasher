#include "fnv.h"

uint64_t FNVA1Hash(uint64_t basis, const char *str){
    uint64_t hash = basis;

    for (int i = 0; str[i] != 0; i++)
    {
        hash = hash ^ str[i];
        hash *= FNV64_PRIME;
    }

    return hash;
}

uint64_t FNVA1Hash_reverse(uint64_t basis, const char *str){
    int len = strlen(str);
    const uint64_t fnv64Prime = FNV64_INV_PRIME;
    uint64_t hash = basis;

    for (int i = 0; i < len; i++)
    {
        hash *= fnv64Prime;
        hash = hash ^ str[len - i - 1];
        
    }

    return hash;
}

uint32_t FNVA1Hash_32(uint32_t basis, const char *str){
    int len = strlen(str);
    const uint32_t fnv32Offset = basis;
    const uint32_t fnv32Prime = FNV32_PRIME;
    uint32_t hash = fnv32Offset;

    for (int i = 0; i < len; i++)
    {
        hash = hash ^ str[i];
        hash *= fnv32Prime;
    }

    return hash;
}

uint32_t FNVA1Hash_32_reverse(uint32_t basis, const char *str){
    int len = strlen(str);
    const uint32_t fnv32Prime = FNV32_INV_PRIME;
    uint32_t hash = basis;

    for (int i = 0; i < len; i++)
    {
        hash *= fnv32Prime;
        hash = hash ^ str[len - i - 1];
        
    }

    return hash;
}

uint64_t FNVA1Hash_rec(uint64_t basis, char *str){
    if(*str != 0) {
        basis = basis ^ str[0];
        basis *= FNV64_PRIME;
        FNVA1Hash_rec(basis, &str[1]);
    }
    return basis;
}
