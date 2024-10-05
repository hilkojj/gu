
#include "tangent_calculator.h"

#include "mesh.h"

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
    const vec3 deltaPos1 = vec3(p1 - p0);
    const vec3 deltaPos2 = vec3(p2 - p0);

    const vec2 deltaUv1 = vec2(uv1 - uv0);
    const vec2 deltaUv2 = vec2(uv2 - uv0);

    const float r = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv1.y * deltaUv2.x);
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
    auto &part = mesh->parts.at(meshPart);

    const VertAttributes &attrs = mesh->attributes;
    const int posOffset = attrs.getOffset(VertAttributes::POSITION);
    const int texOffset = attrs.getOffset(VertAttributes::TEX_COORDS);
    int tanOffset = 0;


    if (attrs.contains(VertAttributes::TANGENT_AND_SIGN))
    {
        tanOffset = attrs.getOffset(VertAttributes::TANGENT_AND_SIGN);
        // set tangents to vec4(0, 0, 0, 1) first, to prevent NaN floats when adding in the next loop.
        for (int i = 0; i < part.indices.size(); i += 3)
        {
            const int vertI0 = part.indices[i], vertI1 = part.indices[i + 1], vertI2 = part.indices[i + 2];

            mesh->get<vec4>(vertI0, tanOffset) = vec4(0, 0, 0, 1);
            mesh->get<vec4>(vertI1, tanOffset) = vec4(0, 0, 0, 1);
            mesh->get<vec4>(vertI2, tanOffset) = vec4(0, 0, 0, 1);
        }
    }
    else
    {
        tanOffset = attrs.getOffset(VertAttributes::TANGENT);
        // set tangents to vec3(0) first, to prevent NaN floats when adding in the next loop.
        for (int i = 0; i < part.indices.size(); i += 3)
        {
            const int vertI0 = part.indices[i], vertI1 = part.indices[i + 1], vertI2 = part.indices[i + 2];

            mesh->get<vec3>(vertI0, tanOffset) = vec3(0);
            mesh->get<vec3>(vertI1, tanOffset) = vec3(0);
            mesh->get<vec3>(vertI2, tanOffset) = vec3(0);
        }
    }

    for (int i = 0; i < part.indices.size(); i += 3)
    {
        const int vertI0 = part.indices[i], vertI1 = part.indices[i + 1], vertI2 = part.indices[i + 2];
        auto p0 = mesh->get<vec3>(vertI0, posOffset),
             p1 = mesh->get<vec3>(vertI1, posOffset),
             p2 = mesh->get<vec3>(vertI2, posOffset);
        auto uv0 = mesh->get<vec2>(vertI0, texOffset),
             uv1 = mesh->get<vec2>(vertI1, texOffset),
             uv2 = mesh->get<vec2>(vertI2, texOffset);
        auto tangent = calculateTangent(p0, p1, p2, uv0, uv1, uv2);
        mesh->get<vec3>(vertI0, tanOffset) += tangent;
        mesh->get<vec3>(vertI1, tanOffset) += tangent;
        mesh->get<vec3>(vertI2, tanOffset) += tangent;
    }
    mesh->normalizeVecAttribute<vec3>(tanOffset);
}

} // namespace TangentCalculator
