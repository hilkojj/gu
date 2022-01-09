
#include "armature.h"

mat4 Bone::getBoneTransform(const vec3 *overrideTranslation, const quat *overrideRotation, const vec3 *overrideScale) const
{
    mat4 t = glm::translate(mat4(1), overrideTranslation ? *overrideTranslation : translation);
    t *= glm::toMat4(overrideRotation ? *overrideRotation : rotation);
    t = glm::scale(t, overrideScale ? *overrideScale : scale);
    return t;
}
