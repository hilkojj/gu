#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "file.h"

std::string File::readString(const char *path)
{
    std::ifstream stream(path, std::ios::in);

    if (!stream.is_open())
        throw std::runtime_error("Could not open: " + std::string(path));

    std::stringstream sstr;
    sstr << stream.rdbuf();
    stream.close();
    return sstr.str();
}

std::vector<u_char> File::readBinary(const char *path)
{
    std::ifstream stream(path, std::ios::binary);

    if (!stream.is_open())
        throw std::runtime_error("Could not open: " + std::string(path));

    std::vector<u_char> buffer(std::istreambuf_iterator<char>(stream), {});
    return buffer;
}
