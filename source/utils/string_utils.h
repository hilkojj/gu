
#ifndef GAME_STRING_H
#define GAME_STRING_H

#include <vector>
#include <string>
#include <algorithm>

namespace su
{

inline std::vector<std::string> split(const std::string &string, const std::string &delimiter, bool bKeepEmptyParts = false)
{
    std::vector<std::string> parts;
    size_t firstDelimIndex = 0;
    size_t secondDelimIndex = 0;
    while (firstDelimIndex < string.length() && secondDelimIndex < string.length())
    {
        secondDelimIndex = string.find(delimiter, firstDelimIndex);
        if (secondDelimIndex == std::string::npos)
        {
            secondDelimIndex = string.length();
        }
        const size_t newPartLength = secondDelimIndex - firstDelimIndex;
        const std::string newPart = string.substr(firstDelimIndex, newPartLength);
        if (bKeepEmptyParts || !newPart.empty())
        {
            parts.push_back(newPart);
        }
        firstDelimIndex = delimiter.length() + secondDelimIndex;
    }
    return parts;
}

inline bool endsWith(const std::string &fullString, const std::string &ending)
{
    if (fullString.length() < ending.length())
    {
        return false;
    }
    return fullString.compare(fullString.length() - ending.length(), ending.length(), ending) == 0;
}

inline bool startsWith(const std::string &fullString, const std::string &start)
{
    if (fullString.length() < start.length())
    {
        return false;
    }
    return fullString.compare(0, start.length(), start) == 0;
}

inline int getNumNewlines(const std::string &string)
{
    int count = 0;
    for (auto &c : string)
    {
        if (c == '\n')
        {
            count++;
        }
    }
    return count;
}

inline char randomAlphanumericChar()
{
    constexpr char chars[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    constexpr int maxIndex = sizeof(chars) - 1;
    return chars[rand() % maxIndex];
}

inline std::string randomAlphanumeric(int length)
{
    std::string string(length, 0);
    std::generate_n(string.begin(), length, randomAlphanumericChar);
    return string;
}

}

#endif //GAME_STRING_H
