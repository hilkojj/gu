#include "mesh.h"
#include "vert_buffer.h"

#include "../../math/math_utils.h"
#include "../../utils/gu_error.h"

VertData::VertData(VertAttributes attrs, std::vector<unsigned char> vertices) :
    attributes(std::move(attrs)),
    vertexData(std::move(vertices))
{}

void VertData::removeVertices(int count)
{
    vertexData.resize(vertexData.size() - count * attributes.getVertSize());
}

void VertData::addVertices(int count)
{
    vertexData.resize(vertexData.size() + count * attributes.getVertSize(), 0);
}

int VertData::nrOfVertices() const
{
    return vertexData.size() / attributes.getVertSize();
}

int Mesh::Part::getNumIndicesToRender() const
{
    return nrOfIndicesToRender < 0 ? inBuffer.numIndices : nrOfIndicesToRender;
}

Mesh::Mesh(const std::string &name, unsigned int nrOfVertices, const VertAttributes &attributes) :
    name(name),
    VertData(attributes, std::vector<unsigned char>(nrOfVertices * attributes.getVertSize()))
{
}

namespace
{

inline void ensureBuffer(const Mesh &mesh)
{
    if (!mesh.vertBuffer || !mesh.vertBuffer->isUploaded())
    {
        throw gu_err(mesh.name + " is not uploaded. Upload it first with a VertBuffer");
    }
}

inline void ensureBufferAndPart(const Mesh &mesh, const int partI)
{
    ensureBuffer(mesh);
    if (partI >= mesh.parts.size())
    {
        throw gu_err(mesh.name + " only has " + std::to_string(mesh.parts.size()) + " part(s). Tried to render part #" + std::to_string(partI));
    }
}

}

void Mesh::render(const int partI)
{
    ensureBufferAndPart(*this, partI);
    vertBuffer->bind();
    const Part &part = parts.at(partI);

    #ifdef EMSCRIPTEN
    glDrawElements(
        part.mode,
        part.getNumIndicesToRender(),
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t) part.inBuffer.indicesOffset
    );
    #else
    glDrawElementsBaseVertex(
        part.mode,
        part.getNumIndicesToRender(),
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t) part.inBuffer.indicesOffset,
        inBuffer.baseVertex
    );
    #endif
}

void Mesh::renderInstances(const GLsizei count, const int partI)
{
    ensureBufferAndPart(*this, partI);
    vertBuffer->bind();
    const Part &part = parts.at(partI);

    #ifdef EMSCRIPTEN
    EM_ASM({
        gl.drawElementsInstanced($0, $1, $2, $3, $4);
    },
    // glDrawElementsInstanced(
        part.mode,
        part.getNumIndicesToRender(),
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t) part.inBuffer.indicesOffset,
        count
    // );
    );
    #else
    glDrawElementsInstancedBaseVertex(
        part.mode,
        part.getNumIndicesToRender(),
        GL_UNSIGNED_SHORT,
        (void *)(uintptr_t) part.inBuffer.indicesOffset,
        count,
        inBuffer.baseVertex
    );
    #endif
}

void Mesh::renderArrays(GLenum mode, const int numVerts) const
{
    ensureBuffer(*this);
    vertBuffer->bind();
    glDrawArrays(
        mode,
        inBuffer.baseVertex,
        numVerts == -1 ? inBuffer.numVertsReserved : numVerts
    );
}

void Mesh::disposeOfflineData()
{
    vertexData.resize(0);
    vertexData.shrink_to_fit();

    for (Part &part : parts)
    {
        part.indices.resize(0);
        part.indices.shrink_to_fit();
    }
}

int Mesh::getNumVertsReservedInBuffer()
{
    return inBuffer.numVertsReserved;
}

Mesh::~Mesh()
{
    if (vertBuffer)
    {
        vertBuffer->onMeshDestroyed();
    }
}

SharedMesh Mesh::getUploadedQuad()
{
    static SharedMesh quad;
    if (quad == nullptr)
    {
        quad = createQuad();
        VertBuffer::uploadSingleMesh(quad);
    }
    return quad;
}

SharedMesh Mesh::createQuad(float min, float max)
{
    SharedMesh quad(new Mesh("quad", 4, VertAttributes().add_(VertAttributes::POSITION)));
    quad->get<vec3>(0, 0) = { min, min, 0.0f };
    quad->get<vec3>(1, 0) = { min, max, 0.0f };
    quad->get<vec3>(2, 0) = { max, max, 0.0f };
    quad->get<vec3>(3, 0) = { max, min, 0.0f };

    auto &part = quad->parts.emplace_back();
    part.indices.insert(part.indices.begin(), {
        2, 1, 0,
        0, 3, 2,
    });
    return quad;
}

SharedMesh Mesh::getUploadedCube()
{
    static SharedMesh cube;
    if (cube == nullptr)
    {
        cube = createCube();
        VertBuffer::uploadSingleMesh(cube);
    }
    return cube;
}

SharedMesh Mesh::createCube(float min, float max)
{
    SharedMesh cube(new Mesh("cube", 8, VertAttributes().add_(VertAttributes::POSITION)));
    cube->get<vec3>(0, 0) = { min, min, min };
    cube->get<vec3>(1, 0) = { max, min, min };
    cube->get<vec3>(2, 0) = { max, max, min };
    cube->get<vec3>(3, 0) = { min, max, min };
    cube->get<vec3>(4, 0) = { min, min, max };
    cube->get<vec3>(5, 0) = { max, min, max };
    cube->get<vec3>(6, 0) = { max, max, max };
    cube->get<vec3>(7, 0) = { min, max, max };

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
