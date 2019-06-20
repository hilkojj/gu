
#include <iostream>
#include <string>

#include "mesh.h"
#include "vert_buffer.h"    
#include "vert_attributes.h"
#include "utils/gu_error.h"

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

Mesh::~Mesh()
{
    std::cout << "Mesh destroyed: " << name << std::endl;

    if (vertBuffer)
        vertBuffer->onMeshDestroyed();
}

vec4 Mesh::getVec4(int vertI, int attrOffset)
{
    return vec4(
        vertices[vertI * attributes.getVertSize() + attrOffset],
        vertices[vertI * attributes.getVertSize() + attrOffset + 1],
        vertices[vertI * attributes.getVertSize() + attrOffset + 2],
        vertices[vertI * attributes.getVertSize() + attrOffset + 3]
    );
}

vec3 Mesh::getVec3(int vertI, int attrOffset)
{
    return vec3(
        vertices[vertI * attributes.getVertSize() + attrOffset],
        vertices[vertI * attributes.getVertSize() + attrOffset + 1],
        vertices[vertI * attributes.getVertSize() + attrOffset + 2]
    );
}

vec2 Mesh::getVec2(int vertI, int attrOffset)
{
    return vec2(
        vertices[vertI * attributes.getVertSize() + attrOffset],
        vertices[vertI * attributes.getVertSize() + attrOffset + 1]
    );
}

float Mesh::getFloat(int vertI, int attrOffset)
{
    return vertices[vertI * attributes.getVertSize() + attrOffset];
}

void Mesh::setVec4(const vec4 &v, int vertI, int attrOffset)
{
    vertices[vertI * attributes.getVertSize() + attrOffset] = v.x;
    vertices[vertI * attributes.getVertSize() + attrOffset + 1] = v.y;
    vertices[vertI * attributes.getVertSize() + attrOffset + 2] = v.z;
    vertices[vertI * attributes.getVertSize() + attrOffset + 3] = v.w;
}


void Mesh::setVec3(const vec3 &v, int vertI, int attrOffset)
{
    vertices[vertI * attributes.getVertSize() + attrOffset] = v.x;
    vertices[vertI * attributes.getVertSize() + attrOffset + 1] = v.y;
    vertices[vertI * attributes.getVertSize() + attrOffset + 2] = v.z;
}

void Mesh::setVec2(const vec2 &v, int vertI, int attrOffset)
{
    vertices[vertI * attributes.getVertSize() + attrOffset] = v.x;
    vertices[vertI * attributes.getVertSize() + attrOffset + 1] = v.y;
}

void Mesh::setFloat(float v, int vertI, int attrOffset)
{
    vertices[vertI * attributes.getVertSize() + attrOffset] = v;
}

void Mesh::addVec4(const vec4 &v, int vertI, int attrOffset)
{
    vertices[vertI * attributes.getVertSize() + attrOffset] += v.x;
    vertices[vertI * attributes.getVertSize() + attrOffset + 1] += v.y;
    vertices[vertI * attributes.getVertSize() + attrOffset + 2] += v.z;
    vertices[vertI * attributes.getVertSize() + attrOffset + 3] += v.w;
}

void Mesh::addVec3(const vec3 &v, int vertI, int attrOffset)
{
    vertices[vertI * attributes.getVertSize() + attrOffset] += v.x;
    vertices[vertI * attributes.getVertSize() + attrOffset + 1] += v.y;
    vertices[vertI * attributes.getVertSize() + attrOffset + 2] += v.z;
}

void Mesh::addVec2(const vec2 &v, int vertI, int attrOffset)
{
    vertices[vertI * attributes.getVertSize() + attrOffset] += v.x;
    vertices[vertI * attributes.getVertSize() + attrOffset + 1] += v.y;
}

void Mesh::normalizeVec3Attribute(int attrOffset)
{
    for (int vertI = 0; vertI < nrOfVertices; vertI++)
        setVec3(normalize(getVec3(vertI, attrOffset)), vertI, attrOffset);
}

void Mesh::normalizeVec2Attribute(int attrOffset)
{
    for (int vertI = 0; vertI < nrOfVertices; vertI++)
        setVec2(normalize(getVec2(vertI, attrOffset)), vertI, attrOffset);
}
