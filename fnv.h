#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define FNV64_BASIS 0xCBF29CE484222645
#define FNV32_BASIS 0x811C9DC5

#define FNV64_PRIME 0x00000100000001b3
#define FNV64_INV_PRIME 0xCE965057AFF6957B

#define FNV32_PRIME 0x01000193
#define FNV32_INV_PRIME 0x359C449B

uint64_t FNVA1Hash(uint64_t basis, const char *str);
uint64_t FNVA1Hash_reverse(uint64_t basis, const char *str);

uint32_t FNVA1Hash_32(uint32_t basis, const char *str);
uint32_t FNVA1Hash_32_reverse(uint32_t basis, const char *str);

uint64_t FNVA1Hash_rec(uint64_t basis, char *str);