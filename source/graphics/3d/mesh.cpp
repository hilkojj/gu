
#include <iostream>
#include <string>

#include "mesh.h"
#include "vert_buffer.h"    
#include "vert_attributes.h"

Mesh::Mesh(std::string name, unsigned int nrOfVertices, unsigned int nrOfIndices, VertAttributes attributes)

    : name(name), nrOfVertices(nrOfVertices), nrOfIndices(nrOfIndices),

      vertices(nrOfVertices * attributes.getVertSize()),
      indices(nrOfIndices),

      attributes(attributes)
{
    std::cout << "Mesh created: " << name << std::endl;
}

void Mesh::disposeOfflineData()
{
    vertices.resize(0);
    vertices.shrink_to_fit();
    indices.resize(0);
    indices.shrink_to_fit();
}

void Mesh::render()
{
    if (!vertBuffer || !vertBuffer->isUploaded()) throw std::runtime_error(name + " is not uploaded. Upload it first with a VertBuffer");
    vertBuffer->bind();
    glDrawElementsBaseVertex(
        GL_TRIANGLES,
        nrOfIndices,
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t)indicesBufferOffset,
        baseVertex
    );
}

Mesh::~Mesh()
{
    std::cout << "Mesh destroyed: " << name << std::endl;

    if (vertBuffer)
        vertBuffer->onMeshDestroyed();
}
