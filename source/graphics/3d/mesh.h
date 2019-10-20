
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include <iostream>

#include "vert_attributes.h"
#include "glm/glm.hpp"
using namespace glm;

class VertData
{
  public:
    VertData(VertAttributes attrs, std::vector<float> vertices);

    VertAttributes attributes;
    std::vector<float> vertices;

    template <class vecType>
    vecType get(int vertI, int attrOffset)
    {
        vecType v;
        for (int i = 0; i < vecType::length(); i++)
        {
            v[i] = vertices[vertI * attributes.getVertSize() + attrOffset + i];
        }
        return v;
    }

    template <class matType>
    matType getMat(int vertI, int attrOffset)
    {
        matType v;
        float *vp = &(v[0][0]);
        for (int i = 0; i < matType::length() * matType::length(); i++)
        {
            vp[i] = vertices[vertI * attributes.getVertSize() + attrOffset + i];
        }
        return v;
    }

    float getFloat(int vertI, int attrOffset);

    template <class vecType>
    void set(const vecType &v, int vertI, int attrOffset)
    {
        for (int i = 0; i < vecType::length(); i++)
        {
            vertices[vertI * attributes.getVertSize() + attrOffset + i] = v[i];
        }
    }

    template <class matType>
    void setMat(const matType &mat, int vertI, int attrOffset)
    {
        const float *vp = &(mat[0][0]);
        for (int i = 0; i < matType::length() * matType::length(); i++)
        {
            vertices[vertI * attributes.getVertSize() + attrOffset + i] = vp[i];
        }
    }

    void setFloat(float v, int vertI, int attrOffset);

    template <class vecType>
    void add(const vecType &v, int vertI, int attrOffset)
    {
        for (int i = 0; i < vecType::length(); i++)
        {
            vertices[vertI * attributes.getVertSize() + attrOffset + i] += v[i];
        }
    }

    template <class vecType>
    void normalizeVecAttribute(int attrOffset)
    {
        for (int vertI = 0; vertI < vertices.size() / attributes.getVertSize(); vertI++)
            set(normalize(get<vecType>(vertI, attrOffset)), vertI, attrOffset);
    }

    void removeVertices(int count);

    void addVertices(int count);
};

class VertBuffer;
class Mesh;

typedef std::shared_ptr<Mesh> SharedMesh;

class Mesh : public VertData
{
  public:

    // returns a shared(!) quad mesh with only the Position attribute. (position attribute can also be used for texture coordinates)
    static SharedMesh getQuad();

    std::string name;
    std::vector<unsigned short> indices;
    GLenum mode = GL_TRIANGLES;

    unsigned int nrOfVertices, nrOfIndices;

    VertBuffer *vertBuffer = nullptr;

    // variables used for glDrawElementsBaseVertex: (https://www.khronos.org/opengl/wiki/GLAPI/glDrawElementsBaseVertex)
    int baseVertex = 0, indicesBufferOffset = 0;

    Mesh(
        std::string name,
        unsigned int nrOfVertices,
        unsigned int nrOfIndices,
        VertAttributes attributes);

    // removes the vertices + indices that are stored in RAM,
    // but the mesh can still be drawn if it is uploaded to VRAM/OpenGL using a VertBuffer
    // WARNING: vertices & indices are resized to 0!
    void disposeOfflineData();

    void render();

    void renderInstances(GLsizei count);

    ~Mesh();

  private:
    static SharedMesh quad;
};

#endif
