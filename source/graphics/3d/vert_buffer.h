
#ifndef VERT_BUFFER_H
#define VERT_BUFFER_H

#include <memory>

#include "glad/glad.h"
#include "vert_attributes.h"
#include "mesh.h"

class VertBuffer
{
  public:

    // creates a VertBuffer for meshes with these attributes
    static VertBuffer* with(VertAttributes &attributes);

    /**
     * upload all added Meshes to OpenGL, after uploading the Meshes can be drawn.
     * 
     * if disposeOfflineData is true then mesh.disposeOfflineData() will be called for each uploaded mesh.
     **/
    void upload(bool disposeOfflineData);

    // adds mesh to Meshes that are going to be uploaded when upload() is called.
    VertBuffer* add(SharedMesh mesh);

    void bind();

    void onMeshDestroyed();

    ~VertBuffer();

  private:

    VertBuffer(VertAttributes &attributes);

    // returns wether the stored vertex data is actually used by Meshes
    bool inUse();

    void setAttrPointersAndEnable();

    // ids of the VertexArrayObject, VertexBufferObject and IndexBufferObject
    GLuint vaoId = 0, vboId = 0, iboId;
    
    GLuint nrOfVerts = 0, vertSize = 0, nrOfIndices = 0;

    std::vector<std::weak_ptr<Mesh>> meshes;

    VertAttributes attrs;

    static GLuint currentlyBoundVao;

};

#endif
