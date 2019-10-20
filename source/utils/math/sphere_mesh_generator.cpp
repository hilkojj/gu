
#include "sphere_mesh_generator.h"
#include "glm/glm.hpp"
#include "../math_utils.h"
#include "../../graphics/3d/tangent_calculator.h"

using namespace glm;

namespace SphereMeshGenerator
{

SharedMesh generate(const std::string &meshName, float radius, int segments, int rings, VertAttributes &attrs)
{
    int nrOfVerts = (segments + 1) * rings;
    SharedMesh mesh = SharedMesh(new Mesh(meshName, nrOfVerts, (segments * (rings - 1)) * 6, attrs));

    int posOffset = attrs.getOffset(VertAttributes::POSITION);
    int norOffset = attrs.contains(VertAttributes::NORMAL) ? attrs.getOffset(VertAttributes::NORMAL) : -1;
    int tanOffset = attrs.contains(VertAttributes::TANGENT) ? attrs.getOffset(VertAttributes::TANGENT) : -1;
    int texOffset = attrs.contains(VertAttributes::TEX_COORDS) ? attrs.getOffset(VertAttributes::TEX_COORDS) : -1;

    int vertI = 0;
    for (int x = 0; x <= segments; x++)
    {
        for (int y = 0; y < rings; y++)
        {
            float lon = 360.0 * ((float)x / segments);
            float lat = (y / (rings - 1.0)) * 180.0 - 90.0;
            vec3 pos = vec3(radius * cos(lon * mu::DEGREES_TO_RAD), radius * sin(lat * mu::DEGREES_TO_RAD), radius * sin(lon * mu::DEGREES_TO_RAD));
            float m = cos(lat * mu::DEGREES_TO_RAD);
            pos.x *= m;
            pos.z *= m;
            mesh->set<vec3>(pos, vertI, posOffset);
            if (norOffset != -1) mesh->set<vec3>(pos / radius, vertI, norOffset);
            if (tanOffset != -1)
            {
                vec3 tan = vec3(cos(lon * mu::DEGREES_TO_RAD - .5 * mu::PI), 0, sin(lon * mu::DEGREES_TO_RAD - .5 * mu::PI));
                mesh->set<vec3>(tan, vertI, tanOffset);
            }
            if (texOffset != -1) mesh->set<vec2>(vec2(lon / 360, lat / 180 + .5), vertI, texOffset);
            vertI++;
        }
    }
    assert(nrOfVerts == vertI);
    int i = 0;
    for (int x = 0; x < segments; x++)
    {
        for (int y = 0; y < rings - 1; y++)
        {
            mesh->indices[i++] = y + (x * rings);
            mesh->indices[i++] = y + 1 + (x * rings);
            mesh->indices[i++] = y + ((x + 1) * rings);

            mesh->indices[i++] = y + ((x + 1) * rings);
            mesh->indices[i++] = y + (x * rings) + 1;
            mesh->indices[i++] = y + ((x + 1) * rings) + 1;
        }
    }
    assert(mesh->nrOfIndices == i);
    return mesh;
}

} // namespace SphereMeshGenerator
