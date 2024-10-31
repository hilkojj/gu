#pragma once

#include "math_utils.h"

struct Sphere
{
    float radius = 1.0f;
    vec3 center = vec3(0.0f);

    bool rayIntersection(const vec3 &rayOrigin, const vec3 &rayDirection, vec3 *p0, vec3 *p1 = nullptr) const;
};
