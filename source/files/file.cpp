#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "file.h"
#include "../utils/gu_error.h"

std::string File::readString(const char *path)
{
    std::cout << "read file\n";
    std::ifstream stream(path, std::ios::in);

    if (!stream.is_open())
        throw gu_err("Could not open: " + std::string(path));

    std::stringstream sstr;
    sstr << stream.rdbuf();
    stream.close();
    std::cout << "read done\n";
    return sstr.str();
}

std::vector<unsigned char> File::readBinary(const char *path)
{
    std::ifstream stream(path, std::ios::binary);

    if (!stream.is_open())
        throw std::runtime_error("Could not open: " + std::string(path));
        
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(stream), {});
}

void File::writeBinary(const char *path, std::vector<unsigned char> data)
{
    std::ofstream out(path, std::ios::out | std::ios::binary);
    out.write((const char *)&data[0], data.size());

    out.close();
}
