
#include "sphere_mesh_generator.h"
#include "math_utils.h"

SharedMesh mu::generateSphereMesh(const std::string &meshName, float radius, int segments, int rings, VertAttributes &attrs)
{
    const int nrOfVerts = (segments + 1) * rings;
    SharedMesh mesh = SharedMesh(new Mesh(meshName, nrOfVerts, attrs));
    auto &part = mesh->parts.emplace_back();
    part.indices.resize((segments * (rings - 1)) * 6);

    const int posOffset = attrs.getOffset(VertAttributes::POSITION);
    const int norOffset = attrs.contains(VertAttributes::NORMAL) ? attrs.getOffset(VertAttributes::NORMAL) : -1;
    const int tanOffset = attrs.contains(VertAttributes::TANGENT) ? attrs.getOffset(VertAttributes::TANGENT) : -1;
    const int texOffset = attrs.contains(VertAttributes::TEX_COORDS) ? attrs.getOffset(VertAttributes::TEX_COORDS) : -1;

    int vertI = 0;
    for (int x = 0; x <= segments; x++)
    {
        for (int y = 0; y < rings; y++)
        {
            const float lon = 360.0 * ((float)x / segments);
            const float lat = (y / (rings - 1.0)) * 180.0 - 90.0;
            vec3 pos = vec3(radius * cos(lon * mu::DEGREES_TO_RAD), radius * sin(lat * mu::DEGREES_TO_RAD), radius * sin(lon * mu::DEGREES_TO_RAD));
            const float m = cos(lat * mu::DEGREES_TO_RAD);
            pos.x *= m;
            pos.z *= m;
            mesh->get<vec3>(vertI, posOffset) = pos;
            if (norOffset != -1)
            {
                mesh->get<vec3>(vertI, norOffset) = pos / radius;
            }
            if (tanOffset != -1)
            {
                const vec3 tangent = vec3(cos(lon * mu::DEGREES_TO_RAD - .5 * mu::PI), 0, sin(lon * mu::DEGREES_TO_RAD - .5 * mu::PI));
                mesh->get<vec3>(vertI, tanOffset) = tangent;
            }
            if (texOffset != -1)
            {
                const vec2 uv = vec2(lon / 360.0f, lat / 180.0f + 0.5f);
                mesh->get<vec2>(vertI, texOffset) = uv;
            }
            vertI++;
        }
    }
    assert(nrOfVerts == vertI);
    int i = 0;
    for (int x = 0; x < segments; x++)
    {
        for (int y = 0; y < rings - 1; y++)
        {
            part.indices[i++] = y + (x * rings);
            part.indices[i++] = y + 1 + (x * rings);
            part.indices[i++] = y + ((x + 1) * rings);

            part.indices[i++] = y + ((x + 1) * rings);
            part.indices[i++] = y + (x * rings) + 1;
            part.indices[i++] = y + ((x + 1) * rings) + 1;
        }
    }
    assert(part.indices.size() == i);
    return mesh;
}
