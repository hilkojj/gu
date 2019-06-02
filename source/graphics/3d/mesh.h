
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include <iostream>

#include "vert_attributes.h"
#include "glm/glm.hpp"
using namespace glm;

class VertBuffer;
class Mesh;

typedef std::shared_ptr<Mesh> SharedMesh;

class Mesh
{

  public:

    // returns a shared(!) quad mesh with only the Position attribute. (position attribute can also be used for texture coordinates)
    static SharedMesh getQuad();

    std::string name;
    std::vector<float> vertices;
    std::vector<unsigned short> indices;
    GLenum mode = GL_TRIANGLES;

    unsigned int nrOfVertices, nrOfIndices;

    VertAttributes attributes;

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

    ~Mesh();

    vec4 getVec4(int vertI, int attrOffset);
    vec3 getVec3(int vertI, int attrOffset);
    vec2 getVec2(int vertI, int attrOffset);
    float getFloat(int vertI, int attrOffset);

    void setVec4(const vec4 &v, int vertI, int attrOffset);
    void setVec3(const vec3 &v, int vertI, int attrOffset);
    void setVec2(const vec2 &v, int vertI, int attrOffset);
    void setFloat(float v, int vertI, int attrOffset);

    void addVec4(const vec4 &v, int vertI, int attrOffset);
    void addVec3(const vec3 &v, int vertI, int attrOffset);
    void addVec2(const vec2 &v, int vertI, int attrOffset);

    void normalizeVec3Attribute(int attrOffset);
    void normalizeVec2Attribute(int attrOffset);

  private:

    static SharedMesh quad;

};

#endif
