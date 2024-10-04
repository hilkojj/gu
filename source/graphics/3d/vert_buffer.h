
#ifndef VERT_BUFFER_H
#define VERT_BUFFER_H

#include "vert_attributes.h"
#include "mesh.h"
#include "../external/gl_includes.h"

#include <memory>

/**
 * A class that encapsulates OpenGL VertexArrayObjects, VertexBufferObjects, IndexBufferObjects and Instanced Arrays
 */
class VertBuffer
{
  public:

    // This class keeps track of what vertex array is bound in the game. If you are going to call glBindVertexArray() yourself, then please update this value or things can break.
    static GLuint currentlyBoundVao;

    GLenum vboUsage = GL_STATIC_DRAW;
    GLenum iboUsage = GL_STATIC_DRAW;

    // creates a VertBuffer for meshes with these attributes
    static VertBuffer* with(const VertAttributes &);

    // try not to use this. It is more efficient to put more meshes (with the same VertAttributes) in 1 VertBuffer
    static void uploadSingleMesh(SharedMesh);

    // adds mesh to Meshes that are going to be uploaded when upload() is called.
    VertBuffer* add(SharedMesh);

    /**
     * upload all added Meshes to OpenGL, after uploading the Meshes can be drawn.
     * 
     * if disposeOfflineData is true then mesh.disposeOfflineData() will be called for each uploaded mesh.
     **/
    void upload(bool disposeOfflineData);

    bool isUploaded() const;

    void bind();

    void onMeshDestroyed(); // Called by ~Mesh()

    void reuploadVertices(const SharedMesh &, int numVerticesToReuploadOrAll=-1); // -1 -> all

    /**
     * upload vertex-attributes that do not advance per vertex, but per instance (glDrawElementsInstanced() & glVertexAttribDivisor())
     * also known as Instanced Arrays (https://www.khronos.org/opengl/wiki/Vertex_Specification#Instanced_arrays)
     *
     * useful for when you want to render a mesh multiple times in 1 draw call, but each on a different position.
     *
     * returns the id of the uploaded buffer. If you want to update the same buffer at a later moment call this function again with id != -1
     **/
    GLuint uploadPerInstanceData(const VertData &, GLuint advanceRate=1, int id=-1, GLenum usage=GL_STATIC_DRAW);

    void usePerInstanceData(GLuint instanceDataId, GLuint advanceRate=1);

    void deletePerInstanceData(GLuint instanceDataId);

    ~VertBuffer();

  private:

    // in WebGL there's a limit for the amount of vertices in a VertBuffer, when exceeding this amount the remaining meshes will be uploaded to 'next'
    VertBuffer *next = NULL;

    VertBuffer(const VertAttributes &);

    // returns wether the stored vertex data is actually used by Meshes
    bool inUse() const;

    void setAttrPointersAndEnable(VertAttributes &, unsigned int divisor=0, unsigned int locationOffset=0);

    // ids of the VertexArrayObject, VertexBufferObject and IndexBufferObject
    GLuint vaoId = 0, vboId = 0, iboId;

    std::vector<GLuint> instanceVbos;
    std::vector<VertAttributes> instanceVboAttrs;
    
    GLuint nrOfVerts = 0, nrOfIndices = 0;

    std::vector<std::weak_ptr<Mesh>> meshes;

    VertAttributes attrs;

    bool uploaded = false;

};

#endif
