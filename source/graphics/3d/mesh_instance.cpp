
#include <iostream>

#include "mesh_instance.h"

MeshInstance::MeshInstance(SharedMesh mesh)
    : mesh(mesh)
{
    std::cout << "Creating MeshInstance of " << mesh->name << std::endl;
}

MeshInstance::~MeshInstance()
{
    std::cout << "Ending MeshInstance of " << mesh->name << std::endl;
}
