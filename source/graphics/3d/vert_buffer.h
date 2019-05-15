
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

    // try not to use this. It is more efficient to put more meshes (with the same VertAttributes) in 1 VertBuffer
    static void uploadSingleMesh(SharedMesh mesh);

    // adds mesh to Meshes that are going to be uploaded when upload() is called.
    VertBuffer* add(SharedMesh mesh);

    /**
     * upload all added Meshes to OpenGL, after uploading the Meshes can be drawn.
     * 
     * if disposeOfflineData is true then mesh.disposeOfflineData() will be called for each uploaded mesh.
     **/
    void upload(bool disposeOfflineData);

    bool isUploaded() const;

    void bind();

    void onMeshDestroyed();

    ~VertBuffer();

  private:

    VertBuffer(VertAttributes &attributes);

    // returns wether the stored vertex data is actually used by Meshes
    bool inUse() const;

    void setAttrPointersAndEnable();

    // ids of the VertexArrayObject, VertexBufferObject and IndexBufferObject
    GLuint vaoId = 0, vboId = 0, iboId;
    
    GLuint nrOfVerts = 0, vertSize = 0, nrOfIndices = 0;

    std::vector<std::weak_ptr<Mesh>> meshes;

    VertAttributes attrs;

    bool uploaded = false;

    static GLuint currentlyBoundVao;

};

#endif
