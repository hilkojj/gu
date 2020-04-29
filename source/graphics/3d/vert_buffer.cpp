
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

VertBuffer *VertBuffer::with(VertAttributes &attributes)
{
    return new VertBuffer(attributes);
}

void VertBuffer::uploadSingleMesh(SharedMesh mesh)
{
    with(mesh->attributes)->add(mesh)->upload(false);
}

VertBuffer::VertBuffer(VertAttributes &attributes)

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
    if (next || nrOfVerts + mesh->nrOfVertices > std::numeric_limits<GLushort>::max())
    {
        if (!next) next = VertBuffer::with(attrs);
        next->add(mesh);
        return this;
    }
    #endif

    // std::cout << "Adding " << mesh->name << " to VertBuffer " << vaoId << "\n";
    meshes.push_back(mesh);

    mesh->baseVertex = nrOfVerts;
    nrOfVerts += mesh->nrOfVertices;

    mesh->indicesBufferOffset = nrOfIndices * sizeof(GLushort);
    nrOfIndices += mesh->nrOfIndices;

    mesh->vertBuffer = this;
    return this;
}

void VertBuffer::upload(bool disposeOfflineData)
{
    if (vboId)
        throw gu_err("VertBuffer already uploaded");

    std::cout << "Uploading vbo\n";
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
        GLuint
            vertsSize = mesh->nrOfVertices * vertSize,
            indicesSize = mesh->nrOfIndices * sizeof(GLushort);

        mesh->vertBufferOffset = vertsOffset;

        auto &indices = mesh->indices;
        #if EMSCRIPTEN
        if (!disposeOfflineData) indices = *&indices;

        for (auto &i : indices) i += mesh->baseVertex;
        #endif

        glBufferSubData(GL_ARRAY_BUFFER, mesh->vertBufferOffset, vertsSize, mesh->vertices.data());
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indicesOffset, indicesSize, indices.data());

        if (disposeOfflineData) mesh->disposeOfflineData();

        vertsOffset += vertsSize;
        indicesOffset += indicesSize;
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

                glVertexAttribIPointer(
                        i,                                    // location of attribute that can be used in vertex shaders. eg: 'layout(location = 0) in vec3 position'
                        attr.size,                            // size.
                        attr.type,                             // type
                        attrs.getVertSize(),                  // stride
                        (void *)(uintptr_t)offset             // offset
                );
                break;
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

GLuint VertBuffer::uploadPerInstanceData(const VertData &data, GLuint advanceRate, int id)
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
    glBufferData(GL_ARRAY_BUFFER, data.vertices.size(), &data.vertices[0], vboUsage);
    usePerInstanceData(id, advanceRate);
    return id;
}

void VertBuffer::onMeshDestroyed()
{
    std::cout << "A mesh in this VB was destroyed\n";
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
        std::cerr << "]\n";
    }
    std::cout << "Deleting VertBuffer: vao & vbo & ibo\n";
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

void VertBuffer::reuploadVertices(const SharedMesh &mesh)
{
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    int vertsOffset = mesh->vertBufferOffset;
    int vertsSize = mesh->nrOfVertices * vertSize;
    glBufferSubData(GL_ARRAY_BUFFER, vertsOffset, vertsSize, mesh->vertices.data());
}
