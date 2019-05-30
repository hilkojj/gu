
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
            mesh->setVec3(pos, vertI, posOffset);
            if (norOffset != -1) mesh->setVec3(pos / radius, vertI, norOffset);
            if (texOffset != -1) mesh->setVec2(vec2(lon / 360, lat / 180 + .5), vertI, texOffset);
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

    if (attrs.contains(VertAttributes::TANGENT))
    {
        int tanOffset = attrs.getOffset(VertAttributes::TANGENT);
        TangentCalculator::addTangentsToMesh(mesh);
        for (int y = 0; y < rings; y++)
        {
            int i0 = y, i1 = y + (segments * rings);
            vec3 tan = normalize(mesh->getVec3(i0, tanOffset) + mesh->getVec3(i1, tanOffset));
            mesh->setVec3(tan, i0, tanOffset);
            mesh->setVec3(tan, i1, tanOffset);
        }
    }
    return mesh;
}

} // namespace SphereMeshGenerator
