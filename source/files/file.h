#include <iostream>
#include <vector>

#ifndef FILE_H
#define FILE_H

class File
{
  public:
    static std::string readString(const char *path);
    static std::vector<u_char> readBinary(const char *path);
};

#endif
