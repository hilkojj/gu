#pragma once

#include <iostream>
#include <string>

class _gu_err : public std::runtime_error
{
  public:
    _gu_err(const std::string &msg, const char *caller, const char *file, const unsigned int line)
    : std::runtime_error(msg)
    {
        #ifdef linux
        std::cerr << "\033[1m\033[31m";
        #endif

        std::cerr << "\n------ gu_err ------\n" <<  msg << "\n\n";

        #ifdef linux
        std::cerr << "\033[1m\033[35m";
        #endif

        std::cerr << "at " << caller << "\n\n";

        #ifdef linux
        std::cerr << "\033[1m\033[32m";
        #endif

        std::cerr << file << ":" << line << "\n" << std::endl;

        #ifdef linux
        std::cerr << "\033[0m";
        #endif
    }
};

#ifdef _WIN32
#define gu_err(msg) _gu_err(msg, __FUNCSIG__, __FILE__, __LINE__)
#else
#define gu_err(msg) _gu_err(msg, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#endif
