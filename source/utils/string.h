
#ifndef GAME_STRING_H
#define GAME_STRING_H

#include <vector>
#include <string>
#include <algorithm>

/**
 * taken from https://stackoverflow.com/a/37454181/10386780
 */
inline std::vector<std::string> splitString(const std::string& str, const std::string& delim)
{
    using namespace std;

    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

/**
 * taken from https://stackoverflow.com/a/874160/10386780
 */
inline bool stringEndsWith(std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    else
        return false;
}
inline bool stringStartsWith(std::string const &fullString, std::string const &start)
{
    if (fullString.length() >= start.length())
        return (0 == fullString.compare(0, start.length(), start));
    else
        return false;
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
