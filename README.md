# Hasher

Reads `prefixes.bin` and `suffixes.bin` into RAM and takes the input characterset or token list and bruteforces the in-between. Hashmap impl uses FNV1A64 directly for O(1) checks against suffixes.
