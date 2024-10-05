
#ifndef GAME_GLTF_MODEL_LOADER_H
#define GAME_GLTF_MODEL_LOADER_H

#include "../vert_attributes.h"
#include "../shared_3d.h"
#include "../../textures/shared_texture.h"

class GltfModelLoader
{
  public:

    enum CalculateTangents
    {
        NEVER,
        IF_NOT_PROVIDED,
        ALWAYS
    };

    VertAttributes vertAttributes;
    CalculateTangents calculateTangents = ALWAYS;
    bool
        loadDiffuseTextures = true,
        loadNormalMaps = true,
        loadAOTextures = true,
        loadEmissiveTextures = true,
        loadMetallicRoughnessTextures = true,
        generateMipMaps = true;

    GLuint textureMagFilter = GL_LINEAR;
    GLuint textureMinFilter = GL_LINEAR_MIPMAP_LINEAR;

    explicit GltfModelLoader(const VertAttributes &);

    std::vector<SharedArmature> armatures;
    std::vector<SharedTexture> textures;
    std::vector<SharedMaterial> materials;
    std::vector<SharedModel> models;
    std::vector<SharedMesh> meshes;

    void fromASCIIFile(const char *path);

    void fromBinaryFile(const char *path);

};


#endif
