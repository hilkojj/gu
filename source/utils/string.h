
#ifndef GAME_STRING_H
#define GAME_STRING_H

#include <vector>
#include <string>

/**
 * taken from https://stackoverflow.com/a/37454181/10386780
 */
std::vector<std::string> splitString(const std::string& str, const std::string& delim)
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

#endif //GAME_STRING_H
