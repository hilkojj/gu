
#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include <vector>

#include "mesh.h"

class MeshInstance
{
  public:
    SharedMesh mesh;

    MeshInstance(SharedMesh mesh);

    ~MeshInstance();

};

#endif
