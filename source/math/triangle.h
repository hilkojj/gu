#pragma once

#include "math_utils.h"

namespace mu::triangle
{

inline vec3 calculateNormal(const vec3 &a, const vec3 &b, const vec3 &c)
{
    return normalize(cross(b - a, c - a));
}

enum orientation
{
    collinear,
    clockwise,
    counterclockwise
};

inline orientation calculateOrientation(const vec2 &a, const vec2 &b, const vec2 &c, float collinearityThreshold = 0.00001f)
{
    const float slopeValue = (b.y - a.y) * (c.x - b.x) - (c.y - b.y) * (b.x - a.x);
    if (abs(slopeValue) < collinearityThreshold)
    {
        return collinear;
    }
    return slopeValue > 0.0f ? clockwise : counterclockwise;
}

inline bool isPointInside(const vec2 &point, const vec2 &a, const vec2 &b, const vec2 &c)
{
    auto sign = [] (const vec2 &p0, const vec2 &p1, const vec2 &p2)
    {
        return (p0.x - p2.x) * (p1.y - p2.y) - (p1.x - p2.x) * (p0.y - p2.y);
    };

    const vec3 d(
        sign(point, a, b),
        sign(point, b, c),
        sign(point, c, a)
    );
    return !(any(lessThan(d, mu::ZERO_3)) && any(greaterThan(d, mu::ZERO_3)));
}

inline vec3 calculateBarycentricCoords(const vec2 &point, const vec2 &a, const vec2 &b, const vec2 &c)
{
    const vec2 v0 = b - a;
    const vec2 v1 = c - a;
    const vec2 v2 = point - a;
    const float dot00 = dot(v0, v0);
    const float dot01 = dot(v0, v1);
    const float dot11 = dot(v1, v1);
    const float dot20 = dot(v2, v0);
    const float dot21 = dot(v2, v1);
    const float denominator = dot00 * dot11 - dot01 * dot01;

    vec3 out;
    out.y = (dot11 * dot20 - dot01 * dot21) / denominator;
    out.z = (dot00 * dot21 - dot01 * dot20) / denominator;
    out.x = 1.0f - out.y - out.z;
    return out;
}

}
