
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include <iostream>
#include <cstring>

#include "vert_attributes.h"
#include "glm/glm.hpp"
using namespace glm;

class VertData
{
  public:
    VertData(VertAttributes attrs, std::vector<u_char> vertices);

    VertAttributes attributes;
    std::vector<u_char> vertices;

    template <class type>
    inline type get(int vertI, int attrOffset)
    {
        type v;
        memcpy(
                &v,
                &(vertices[vertI * attributes.getVertSize() + attrOffset]),
                sizeof(type)
        );
        return v;
    }

    template <class type>
    inline void set(const type &v, int vertI, int attrOffset)
    {
        memcpy(
                &(vertices[vertI * attributes.getVertSize() + attrOffset]),
                &v,
                sizeof(type)
        );
    }

    template <class type>
    inline void add(const type &v, int vertI, int attrOffset)
    {
        set(get<type>(vertI, attrOffset) + v, vertI, attrOffset);
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
        const std::string& name,
        unsigned int nrOfVertices,
        unsigned int nrOfIndices,
        VertAttributes attributes);

    // removes the vertices + indices that are stored in RAM,
    // but the mesh can still be drawn if it is uploaded to VRAM/OpenGL using a VertBuffer
    // WARNING: vertices & indices are resized to 0!
    void disposeOfflineData();

    void render();

    void renderInstances(GLsizei count);

    void renderArrays();

    ~Mesh();

  private:
    static SharedMesh quad;
};

#endif
