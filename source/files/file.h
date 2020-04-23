#include <iostream>
#include <vector>
#include <functional>

#ifndef FILE_H
#define FILE_H

class File
{
  public:
    static std::string readString(const char *path);
    static std::vector<unsigned char> readBinary(const char *path);
    static bool exists(const char *path);

    static void writeBinary(const char *path, std::vector<unsigned char> &data);

    static void iterateFilesRecursively(const std::string &path, std::function<void(const std::string &)> cb);

};

#endif
