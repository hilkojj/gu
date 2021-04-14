
#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <vector>

#include "../texture.h"
#include "../../asset_manager/asset.h"
#include "mesh.h"

struct Material
{
    std::string name;
    vec3 diffuse, ambient, emissive, reflection;
    vec4 specular;
    float shininess;

    asset<Texture> diffuseTexture, specularMap, normalMap;
};

typedef std::shared_ptr<Material> SharedMaterial;

struct ModelPart
{
    SharedMesh mesh;
    int meshPartIndex = 0;
    SharedMaterial material;
    // todo: SharedArmature armature = nullptr;
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
