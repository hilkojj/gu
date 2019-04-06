#include <iostream>
#include <fstream>
#include <sstream>

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