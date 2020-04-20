
#ifndef GAME_FILEREADER_H
#define GAME_FILEREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

#include "../utils/gu_error.h"
#include "file.h"

class FileReader
{
  protected:
    int pos = 0;
    std::vector<unsigned char> data;

  public:
//    std::ifstream stream;

    FileReader(const char *path)// : stream(path, std::ios::in | std::ios::binary)
        : data(File::readBinary(path))
    {}

    template<typename type>
    type read()
    {
        type out;
        if (!hasNMoreBytes(sizeof(type)))
            return out;
        memcpy(&out, &data[pos], sizeof(type));
        pos += sizeof(type);
        return out;
    }

    template<typename byte>
    void read(int n, std::vector<byte> &out)
    {
        if (!hasNMoreBytes(n))
            return;
        out.insert(out.end(), n, 0);
        char *copyTo = (char *) &out[out.size() - n];
        memcpy(copyTo, &data[pos], n);
        pos += n;
    }

    void skip(int n)
    {
        if (hasNMoreBytes(n))
            pos += n;
    }

    template<typename type>
    void skip()
    {
        skip(sizeof(type));
    }

    int currentPosition()
    {
        return pos;
    }

    bool hasNMoreBytes(int n)
    {
        return pos + n <= data.size();
    }

    bool endReached()
    {
        return hasNMoreBytes(1);
    }
};


#endif
