#include <iostream>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>


#ifdef _WIN32
#include <filesystem>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif


#ifndef FILE_H
#define FILE_H

class File
{
  public:
    static std::string readString(const char *path);
    static std::vector<unsigned char> readBinary(const char *path);
    static bool exists(const char *path);

    static void createDir(const char *path);

    template<typename data_vec>
    static void writeBinary(const char *path, const data_vec &data)
    {
        std::ofstream out(path, std::ios::out | std::ios::binary);
        out.write((const char *)&data[0], data.size());

        out.close();
    }

    static void iterateDirectoryRecursively(const std::string &path, std::function<void(const std::string &, bool isDir)> cb);

};

#endif
