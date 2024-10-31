#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
using namespace glm;

namespace mu
{

const vec3
    X = vec3(1, 0, 0),
    Y = vec3(0, 1, 0),
    Z = vec3(0, 0, 1),
    ZERO_3 = vec3(0),
    ONE_3 = vec3(1);

const float

    PI = pi<float>(),

    DEGREES_TO_RAD = PI / 180.0f,
    RAD_TO_DEGREES = 180.0f / PI;

const quat NO_ROTATION = identity<quat>();

inline float random()
{
    return float(std::rand() / RAND_MAX);
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
    return std::rand() % (max + 1);
}

inline int randomInt(int min, int max)
{
    return randomInt(max - min) + min;
}

inline float randomFromX(float x)
{
    return fract(sin(x) * 100000.0);
}

inline int randomIntFromX(float x, int max)
{
    if (max == 0) return 0;
    return int(randomFromX(x) * 1000) % max;
}

template<typename type>
inline type map(type value, type min1, type max1, type min2, type max2)
{
    const float x = (value - min1) / (max1 - min1);
    return x * (max2 - min2) + min2;
}

inline bool findCircleIntersections(
    const vec2 &circle0Pos, const vec2 &circle1Pos, const float circle0Radius, const float circle1Radius,
    vec2 &outIntersection0, vec2 &outIntersection1
)
{
    const float circleDistance = distance(circle0Pos, circle1Pos);
    if (circleDistance == 0.0f || circleDistance > circle0Radius + circle1Radius)
    {
        return false;
    }

    const float
        circle0RadiusSq = circle0Radius * circle0Radius,
        circle1RadiusSq = circle1Radius * circle1Radius,
        circleDistanceSq = circleDistance * circleDistance,

        a = (circle0RadiusSq - circle1RadiusSq + circleDistanceSq) / (2 * circleDistance),
        h = sqrt(circle0RadiusSq - (a * a)),

        x = circle0Pos.x + a * (circle1Pos.x - circle0Pos.x) / circleDistance,
        y = circle0Pos.y + a * (circle1Pos.y - circle0Pos.y) / circleDistance;

    outIntersection0 = vec2(
        x + h * (circle1Pos.y - circle0Pos.y) / circleDistance,
        y - h * (circle1Pos.x - circle0Pos.x) / circleDistance
    );
    outIntersection1 = vec2(
        x - h * (circle1Pos.y - circle0Pos.y) / circleDistance,
        y + h * (circle1Pos.x - circle0Pos.x) / circleDistance
    );
    return true;
}

}
