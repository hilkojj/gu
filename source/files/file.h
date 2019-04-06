#include <iostream>

#ifndef FILE_H
#define FILE_H

class File
{
  public:
    static std::string readString(const char *path);
};

#endif
