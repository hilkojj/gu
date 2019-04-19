
#include <iostream>
#include <memory>

#include "vert_buffer.h"
#include "mesh.h"
#include "vert_attributes.h"

VertBuffer *VertBuffer::with(VertAttributes &attributes)
{
    return new VertBuffer(attributes);
}

VertBuffer::VertBuffer(VertAttributes &attributes)
    : vertSize(attributes.getVertSize())
{
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    for (int i = 0; i < attributes.nrOfAttributes(); i++)
    {
        VertAttr &attr = attributes.get(i);
        glVertexAttribPointer(
            i,                                    // location of VertexPosition that is used in vertex shader. 'layout(location = 0)'
            attr.size,                            // size.
            GL_FLOAT,                             // type
            attr.normalized ? GL_TRUE : GL_FALSE, // normalized?
            0,                                    // stride
            (void *)0                             // offset
        );
    }
}

VertBuffer *VertBuffer::add(SharedMesh mesh)
{
    if (mesh->vertBuffer)
        throw std::runtime_error(mesh->name + " was already added to a VertBuffer");

    std::cout << "Adding " << mesh->name << " to VertBuffer\n";
    meshes.push_back(mesh);
    nrOfVerts += mesh->nrOfVertices;
    mesh->vertBuffer = this;
    return this;
}

void VertBuffer::upload()
{
    if (vboId)
        throw std::runtime_error("VertBuffer already uploaded");

    std::cout << "Uploading vbo\n";
    glBindVertexArray(vaoId);

    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nrOfVerts * vertSize, NULL, GL_STATIC_DRAW);

    GLuint offset = 0;
    for (std::weak_ptr<Mesh> m : meshes)
    {
        if (m.expired())
            throw std::runtime_error("Trying to upload a VertBuffer whose Meshes are already destroyed");

        SharedMesh mesh = m.lock();
        GLuint size = mesh->nrOfVertices * sizeof(GLfloat) * vertSize;

        glBufferSubData(GL_ARRAY_BUFFER, offset, size, mesh->vertices.data());

        offset += size;
    }

    /* To confirm the uploaded data is correct:

    float data[nrOfVerts * vertSize];

    glGetBufferSubData(GL_ARRAY_BUFFER, 0, offset, data);

    for (int i = 0; i < nrOfVerts * vertSize; i++)
        std::cout << data[i];

    std::cout << std::endl;
    */
}

void VertBuffer::onMeshDestroyed()
{
    std::cout << "A mesh in this VB was destroyed\n";

    if (!inUse())
        delete this;
}

bool VertBuffer::inUse()
{
    for (std::weak_ptr<Mesh> m : meshes)
        if (!m.expired())
            return true;
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

    std::cout << "Deleting VertBuffer: vao & vbo\n";
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(1, &vboId);
}
