#pragma once

#include "shared_3d.h"

#include "../../math/math_utils.h"

#include <memory>
#include <vector>
#include <unordered_map>

struct Bone
{
    SharedBone parent;
    std::vector<SharedBone> children;
    std::string name;
    vec3 translation = mu::ZERO_3;
    quat rotation = mu::NO_ROTATION;
    vec3 scale = mu::ONE_3;
    mat4 inverseBindMatrix;

    mat4 getBoneTransform(const vec3 *overrideTranslation= NULL, const quat *overrideRotation= NULL, const vec3 *overrideScale= NULL) const;
};

struct Armature
{
    struct Animation
    {

        struct Timeline
        {
            std::vector<float> times;
        };
        typedef std::shared_ptr<Timeline> SharedTimeline;

        struct PropertyValues
        {
            std::vector<vec3> vec3Values;
            std::vector<quat> quatValues;
        };
        typedef std::shared_ptr<PropertyValues> SharedPropertyValues;

        struct Channel
        {

            enum Interpolation
            {
                STEP, LINEAR, CUBICSPLINE
            };
            enum TargetProperty
            {
                TRANSLATION, ROTATION, SCALE
            };

            SharedBone target;
            TargetProperty targetProperty;
            SharedTimeline timeline;
            SharedPropertyValues propertyValues;
            Interpolation interpolation;
        };

        std::string name;
        std::vector<Channel> channels;
        float duration = 0;
    };

    std::string name;
    SharedBone root;
    std::vector<SharedBone> bones;
    std::unordered_map<std::string, Animation> animations;
};
