#pragma once

#include "shared_3d.h"

#include "../textures/shared_texture.h"

#include "../../math/math_utils.h"

#include <vector>

/**
 *  This Material struct contains variables used for PBR rendering. (https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#materials)
 *
 *  It also contains some (non-PBR) variables for backwards compatibility (JsonModelLoader / the libgdx exporter for blender)
 */
struct Material
{
    std::string name;
    vec3 diffuse = mu::ZERO_3, ambient = mu::ZERO_3, emissive = mu::ZERO_3;
    float metallic = 0.0f, roughness = 0.5f;

    bool doubleSided = false;

    SharedTexture
        diffuseTexture,
        normalMap,
        metallicRoughnessTexture,
        aoTexture,   // or occlusionTexture
        emissiveTexture;

    float normalMapScale = 1.0f;
    float aoTextureStrength = 1.0f;

    // for backwards compatibility, or the specular extension for glTF/PBR.
    vec4 specular = vec4(1.0f);

#ifndef GU_PBR_ONLY
    // backwards compatibility, not PBR:
    vec3 reflection = vec3(1.0f);
    float shininess = 1.0f;
    SharedTexture specularMap;
#endif
};

struct ModelPart
{
    SharedMesh mesh;
    int meshPartIndex = 0;
    SharedMaterial material;

    SharedArmature armature;
};

class Model
{
  public:
    std::vector<ModelPart> parts;
    std::string name;

    Model(std::string name);

    ~Model();
};
