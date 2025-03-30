#pragma once
#include <string>
//taken from https://www.partow.net/programming/hashfunctions/index.html#StringHashing

consteval unsigned int APHash(const char* str, int length)
{
    unsigned int hash = 0xAAAAAAAA;
    unsigned int i = 0;

    for(int i = 0; i < length; i++)
    {
        hash ^= ((i & 1) == 0) ? ((hash << 7) ^ (*str) * (hash >> 3)) :
            (~((hash << 11) + ((*str) ^ (hash >> 5))));
    }

    return hash;
}