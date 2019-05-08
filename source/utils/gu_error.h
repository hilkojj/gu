#ifndef GU_ERROR_H
#define GU_ERROR_H

#include <iostream>
#include <string>

class gu_err : public std::runtime_error
{
  public:
    gu_err(const std::string &msg)
    : std::runtime_error(msg)
    {
        std::cerr << "gu_err: " <<  msg << std::endl;
    }
};

#endif
