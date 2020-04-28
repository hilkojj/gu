
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdlib.h>
#include <string>
#include <functional>

#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace glm;

namespace mu
{

const vec3
    X = vec3(1, 0, 0),
    Y = vec3(0, 1, 0),
    Z = vec3(0, 0, 1),
    ZERO_3 = vec3(0);

const float

    PI = pi<float>(),

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
    if (max == 0) return 0;
    return std::rand() % max;
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

inline vec3 calculateNormal(const vec3 &p0, const vec3 &p1, const vec3 &p2)
{
    return normalize(cross(p1 - p0, p2 - p0));
}

// following 3 functions are 99% copied from: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/

// Given three collinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
inline bool onSegment(const vec2 &p, const vec2 &q, const vec2 &r)
{
    return q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
           q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y);

}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
inline int orientation(const vec2 &p, const vec2 &q, const vec2 &r)
{
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/
    // for details of below formula.
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // collinear

    return (val > 0)? 1: 2; // clock or counterclockwise
}

inline bool lineSegmentsIntersect(const vec2 &p1, const vec2 &q1, const vec2 &p2, const vec2 &q2)
{
    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

     // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false; // Doesn't fall in any of the above cases
}

inline float sign(const vec2 &p0, const vec2 &p1, const vec2 &p2)
{
    return (p0.x - p2.x) * (p1.y - p2.y) - (p1.x - p2.x) * (p0.y - p2.y);
}

inline bool pointInTriangle(const vec2 &point, const vec2 &p0, const vec2 &p1, const vec2 &p2)
{
    float d1 = sign(point, p0, p1),
        d2 = sign(point, p1, p2),
        d3 = sign(point, p2, p0);

    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

/**
 * Compute barycentric coordinates (out) for
 * point p with respect to triangle (a, b, c)
 *
 * taken from: https://gamedev.stackexchange.com/a/23745
 */
inline void barycentricCoords(const vec2 &p, const vec2 &a, const vec2 &b, const vec2 &c, vec3 &out)
{
    vec2 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    out.y = (d11 * d20 - d01 * d21) / denom;
    out.z = (d00 * d21 - d01 * d20) / denom;
    out.x = 1.0f - out.y - out.z;
}

/**
 * Executes 'callback' size^2 times in a spiral loop.
 * 'callback' must return true to continue, or false to beak the loop.
 * 
 * Example:
 * if size is 3 then 'callback' will be called 9 times with these positions:
 * (0, 0) (1, 0) (1, 1) (0, 1) (-1, 1) (-1, 0) (-1, -1) (0, -1) (1, -1)
 */
inline void spiral(int size, const std::function<bool(ivec2 pos)> &callback)
{
    ivec2 pos(0), dir(1, 0);
    int steps = 1, stepsToDo = 1, changeSteps = 2;
    for (int i = 0; i < size * size; i++)
    {
        if (!callback(pos)) break;

        pos += dir;
        if (--stepsToDo == 0)
        {
            dir = ivec2(-dir.y, dir.x);
            if (--changeSteps == 0)
            {
                changeSteps = 2;
                steps++;
            }
            stepsToDo = steps;
        }
    }
}

inline void loop3d(int size, const std::function<bool(int x, int y, int z)> &callback)
{
    for (int z = 0; z < size; z++)
        for (int y = 0; y < size; y++)
            for (int x = 0; x < size; x++)
                if (!callback(x, y, z)) return;
}

template <class vec>
inline bool allGreaterOrEqualTo(const vec &v, const double &val)
{
    for (int i = 0; i < vec::length(); i++)
        if (v[i] < val) return false;
    return true;
}

} // namespace mu

#endif
