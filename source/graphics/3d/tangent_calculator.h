#ifndef TANGENT_CALCULATOR_H
#define TANGENT_CALCULATOR_H

#include "mesh.h"
#include "glm/glm.hpp"
using namespace glm;

namespace TangentCalculator
{

/**
 * Returns the tangent of triangle (p0, p1, p2).
 * For each point in the triangle an UV point (texture coords) is required.
 * https://youtu.be/4DUfwAEx4Ts?t=423
 */
vec3 calculateTangent(
    vec3 &p0, vec3 &p1, vec3 &p2,
    vec2 &uv0, vec2 &uv1, vec2 &uv2);

/**
 * Adds Tangents to mesh.
 * Mesh must have attributes including: VertAttributes::POSITION, VertAttributes::TEX_COORDS, VertAttributes::TANGENT.
 */
void addTangentsToMesh(SharedMesh mesh);
void addTangentsToMesh(Mesh *mesh);

} // namespace TangentCalculator


#endif
