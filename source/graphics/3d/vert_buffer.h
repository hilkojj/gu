
#ifndef VERT_BUFFER_H
#define VERT_BUFFER_H

#include <memory>

#include "glad/glad.h"
#include "vert_attributes.h"
#include "mesh.h"

class VertBuffer
{
  public:

    static VertBuffer* with(VertAttributes &attributes);

    void upload();

    void onMeshDestroyed();

    ~VertBuffer();

    VertBuffer* add(SharedMesh mesh);

    bool inUse();

  private:

    VertBuffer(VertAttributes &attributes);

    GLuint vaoId = 0, vboId = 0;
    
    GLuint nrOfVerts = 0, vertSize = 0;

    std::vector<std::weak_ptr<Mesh>> meshes;

};

#endif
