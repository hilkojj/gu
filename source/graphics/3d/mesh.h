
#ifndef MESH_H
#define MESH_H

#include <vector>

#include "vert_attributes.h"

class Mesh
{

  public:

    std::vector<float> vertices;
    std::vector<unsigned short> indices;

    unsigned int nrOfVertices, nrOfIndices;

    Mesh(unsigned int nrOfVertices, unsigned int nrOfIndices, const VertAttributes &attributes);

};

#endif
