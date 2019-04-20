
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include <iostream>

#include "vert_attributes.h"

class VertBuffer;

class Mesh
{

  public:
    std::string name;
    std::vector<float> vertices;
    std::vector<unsigned short> indices;

    unsigned int nrOfVertices, nrOfIndices;

    VertAttributes attributes;

    VertBuffer *vertBuffer = nullptr;

    // variables used for glDrawElementsBaseVertex: (https://www.khronos.org/opengl/wiki/GLAPI/glDrawElementsBaseVertex)
    int baseVertex = 0, indicesBufferOffset = 0;

    Mesh(
        std::string name,
        unsigned int nrOfVertices,
        unsigned int nrOfIndices,
        VertAttributes attributes);

    // removes the vertices + indices that are stored in RAM,
    // but the mesh can still be drawn if it is uploaded to VRAM/OpenGL using a VertBuffer
    // WARNING: vertices & indices are resized to 0!
    void disposeOfflineData();

    void render();

    ~Mesh();
};

typedef std::shared_ptr<Mesh> SharedMesh;

#endif
