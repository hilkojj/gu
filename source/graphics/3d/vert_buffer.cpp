
#include <iostream>
#include <memory>
#include <string>
#include <limits>

#include "../../gl_includes.h"
#include "vert_buffer.h"
#include "mesh.h"
#include "vert_attributes.h"
#include "../../utils/gu_error.h"

GLuint VertBuffer::currentlyBoundVao = 0;

VertBuffer *VertBuffer::with(const VertAttributes &attributes)
{
    return new VertBuffer(attributes);
}

void VertBuffer::uploadSingleMesh(SharedMesh mesh)
{
    with(mesh->attributes)->add(mesh)->upload(false);
}

VertBuffer::VertBuffer(const VertAttributes &attributes)

    : vertSize(attributes.getVertSize()), attrs(attributes)
{
    glGenVertexArrays(1, &vaoId);
}

void VertBuffer::bind()
{
    if (currentlyBoundVao != vaoId) // todo: this will break when glBindVertexArray() is called from another place.
    {
        glBindVertexArray(vaoId);
        currentlyBoundVao = vaoId;
        // vbo and ibo should still be bound at this point.
    }
}

VertBuffer *VertBuffer::add(SharedMesh mesh)
{
    if (mesh->vertBuffer)
        throw gu_err(mesh->name + " was already added to a VertBuffer");

    #ifdef EMSCRIPTEN
    if (next || nrOfVerts + mesh->nrOfVertices() > std::numeric_limits<GLushort>::max())
    {
        if (!next) next = VertBuffer::with(attrs);
        next->add(mesh);
        return this;
    }
    #endif

    // std::cout << "Adding " << mesh->name << " to VertBuffer " << vaoId << "\n";
    meshes.push_back(mesh);

    mesh->baseVertex = nrOfVerts;
    mesh->_nrOfVertices = mesh->nrOfVertices();
    nrOfVerts += mesh->_nrOfVertices;

    for (auto &part : mesh->parts)
    {
        part.indicesBufferOffset = nrOfIndices * sizeof(GLushort);
        part.nrOfIndices = part.indices.size();
        nrOfIndices += part.nrOfIndices;
    }

    mesh->vertBuffer = this;
    return this;
}

void VertBuffer::upload(bool disposeOfflineData)
{
    if (vboId)
        throw gu_err("VertBuffer already uploaded");

    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "Uploading vbo\n";
    #endif
    bind();

    glGenBuffers(1, &vboId);    // create VertexBuffer
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, nrOfVerts * vertSize, NULL, vboUsage);

    glGenBuffers(1, &iboId);    // create IndexBuffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * nrOfIndices, NULL, iboUsage);

    GLuint vertsOffset = 0, indicesOffset = 0;
    for (std::weak_ptr<Mesh> m : meshes)
    {
        if (m.expired())
            throw gu_err("Trying to upload a VertBuffer whose Meshes are already destroyed");

        SharedMesh mesh = m.lock();

        if (mesh->nrOfVertices() != mesh->_nrOfVertices)
            throw gu_err("Mesh vertices have resized between .add() and .upload() for " + mesh->name);

        GLuint vertsSize = mesh->_nrOfVertices * vertSize;
        mesh->vertBufferOffset = vertsOffset;
        glBufferSubData(GL_ARRAY_BUFFER, mesh->vertBufferOffset, vertsSize, mesh->vertices.data());
        vertsOffset += vertsSize;

        for (auto &part : mesh->parts)
        {
            if (part.indices.size() != part.nrOfIndices)
                throw gu_err("Mesh part indices have resized between .add() and .upload() for mesh: " + mesh->name + " part: " + part.name);

            #if EMSCRIPTEN
            for (auto &i : part.indices) i += mesh->baseVertex;
            if (!disposeOfflineData) for (auto &i : part.indices) i -= mesh->baseVertex;
            #endif

            GLuint indicesSize = part.nrOfIndices * sizeof(GLushort);

            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indicesOffset, indicesSize, part.indices.data());
            indicesOffset += indicesSize;
        }

        if (disposeOfflineData) mesh->disposeOfflineData();
    }
    setAttrPointersAndEnable(attrs);
    uploaded = true;
    if (next) next->upload(disposeOfflineData);
}

