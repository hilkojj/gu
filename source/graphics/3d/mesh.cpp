
#include <iostream>
#include <string>

#include "mesh.h"
#include "vert_buffer.h"    
#include "vert_attributes.h"
#include "../../utils/gu_error.h"

SharedMesh Mesh::quad;

SharedMesh Mesh::getQuad()
{
    if (!quad)
    {
        quad = SharedMesh(new Mesh("quad", 4, 6, VertAttributes().add_(VertAttributes::POSITION)));
        quad->vertices.insert(quad->vertices.begin(), {
            -1, -1, 0,
            -1, 1, 0,
            1, 1, 0,
            1, -1, 0,
        });
        quad->indices.insert(quad->indices.begin(), {
            2, 1, 0,
            0, 3, 2,
        });
        VertBuffer::uploadSingleMesh(quad);
    }
    return quad;
}

VertData::VertData(VertAttributes attrs, std::vector<float> vertices)
    : attributes(attrs), vertices(vertices)
{}

Mesh::Mesh(std::string name, unsigned int nrOfVertices, unsigned int nrOfIndices, VertAttributes attributes)

    : name(name), nrOfVertices(nrOfVertices), nrOfIndices(nrOfIndices),

      VertData(attributes, std::vector<float>(nrOfVertices * attributes.getVertSize())),
      indices(nrOfIndices)
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
    if (!vertBuffer || !vertBuffer->isUploaded()) throw gu_err(name + " is not uploaded. Upload it first with a VertBuffer");
    vertBuffer->bind();
    #ifdef EMSCRIPTEN
    glDrawElements(
        mode,
        nrOfIndices,
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t)indicesBufferOffset
    );
    #else
    glDrawElementsBaseVertex(
        mode,
        nrOfIndices,
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t)indicesBufferOffset,
        baseVertex
    );
    #endif
}

void Mesh::renderInstances(GLsizei count)
{
    if (!vertBuffer || !vertBuffer->isUploaded()) throw gu_err(name + " is not uploaded. Upload it first with a VertBuffer");
    vertBuffer->bind();
    #ifdef EMSCRIPTEN
    EM_ASM({
        gl.drawElementsInstanced($0, $1, $2, $3, $4);
    },
    // glDrawElementsInstanced(
        mode,
        nrOfIndices,
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t)indicesBufferOffset,
        count
    // );
    );
    #else
    glDrawElementsInstancedBaseVertex(
        mode,
        nrOfIndices,
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t)indicesBufferOffset,
        count,
        baseVertex
    );
    #endif
}

Mesh::~Mesh()
{
    std::cout << "Mesh destroyed: " << name << std::endl;

    if (vertBuffer)
        vertBuffer->onMeshDestroyed();
}

float VertData::getFloat(int vertI, int attrOffset)
{
    return vertices[vertI * attributes.getVertSize() + attrOffset];
}

void VertData::setFloat(float v, int vertI, int attrOffset)
{
    vertices[vertI * attributes.getVertSize() + attrOffset] = v;
}
