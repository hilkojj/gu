
#include <iostream>

#include "mesh.h"
#include "vert_attributes.h"

Mesh::Mesh(std::string name, unsigned int nrOfVertices, unsigned int nrOfIndices, const VertAttributes &attributes)

    : name(name), nrOfVertices(nrOfVertices), nrOfIndices(nrOfIndices),

      vertices(nrOfVertices * attributes.getVertSize()),
      indices(nrOfIndices)
{
    std::cout << "Mesh created: " << name << std::endl;
}

Mesh::~Mesh()
{
    std::cout << "Mesh destroyed: " << name << std::endl;
}
