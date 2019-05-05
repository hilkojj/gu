
#include "tangent_calculator.h"

vec2 delta(vec2 p0, vec2 p1)
{
    float minX = min(p0.x, p1.x), maxX = max(p0.x, p1.x);
    float minY = min(p0.y, p1.y), maxY = max(p0.y, p1.y);

    float deltaX = min(abs(minX - maxX), minX + 1 - maxX);
    float deltaY = min(abs(minY - maxY), minY + 1 - maxY);
    return vec2(deltaX, deltaY);
}

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

    vec2 deltaUv1 = delta(uv1, uv0);
    vec2 deltaUv2 = delta(uv2, uv0);

    float r = 1 / (deltaUv1.x * deltaUv2.y - deltaUv1.y * deltaUv2.x);
    vec3 tangent = vec3(deltaPos1 * deltaUv2.y) - vec3(deltaPos2 * deltaUv1.y);
    tangent *= r;
    return normalize(tangent);
}

void addTangentsToMesh(Mesh *mesh)
{
    VertAttributes &attrs = mesh->attributes;
    int posOffset = attrs.getOffset(VertAttributes::POSITION);
    int texOffset = attrs.getOffset(VertAttributes::TEX_COORDS);
    int tanOffset = attrs.getOffset(VertAttributes::TANGENT);

    for (int i = 0; i < mesh->nrOfIndices; i += 3)
    {
        int vertI0 = mesh->indices[i], vertI1 = mesh->indices[i + 1], vertI2 = mesh->indices[i + 2];
        auto p0 = mesh->getVec3(vertI0, posOffset),
             p1 = mesh->getVec3(vertI1, posOffset),
             p2 = mesh->getVec3(vertI2, posOffset);
        auto uv0 = mesh->getVec2(vertI0, texOffset),
             uv1 = mesh->getVec2(vertI1, texOffset),
             uv2 = mesh->getVec2(vertI2, texOffset);
        auto tangent = calculateTangent(p0, p1, p2, uv0, uv1, uv2);
        mesh->addVec3(tangent, vertI0, tanOffset);
        mesh->addVec3(tangent, vertI1, tanOffset);
        mesh->addVec3(tangent, vertI2, tanOffset);
    }
    mesh->normalizeVec3Attribute(tanOffset);
}

} // namespace TangentCalculator
