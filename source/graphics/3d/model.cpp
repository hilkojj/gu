
#include "model.h"

Model::Model(std::string name) : name(name)
{
    std::cout << "Model created: " << name << std::endl;
}

Model::~Model()
{
    std::cout << "Model destroyed: " << name << std::endl;
}
