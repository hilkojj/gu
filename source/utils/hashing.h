
#ifndef GAME_HASHING_H
#define GAME_HASHING_H

template<typename T>
inline size_t hashValue(const T &v)
{
    return std::hash<T>{}(v);
}

inline uint32 hashStringCrossPlatform(const char *str)
{
    uint32 hash = 0x811c9dc5;
    uint32 prime = 0x1000193;
    int i = -1;
    while (str[++i] != '\0')
    {
        unsigned char c = str[i];

        hash = hash ^ c;
        hash *= prime;
    }
    return hash;
}

template <class type>
inline uint32 typeHashCrossPlatform()
{
    return hashStringCrossPlatform(typeid(type).name());
}


#endif //GAME_HASHING_H
