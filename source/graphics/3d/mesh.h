
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include <iostream>

#include "vert_attributes.h"

class Mesh
{

  public:
    std::string name;
    std::vector<float> vertices;
    std::vector<unsigned short> indices;

    unsigned int nrOfVertices, nrOfIndices;

    Mesh(
        std::string name,
        unsigned int nrOfVertices,
        unsigned int nrOfIndices,
        const VertAttributes &attributes);

    ~Mesh();
};

typedef std::shared_ptr<Mesh> SharedMesh;

#endif
