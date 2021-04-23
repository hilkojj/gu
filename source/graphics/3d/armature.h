
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

    mat4 getBoneSpaceTransform() const;
};

struct Armature
{
    struct Animation
    {

        struct KeyFrame
        {
            float keyTime;

            vec3 translation = vec3(0);
            quat rotation = quat();
            vec3 scale = vec3(1);
        };

        std::unordered_map<SharedBone, std::vector<KeyFrame>> keyFramesPerBone;
        float duration = 0.;
    };

    std::string name;
    SharedBone root;
    std::vector<SharedBone> bones;
    std::unordered_map<std::string, SharedBone> bonesByName;
    std::unordered_map<std::string, Animation> animations;
};

typedef std::shared_ptr<Armature> SharedArmature;

#endif
