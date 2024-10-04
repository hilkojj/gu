#include "model.h"

#include <string>

Model::Model(std::string name) : name(name)
{
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "Model created: " << name << std::endl;
    #endif
}

Model::~Model()
{
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "Model destroyed: " << name << std::endl;
    #endif
}
