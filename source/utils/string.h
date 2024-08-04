
#ifndef GAME_STRING_H
#define GAME_STRING_H

#include <vector>
#include <string>
#include <algorithm>

inline std::vector<std::string> splitString(const std::string &string, const std::string &delimiter, bool bKeepEmptyParts = false)
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

inline bool stringEndsWith(const std::string &fullString, const std::string &ending)
{
    if (fullString.length() < ending.length())
    {
        return false;
    }
    return fullString.compare(fullString.length() - ending.length(), ending.length(), ending) == 0;
}

inline bool stringStartsWith(const std::string &fullString, const std::string &start)
{
    if (fullString.length() < start.length())
    {
        return false;
    }
    return fullString.compare(0, start.length(), start) == 0;
}

inline int nrOfNewlines(const std::string &str)
{
    int count = 0;
    for (auto &c : str)
        if (c == '\n')
            count++;
    return count;
}

inline std::string randomString(int length)
{
    auto getRandomChar = [] () -> char
    {
        constexpr char chars[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        constexpr int maxIndex = sizeof(chars) - 1;
        return chars[rand() % maxIndex];
    };
    std::string str(length,0);
    std::generate_n(str.begin(), length, getRandomChar);
    return str;
}

#endif //GAME_STRING_H
