
#ifndef ARMATURE_H
#define ARMATURE_H

#include <memory>
#include <vector>
#include <unordered_map>

#include "../../utils/math_utils.h"

struct Bone;
typedef std::shared_ptr<Bone> SharedBone;

struct Bone
{
    std::vector<SharedBone> children;
    std::string name;
    vec3 translation = vec3(0);
    quat rotation = quat();
    vec3 scale = vec3(1);
};

struct Armature
{
    std::string name;
    SharedBone root;
    std::unordered_map<std::string, SharedBone> bonesByName;
};

typedef std::shared_ptr<Armature> SharedArmature;

#endif
