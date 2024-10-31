#pragma once

#include "vert_attributes.h"
#include "shared_3d.h"

#include <vector>

class VertData
{
  public:
    VertAttributes attributes;
    std::vector<unsigned char> vertexData;

    VertData(VertAttributes attrs, std::vector<unsigned char> vertices);

    template <class type>
    inline type &get(int vertI, int attrOffset)
    {
        return *((type *) &(vertexData[vertI * attributes.getVertSize() + attrOffset]));
    }

    template <class vecType>
    void normalizeVecAttribute(int attrOffset)
    {
        for (int vertI = 0; vertI < vertexData.size() / attributes.getVertSize(); vertI++)
        {
            vecType &value = get<vecType>(vertI, attrOffset);
            value = normalize(value);
        }
    }

    void removeVertices(int count);

    void addVertices(int count);

    int nrOfVertices() const;
};


class VertBuffer;

class Mesh : public VertData
{
  public:
    struct Part
    {
        std::string name;
        std::vector<unsigned short> indices;
        GLenum mode = GL_TRIANGLES;
        int nrOfIndicesToRender = -1; // -1 => all

        int getNumIndicesToRender() const;

      private:
        // also used for glDrawElementsBaseVertex
        friend VertBuffer;
        friend Mesh;

        // The position and size of this Part when it was added to a VertBuffer:
        struct
        {
            int indicesOffset = 0;
            int numIndices = 0;
        }
        inBuffer;
    };

    Mesh(
        const std::string &name,
        unsigned int nrOfVertices,
        const VertAttributes &attributes
    );

    void render(int part = 0);

    void renderInstances(GLsizei count, int part = 0);

    void renderArrays(GLenum mode = GL_TRIANGLES, int numVerts = -1 /* -1 => all */) const;

    /* Removes the vertices + indices that are stored in memory,
     * but the mesh can still be drawn if it was uploaded to VRAM/OpenGL using a VertBuffer.
     * WARNING: vertices & indices are resized to 0!
     */
    void disposeOfflineData();

    int getNumVertsReservedInBuffer();

    ~Mesh();

    // Returns a static shared quad, which has been uploaded.
    static SharedMesh getUploadedQuad();

    static SharedMesh createQuad(float min = -1.0f, float max = 1.0f);

    // Returns a static shared cube, which has been uploaded.
    static SharedMesh getUploadedCube();

    static SharedMesh createCube(float min= -0.5f, float max = 0.5f);

    std::string name;
    VertBuffer *vertBuffer = nullptr;
    std::vector<Part> parts;

  private:
    // variables used for glDrawElementsBaseVertex: (https://www.khronos.org/opengl/wiki/GLAPI/glDrawElementsBaseVertex)

    friend VertBuffer;
    // Position and size of the mesh in the vert buffer:
    struct
    {
        int baseVertex = 0;
        int vertOffset = 0;
        unsigned int numVertsReserved = 0;
    }
    inBuffer;
};
