
#include "file_utils.h"

#include "../utils/gu_error.h"
#include "../utils/string_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <filesystem>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif


std::string fu::readString(const char *path)
{
    std::ifstream stream(path, std::ios::in);

    if (!stream.is_open())
    {
        throw gu_err("Could not open: " + std::string(path));
    }

    std::stringstream sstr;
    sstr << stream.rdbuf();
    stream.close();
    return sstr.str();
}

std::vector<unsigned char> fu::readBinary(const char *path)
{
    std::ifstream stream(path, std::ios::binary);

    if (!stream.is_open())
    {
        throw gu_err("Could not open: " + std::string(path));
    }

    return std::vector<unsigned char>(std::istreambuf_iterator<char>(stream), {});
}

bool fu::exists(const char *path)
{
    std::ifstream stream(path, std::ios::in);
    bool exists = stream.is_open();
    stream.close();
    return exists;
}

void fu::writeBinary(const char *path, const char *data, size_t dataSize)
{
    std::ofstream out(path, std::ios::out | std::ios::binary);
    out.write(data, size_t(dataSize));
    out.close();
}

void fu::iterateDirectoryRecursively(
    const char *directoryPath,
    const std::function<void(const std::string &, bool)> &entryCallback
)
{
    std::string path = directoryPath;
    if (!su::endsWith(path, "/"))
    {
        path += "/";
    }

    #ifndef _WIN32
    if (auto openedDir = opendir(path.c_str()))
    {
        while (auto entry = readdir(openedDir))
        {
            if (entry->d_name[0] == '.')
            {
                continue;
            }
            const bool bEntryIsDirectory = entry->d_type == DT_DIR;
            if (bEntryIsDirectory)
            {
                iterateDirectoryRecursively((path + entry->d_name).c_str(), entryCallback);
            }

            entryCallback(path + entry->d_name, bEntryIsDirectory);
        }
        closedir(openedDir);
    }
    #else
    for (const auto &entry : std::filesystem::recursive_directory_iterator(path))
    {
        entryCallback(entry.path().generic_string(), entry.is_directory());
    }
    #endif
}

void fu::createDirectory(const char *path)
{
    #ifdef _WIN32
    std::filesystem::create_directory(path);
    #else
    mkdir(path, 0777);
    #endif
}
