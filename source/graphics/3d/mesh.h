
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include <iostream>
#include <cstring>

#include "vert_attributes.h"
#include "../../utils/math_utils.h"

typedef unsigned char u_char;

class VertData
{
  public:
    VertData(VertAttributes attrs, std::vector<u_char> vertices);

    VertAttributes attributes;
    std::vector<u_char> vertices;

    template <class type>
    inline type &getRef(int vertI, int attrOffset)
    {
        return *((type *) &(vertices[vertI * attributes.getVertSize() + attrOffset]));
    }

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

    int nrOfVertices() const;
};

class VertBuffer;
class Mesh;

typedef std::shared_ptr<Mesh> SharedMesh;

class Mesh : public VertData
{
  public:
    // returns a shared(!) uploaded quad mesh with only the Position attribute. (position attribute can also be used for texture coordinates)
    static SharedMesh getQuad();

    // creates a quad mesh. Not the same as getQuad()
    static SharedMesh createQuad(float min=-1, float max=1);

    static SharedMesh getCube();

    static SharedMesh createCube(float min=-.5, float max=.5);

    std::string name;

    VertBuffer *vertBuffer = nullptr;

    struct Part
    {
        std::string name;
        std::vector<unsigned short> indices;
        GLenum mode = GL_TRIANGLES;
        int nrOfIndicesToRender = -1; // -1 => all

        int getNrOfIndicesToRender() const;

      private:
        // also used for glDrawElementsBaseVertex
        friend VertBuffer;
        friend Mesh;
        int indicesBufferOffset = 0;
        unsigned int nrOfIndices;
    };

    std::vector<Part> parts;

    Mesh(
        const std::string& name,
        unsigned int nrOfVertices,
        VertAttributes attributes);

    void render(int part=0);

    void renderInstances(GLsizei count, int part=0);

    void renderArrays(GLenum mode = GL_TRIANGLES, int nrOfVerts=-1) const;

    // removes the vertices + indices that are stored in RAM,
    // but the mesh can still be drawn if it is uploaded to VRAM/OpenGL using a VertBuffer
    // WARNING: vertices & indices are resized to 0!
    void disposeOfflineData();

    unsigned int getNrOfVertsReservedInVertBuffer() const;

    ~Mesh();

  private:
    static SharedMesh quad, cube;


    // variables used for glDrawElementsBaseVertex: (https://www.khronos.org/opengl/wiki/GLAPI/glDrawElementsBaseVertex)
    friend VertBuffer;
    int baseVertex = 0;
    int vertBufferOffset = 0;
    unsigned int nrOfVertsReservedInVertBuffer;
};

#endif
