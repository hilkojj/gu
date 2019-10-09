#ifndef GU_ERROR_H
#define GU_ERROR_H

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
        
        std::cerr << file << ":" << line << "\n\n";

        #ifdef linux
        std::cerr << "\033[0m";
        #endif
    }
};

#ifdef linux
#define gu_err(msg) _gu_err(msg, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#else
#define gu_err(msg) _gu_err(msg, __FUNCSIG__, __FILE__, __LINE__)
#endif



#endif
