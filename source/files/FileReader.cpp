
#include "FileReader.h"

#include "file_utils.h"

#include <cstring>

FileReader::FileReader(const char *path) :
    data(fu::readBinary(path))
{}

void FileReader::copy(const int n, char *out)
{
    memcpy(out, &data[readPos], n);
}

void FileReader::skip(const int n)
{
    if (hasNMoreBytes(n))
    {
        readPos += n;
    }
}

int FileReader::currentReadPosition() const
{
    return readPos;
}

bool FileReader::hasNMoreBytes(const int n) const
{
    return readPos + n <= data.size();
}

bool FileReader::reachedEnd() const
{
    return !hasNMoreBytes(1);
}
