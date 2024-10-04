#include <iostream>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>

#ifndef FILE_H
#define FILE_H

namespace fu
{

std::string readString(const char *path);

std::vector<unsigned char> readBinary(const char *path);

bool exists(const char *path);

void createDirectory(const char *path);

template<typename data_vec>
void writeBinary(const char *path, const data_vec &data)
{
    std::ofstream out(path, std::ios::out | std::ios::binary);
    out.write((const char *) &data[0], data.size());
    out.close();
}

void iterateDirectoryRecursively(
    const char *directoryPath,
    const std::function<void(const std::string &path, bool bDirectory)> &entryCallback
);

};

#endif
