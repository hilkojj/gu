#include "mesh.h"
#include "vert_buffer.h"    
#include "vert_attributes.h"
#include "../../utils/gu_error.h"

#include <utility>
#include <iostream>
#include <string>

SharedMesh Mesh::getQuad()
{
    static SharedMesh quad;
    if (quad == nullptr)
    {
        quad = createQuad();
        VertBuffer::uploadSingleMesh(quad);
    }
    return quad;
}

VertData::VertData(VertAttributes attrs, std::vector<u_char> vertices) :
    attributes(std::move(attrs)),
    vertices(std::move(vertices))
{}

Mesh::Mesh(const std::string &name, unsigned int nrOfVertices, VertAttributes attributes) :
    name(name),
    VertData(attributes, std::vector<u_char>(nrOfVertices * attributes.getVertSize()))
{
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "Mesh created: " << name << std::endl;
    #endif
}

void Mesh::disposeOfflineData()
{
    vertices.resize(0);
    vertices.shrink_to_fit();

    for (Part &part : parts)
    {
        part.indices.resize(0);
        part.indices.shrink_to_fit();
    }
}

void Mesh::render(int partI)
{
    if (!vertBuffer || !vertBuffer->isUploaded())
    {
        throw gu_err(name + " is not uploaded. Upload it first with a VertBuffer");
    }
    vertBuffer->bind();
    if (parts.size() <= partI)
    {
        throw gu_err(name + " only has " + std::to_string(parts.size()) + " part(s). Requested: " + std::to_string(partI));
    }
    const Part &part = parts.at(partI);

    #ifdef EMSCRIPTEN
    glDrawElements(
        part.mode,
        part.getNrOfIndicesToRender(),
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t) part.indicesBufferOffset
    );
    #else
    glDrawElementsBaseVertex(
        part.mode,
        part.getNrOfIndicesToRender(),
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t) part.indicesBufferOffset,
        baseVertex
    );
    #endif
}

void Mesh::renderInstances(GLsizei count, int partI)
{
    if (!vertBuffer || !vertBuffer->isUploaded())
    {
        throw gu_err(name + " is not uploaded. Upload it first with a VertBuffer");
    }
    vertBuffer->bind();
    if (parts.size() <= partI)
    {
        throw gu_err(name + " only has " + std::to_string(parts.size()) + " part(s). Requested: " + std::to_string(partI));
    }
    const Part &part = parts.at(partI);

    #ifdef EMSCRIPTEN
    EM_ASM({
        gl.drawElementsInstanced($0, $1, $2, $3, $4);
    },
    // glDrawElementsInstanced(
        part.mode,
        part.getNrOfIndicesToRender(),
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t) part.indicesBufferOffset,
        count
    // );
    );
    #else
    glDrawElementsInstancedBaseVertex(
        part.mode,
        part.getNrOfIndicesToRender(),
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t) part.indicesBufferOffset,
        count,
        baseVertex
    );
    #endif
}

Mesh::~Mesh()
{
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "Mesh destroyed: " << name << std::endl;
    #endif

    if (vertBuffer)
    {
        vertBuffer->onMeshDestroyed();
    }
}

void Mesh::renderArrays(GLenum mode, int nrOfVerts) const
{
    if (!vertBuffer || !vertBuffer->isUploaded())
    {
        throw gu_err(name + " is not uploaded. Upload it first with a VertBuffer");
    }
    vertBuffer->bind();
    glDrawArrays(
        mode,
        baseVertex,
        nrOfVerts == -1 ? nrOfVertsReservedInVertBuffer : nrOfVerts
    );
}

SharedMesh Mesh::createQuad(float min, float max)
{
    SharedMesh quad(new Mesh("quad", 4, VertAttributes().add_(VertAttributes::POSITION)));
    quad->set<float[12]>({
        min, min, 0,
        min, max, 0,
        max, max, 0,
        max, min, 0,
    }, 0, 0);
    auto &part = quad->parts.emplace_back();
    part.indices.insert(part.indices.begin(), {
        2, 1, 0,
        0, 3, 2,
    });
    return quad;
}

SharedMesh Mesh::createCube(float min, float max)
{
    SharedMesh cube(new Mesh("cube", 8, VertAttributes().add_(VertAttributes::POSITION)));
    cube->set<float[24]>({
        min, min, min,
        max, min, min,
        max, max, min,
        min, max, min,
        min, min, max,
        max, min, max,
        max, max, max,
        min, max, max
    }, 0, 0);
    auto &part = cube->parts.emplace_back();
    part.indices.insert(part.indices.begin(), {
        0, 1, 3, 3, 1, 2,
        1, 5, 2, 2, 5, 6,
        5, 4, 6, 6, 4, 7,
        4, 0, 7, 7, 0, 3,
        3, 2, 7, 7, 2, 6,
        4, 5, 0, 0, 5, 1
    });
    return cube;
}

SharedMesh Mesh::getCube()
{
    static SharedMesh cube;
    if (cube == nullptr)
    {
        cube = createCube();
        VertBuffer::uploadSingleMesh(cube);
    }
    return cube;
}

unsigned int Mesh::getNrOfVertsReservedInVertBuffer() const
{
    if (!vertBuffer)
    {
        throw gu_err("Mesh \"" + name + "\" was not added to a VertBuffer.");
    }
    return nrOfVertsReservedInVertBuffer;
}

void VertData::removeVertices(int count)
{
    vertices.resize(vertices.size() - count * attributes.getVertSize());
}

void VertData::addVertices(int count)
{
    vertices.resize(vertices.size() + count * attributes.getVertSize(), 0);
}

int VertData::nrOfVertices() const
{
    return vertices.size() / attributes.getVertSize();
}

int Mesh::Part::getNrOfIndicesToRender() const
{
    return nrOfIndicesToRender < 0 ? nrOfIndices : nrOfIndicesToRender;
}
