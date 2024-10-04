#ifndef SPHERE_MESH_GENERATOR_H
#define SPHERE_MESH_GENERATOR_H

#include "graphics/3d/mesh.h"

namespace mu
{

/**
 * Generates a Sphere mesh.
 * Define what attributes the vertices should have in 'attrs'.
 * POSITION is required, NORMAL, TANGENT and TEX_COORDS are supported.
 **/
SharedMesh generateSphereMesh(const std::string &meshName, float radius, int segments, int rings, VertAttributes &attrs);

}

#endif
