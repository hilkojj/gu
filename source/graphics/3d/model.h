
#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <vector>

#include "mesh.h"

struct ModelPart
{
    SharedMesh mesh;
    // todo: SharedMaterial material;
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
