
#ifndef GAME_STARTUP_ARGS_H
#define GAME_STARTUP_ARGS_H

#include <map>
#include "./string_utils.h"

inline void startupArgsToMap(int argCount, char *argValues[], std::map<std::string, std::string> &map)
{
    for (int i = 1; i < argCount; i++)
    {
        auto key = argValues[i];
        if (i + 1 < argCount && argValues[i + 1][0] != '-')
            map[key] = argValues[++i];
        else
            map[key] = "";
    }
}

#endif //GAME_STARTUP_ARGS_H
