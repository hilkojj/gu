
#include "armature.h"

mat4 Bone::getBoneSpaceTransform() const
{
    mat4 t = glm::translate(mat4(1), translation);
    t *= glm::toMat4(rotation);
    t = glm::scale(t, scale);
    return t;
}
