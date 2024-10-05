
#ifndef FILE_H
#define FILE_H

#include <vector>
#include <functional>
#include <string>

namespace fu
{

std::string readString(const char *path);

std::vector<unsigned char> readBinary(const char *path);

bool exists(const char *path);

void createDirectory(const char *path);

void writeBinary(const char *path, const char *data, size_t dataSize);

void iterateDirectoryRecursively(
    const char *directoryPath,
    const std::function<void(const std::string &path, bool bDirectory)> &entryCallback
);

};

#endif
