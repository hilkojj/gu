
#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <vector>

#include "../texture.h"
#include "../../asset_manager/asset.h"
#include "mesh.h"
#include "armature.h"

struct TextureAssetOrPtr
{
    asset<Texture> assetTex;
    SharedTexture sharedTex;

    Texture &get()
    {
        if (sharedTex)
            return *sharedTex.get();
        else
            return assetTex.get();
    }

    bool isSet() const
    {
        return assetTex.isSet() || !!sharedTex;
    }
};

/**
 *  This Material struct contains variables used for PBR rendering. (https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#materials)
 *
 *  It also contains some (non-PBR) variables for backwards compatibility (JsonModelLoader / the libgdx exporter for blender)
 */
struct Material
{
    std::string name;
    vec3 diffuse = mu::ZERO_3, ambient = mu::ZERO_3, emissive = mu::ZERO_3;
    float metallic = .0, roughness = .5;

    bool doubleSided = false;

    TextureAssetOrPtr
        diffuseTexture,
        normalMap,
        metallicRoughnessTexture,
        aoTexture,   // or occlusionTexture
        emissiveTexture;

    float normalMapScale = 1.0f;
    float aoTextureStrength = 1.0f; // occludedColor = lerp(color, color * <sampled occlusion
                                  // texture value>, <occlusion strength>)

    // for backwards compatibility, or the specular extension for glTF/PBR.
    vec4 specular = vec4(1.0f);

    // backwards compatibility, not PBR:
    vec3 reflection = vec3(1.0f);
    float shininess = 1.0f;
    TextureAssetOrPtr specularMap;
};

typedef std::shared_ptr<Material> SharedMaterial;

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

typedef std::shared_ptr<Model> SharedModel;

#endif
