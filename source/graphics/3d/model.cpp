#include "model.h"

#ifndef GU_PUT_A_SOCK_IN_IT
#include <iostream>
#endif

Model::Model(std::string name) : name(name)
{
    #ifndef GU_PUT_A_SOCK_IN_IT
    std::cout << "Model created: " << name << std::endl;
    #endif
}

Model::~Model()
{
    #ifndef GU_PUT_A_SOCK_IN_IT
    std::cout << "Model destroyed: " << name << std::endl;
    #endif
}
