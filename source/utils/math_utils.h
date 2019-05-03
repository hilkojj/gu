
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdlib.h>
#include <string>

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

    DEGREES_TO_RAD = PI / 180.0f,
    RAD_TO_DEGREES = 180.0f / PI;


inline float random()
{
    return (float) std::rand() / RAND_MAX;
}

inline float random(float max)
{
    return random() * max;
}


inline float random(float min, float max)
{
    return random(max - min) + min;
}

inline int randomInt(int max)
{
    return std::rand() % max;
}

inline int randomInt(int min, int max)
{
    return randomInt(max - min) + min;
}

inline glm::vec3 calculateNormal(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2)
{
    return glm::normalize(glm::cross(p1 - p0, p2 - p0));
}

inline std::string toString(glm::vec3 &p)
{
    return "(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ", " + std::to_string(p.z) + ")";
}

inline std::string toString(glm::vec2 &p)
{
    return "(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")";
}

} // namespace mu

#endif
