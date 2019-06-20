
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
    VertBuffer *buffer = with(mesh->attributes);
    buffer->add(mesh)->upload(false);
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

    std::cout << "Adding " << mesh->name << " to VertBuffer " << vaoId << "\n";
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nrOfVerts * vertSize, NULL, GL_STATIC_DRAW);

    glGenBuffers(1, &iboId);    // create IndexBuffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * nrOfIndices, NULL, GL_STATIC_DRAW);

    GLuint vertsOffset = 0, indicesOffset = 0;
    for (std::weak_ptr<Mesh> m : meshes)
    {
        if (m.expired())
            throw gu_err("Trying to upload a VertBuffer whose Meshes are already destroyed");

        SharedMesh mesh = m.lock();
        GLuint
            vertsSize = mesh->nrOfVertices * sizeof(GLfloat) * vertSize,
            indicesSize = mesh->nrOfIndices * sizeof(GLushort);

        auto &indices = mesh->indices;
        #if EMSCRIPTEN
        if (!disposeOfflineData) indices = *&indices;

        for (auto &i : indices) i += mesh->baseVertex;
        #endif

        glBufferSubData(GL_ARRAY_BUFFER, vertsOffset, vertsSize, mesh->vertices.data());
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indicesOffset, indicesSize, indices.data());

        if (disposeOfflineData) mesh->disposeOfflineData();

        vertsOffset += vertsSize;
        indicesOffset += indicesSize;
    }
    setAttrPointersAndEnable();
    uploaded = true;
    if (next) next->upload(disposeOfflineData);

    /* To confirm the uploaded data is correct:

    float dataV[nrOfVerts * vertSize];
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertsOffset, dataV);

    std::cout << "Vertex data: ";
    for (int i = 0; i < nrOfVerts * vertSize; i += vertSize)
    {
        for (int j = 0; j < vertSize; j++)
            std::cout << dataV[i + j] << ", ";
        std::cout << std::endl;
    }
    std::cout << std::endl;

    GLushort dataI[nrOfIndices];
    glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indicesOffset, dataI);

    std::cout << "Indices: ";
    for (int i = 0; i < nrOfIndices; i++)
        std::cout << dataI[i] << ", ";
    std::cout << std::endl;
    // */
}

void VertBuffer::setAttrPointersAndEnable()
{
    GLint offset = 0;
    for (int i = 0; i < attrs.nrOfAttributes(); i++)
    {
        VertAttr &attr = attrs.get(i);
        glVertexAttribPointer(
            i,                                    // location of attribute that can be used in vertex shaders. eg: 'layout(location = 0) in vec3 position'
            attr.size,                            // size.
            GL_FLOAT,                             // type
            attr.normalized ? GL_TRUE : GL_FALSE, // normalized?
            vertSize * sizeof(GLfloat),           // stride
            (void *)(uintptr_t)offset             // offset
        );
        glEnableVertexAttribArray(i);
        offset += attr.size * sizeof(GLfloat);
    }
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
    if (vaoId == currentlyBoundVao)
        currentlyBoundVao = 0;
}