void VertBuffer::setAttrPointersAndEnable(VertAttributes &attrs, unsigned int divisor, unsigned int locationOffset)
{
    GLint offset = 0;
    for (int i = locationOffset; i < locationOffset + attrs.nrOfAttributes(); i++)
    {
        auto &attr = attrs.get(i - locationOffset);
        glDisableVertexAttribArray(i);
        switch (attr.type)
        {
            case GL_INT:
            case GL_UNSIGNED_INT:
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
            case GL_BYTE:
            case GL_UNSIGNED_BYTE:
                if (!attr.normalized)
                {
                    glVertexAttribIPointer(
                        i,                                    // location of attribute that can be used in vertex shaders. eg: 'layout(location = 0) in vec3 position'
                        attr.size,                            // size.
                        attr.type,                             // type
                        attrs.getVertSize(),                  // stride
                        (void *)(uintptr_t)offset             // offset
                    );
                    break;
                }
            default:
                glVertexAttribPointer(
                        i,                                    // location of attribute that can be used in vertex shaders. eg: 'layout(location = 0) in vec3 position'
                        attr.size,                            // size.
                        attr.type,                             // type
                        attr.normalized ? GL_TRUE : GL_FALSE, // normalized?
                        attrs.getVertSize(),                  // stride
                        (void *)(uintptr_t)offset             // offset
                );
        }
        glEnableVertexAttribArray(i);
        offset += attr.byteSize;

        if (divisor)
        {
            #ifdef EMSCRIPTEN
            EM_ASM({
                gl.vertexAttribDivisor($0, $1);
            }, i, divisor);
            #else
            glVertexAttribDivisor(i, divisor);
            #endif
        }
    }
}

GLuint VertBuffer::uploadPerInstanceData(const VertData &data, GLuint advanceRate, int id, GLenum usage)
{
    bind();
    if (id == -1)
    {
        id = instanceVbos.size();
        for (int i = 0; i < instanceVbos.size(); i++) if (instanceVbos[i] == -1) id = i;
        if (id == instanceVbos.size())
        {
            instanceVbos.emplace_back();
            instanceVboAttrs.emplace_back();
        }
        glGenBuffers(1, &instanceVbos[id]);
        instanceVboAttrs[id] = data.attributes;
    }

    glBindBuffer(GL_ARRAY_BUFFER, instanceVbos[id]);
    if (data.vertices.size() > 0)
        glBufferData(GL_ARRAY_BUFFER, data.vertices.size(), &data.vertices[0], usage);
    usePerInstanceData(id, advanceRate);
    return id;
}

void VertBuffer::onMeshDestroyed()
{
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "A mesh in this VB was destroyed\n";
    #endif
    if (!inUse()) delete this;
}

bool VertBuffer::isUploaded() const
{
    return uploaded;
}

bool VertBuffer::inUse() const
{
    for (std::weak_ptr<Mesh> m : meshes)
        if (!m.expired()) return true;
    return false;
}

VertBuffer::~VertBuffer()
{
    if (inUse())
    {
        std::cerr << "WARNING: Deleting a VertBuffer that is still in use by [";
        bool first = true;

        for (std::weak_ptr<Mesh> m : meshes)
            if (!m.expired())
            {
                if (!first)
                    std::cerr << ", ";
                std::cerr << m.lock()->name;
                first = false;
            }
        std::cerr << "]" << std::endl;
    }
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "Deleting VertBuffer: vao & vbo & ibo\n";
    #endif
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(1, &vboId);
    glDeleteBuffers(1, &iboId);

    for (auto &id : instanceVbos)
        if (id != -1) glDeleteBuffers(1, &id);

    if (vaoId == currentlyBoundVao)
        currentlyBoundVao = 0;
}

void VertBuffer::usePerInstanceData(GLuint instanceDataId, GLuint advanceRate)
{
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbos[instanceDataId]);
    setAttrPointersAndEnable(instanceVboAttrs[instanceDataId], advanceRate, attrs.nrOfAttributes());
}

void VertBuffer::deletePerInstanceData(GLuint instanceDataId)
{
    bind();
    glDeleteBuffers(1, &instanceVbos[instanceDataId]);
    instanceVbos[instanceDataId] = -1;
}

void VertBuffer::reuploadVertices(const SharedMesh &mesh, int nrOfVerticesToReupload)
{
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    int vertsOffset = mesh->vertBufferOffset;
    if (nrOfVerticesToReupload > mesh->_nrOfVertices)
        throw gu_err("Cannot reupload more verts than before. Mesh: " + mesh->name);
    uint vertsSize = (nrOfVerticesToReupload == -1 ? mesh->_nrOfVertices : uint(nrOfVerticesToReupload)) * vertSize;
    glBufferSubData(GL_ARRAY_BUFFER, vertsOffset, vertsSize, mesh->vertices.data());
}
