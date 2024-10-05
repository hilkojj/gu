
#ifndef GAME_HASHING_H
#define GAME_HASHING_H

#include "../math/math_utils.h"

template<typename T>
inline size_t hashValue(const T &v)
{
    return std::hash<T>{}(v);
}

template <typename string>
inline uint32 hashStringCrossPlatform(const string &inputString)
{
    uint32 hash = 0x811c9dc5;
    uint32 prime = 0x1000193;
    int i = -1;
    while (inputString[++i] != '\0')
    {
        hash = hash ^ inputString[i];
        hash *= prime;
    }
    return hash;
}

#endif //GAME_HASHING_H
