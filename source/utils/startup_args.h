#pragma once

#include "string_utils.h"

#include <map>

inline void startupArgsToMap(int argCount, char *argValues[], std::map<std::string, std::string> &map)
{
    for (int i = 1; i < argCount; i++)
    {
        auto key = argValues[i];
        if (i + 1 < argCount && argValues[i + 1][0] != '-')
        {
            map[key] = argValues[++i];
        }
        else
        {
            map[key] = "";
        }
    }
}
