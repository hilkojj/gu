
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

bool File::exists(const char *path)
{
    std::ifstream stream(path, std::ios::in);
    bool exists = stream.is_open();
    stream.close();
    return exists;
}

void File::iterateDirectoryRecursively(const std::string &path_, std::function<void(const std::string &, bool)> cb)
{
    std::string path = path_;
    if (!stringEndsWith(path, "/"))
        path += "/";

    #ifndef _WIN32
    if (auto dir = opendir(path.c_str())) {
        while (auto f = readdir(dir)) {
            if (f->d_name[0] == '.') continue;
            bool isDir = f->d_type == DT_DIR;
            if (isDir)
                iterateDirectoryRecursively(path + f->d_name, cb);

            cb(path + f->d_name, isDir);
        }
        closedir(dir);
    }
    #else
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(path))
    {
        cb(dirEntry.path().generic_string(), dirEntry.is_directory());
    }
    #endif
}

void File::createDir(const char *path)
{
    #ifdef _WIN32
    std::filesystem::create_directory(path);
    #else
    mkdir(path, 0777);
    #endif
}
