
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

namespace mu
{

const glm::vec3
    X = glm::vec3(1, 0, 0),
    Y = glm::vec3(0, 1, 0),
    Z = glm::vec3(0, 0, 1);

const float

    PI = glm::pi<float>(),

    DEGREES_TO_RAD = PI / 180.0f;

} // namespace mu

#endif
