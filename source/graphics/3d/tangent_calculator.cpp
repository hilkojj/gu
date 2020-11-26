
#include "tangent_calculator.h"

namespace TangentCalculator
{

/**
 * Returns the tangent of triangle (p0, p1, p2).
 * For each point in the triangle an UV point (texture coords) is required.
 * https://youtu.be/4DUfwAEx4Ts?t=423
 */
vec3 calculateTangent(
    vec3 &p0, vec3 &p1, vec3 &p2,
    vec2 &uv0, vec2 &uv1, vec2 &uv2)
{
    vec3 deltaPos1 = vec3(p1 - p0);
    vec3 deltaPos2 = vec3(p2 - p0);

    vec2 deltaUv1 = vec2(uv1 - uv0);
    vec2 deltaUv2 = vec2(uv2 - uv0);

    float r = 1 / (deltaUv1.x * deltaUv2.y - deltaUv1.y * deltaUv2.x);
    vec3 tangent = vec3(deltaPos1 * deltaUv2.y) - vec3(deltaPos2 * deltaUv1.y);
    tangent *= r;
    return normalize(tangent);
}

void addTangentsToMesh(SharedMesh mesh, int meshPart)
{
    addTangentsToMesh(mesh.get(), meshPart);
}

void addTangentsToMesh(Mesh *mesh, int meshPart)
{
    VertAttributes &attrs = mesh->attributes;
    int posOffset = attrs.getOffset(VertAttributes::POSITION);
    int texOffset = attrs.getOffset(VertAttributes::TEX_COORDS);
    int tanOffset = attrs.getOffset(VertAttributes::TANGENT);

    auto &part = mesh->parts.at(meshPart);

    for (int i = 0; i < part.indices.size(); i += 3)
    {
        int vertI0 = part.indices[i], vertI1 = part.indices[i + 1], vertI2 = part.indices[i + 2];
        auto p0 = mesh->get<vec3>(vertI0, posOffset),
             p1 = mesh->get<vec3>(vertI1, posOffset),
             p2 = mesh->get<vec3>(vertI2, posOffset);
        auto uv0 = mesh->get<vec2>(vertI0, texOffset),
             uv1 = mesh->get<vec2>(vertI1, texOffset),
             uv2 = mesh->get<vec2>(vertI2, texOffset);
        auto tangent = calculateTangent(p0, p1, p2, uv0, uv1, uv2);
        mesh->add<vec3>(tangent, vertI0, tanOffset);
        mesh->add<vec3>(tangent, vertI1, tanOffset);
        mesh->add<vec3>(tangent, vertI2, tanOffset);
    }
    mesh->normalizeVecAttribute<vec3>(tanOffset);
}

} // namespace TangentCalculator
