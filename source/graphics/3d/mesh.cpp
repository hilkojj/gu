#include <utility>


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
        quad = createQuad();
        VertBuffer::uploadSingleMesh(quad);
    }
    return quad;
}

VertData::VertData(VertAttributes attrs, std::vector<u_char> vertices)
    : attributes(std::move(attrs)), vertices(std::move(vertices))
{}

Mesh::Mesh(const std::string& name, unsigned int nrOfVertices, VertAttributes attributes)

    : name(name),

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

    for (auto &part : parts)
    {
        part.indices.resize(0);
        part.indices.shrink_to_fit();
    }
}

void Mesh::render(int partI)
{
    if (!vertBuffer || !vertBuffer->isUploaded()) throw gu_err(name + " is not uploaded. Upload it first with a VertBuffer");
    vertBuffer->bind();
    if (parts.size() <= partI) throw gu_err(name + " only has " + std::to_string(parts.size()) + " part(s). Requested: " + std::to_string(partI));
    auto &part = parts.at(partI);

    #ifdef EMSCRIPTEN
    glDrawElements(
        part.mode,
        part.nrOfIndices,
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t) part.indicesBufferOffset
    );
    #else
    glDrawElementsBaseVertex(
        part.mode,
        part.nrOfIndices,
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t) part.indicesBufferOffset,
        baseVertex
    );
    #endif
}

void Mesh::renderInstances(GLsizei count, int partI)
{
    if (!vertBuffer || !vertBuffer->isUploaded()) throw gu_err(name + " is not uploaded. Upload it first with a VertBuffer");
    vertBuffer->bind();
    if (parts.size() <= partI) throw gu_err(name + " only has " + std::to_string(parts.size()) + " part(s). Requested: " + std::to_string(partI));
    auto &part = parts.at(partI);

    #ifdef EMSCRIPTEN
    EM_ASM({
        gl.drawElementsInstanced($0, $1, $2, $3, $4);
    },
    // glDrawElementsInstanced(
        part.mode,
        part.nrOfIndices,
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t) part.indicesBufferOffset,
        count
    // );
    );
    #else
    glDrawElementsInstancedBaseVertex(
        part.mode,
        part.nrOfIndices,
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
        vertBuffer->onMeshDestroyed();
}

void Mesh::renderArrays(GLenum mode) const
{
    if (!vertBuffer || !vertBuffer->isUploaded()) throw gu_err(name + " is not uploaded. Upload it first with a VertBuffer");
    vertBuffer->bind();
    glDrawArrays(
            mode,
            baseVertex,
            _nrOfVertices
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
