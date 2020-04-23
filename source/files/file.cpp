#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#ifndef EMSCRIPTEN
#include <filesystem>
#else
#include <dirent.h>
#endif

#include "file.h"
#include "../utils/gu_error.h"
#include "../utils/string.h"


std::string File::readString(const char *path)
{
    std::ifstream stream(path, std::ios::in);

    if (!stream.is_open())
        throw gu_err("Could not open: " + std::string(path));

    std::stringstream sstr;
    sstr << stream.rdbuf();
    stream.close();
    return sstr.str();
}

std::vector<unsigned char> File::readBinary(const char *path)
{
    std::ifstream stream(path, std::ios::binary);

    if (!stream.is_open())
        throw gu_err("Could not open: " + std::string(path));
        
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(stream), {});
}

void File::writeBinary(const char *path, std::vector<unsigned char> &data)
{
    std::ofstream out(path, std::ios::out | std::ios::binary);
    out.write((const char *)&data[0], data.size());

    out.close();
}

bool File::exists(const char *path)
{
    std::ifstream stream(path, std::ios::in);
    bool exists = stream.is_open();
    stream.close();
    return exists;
}

void File::iterateFilesRecursively(const std::string &path_, std::function<void(const std::string &)> cb)
{
    std::string path = path_;
    if (!stringEndsWith(path, "/"))
        path += "/";

    #ifdef EMSCRIPTEN
    if (auto dir = opendir(path.c_str())) {
        while (auto f = readdir(dir)) {
            if (f->d_name[0] == '.') continue;
            if (f->d_type == DT_DIR)
                iterateFilesRecursively(path + f->d_name, cb);

            if (f->d_type == DT_REG)
                cb(path + f->d_name);
        }
        closedir(dir);
    }
    #else
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(path))
    {
        if (dirEntry.is_directory())
            continue;
        cb(dirEntry.path().generic_string());
    }
    #endif
}
