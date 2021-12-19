
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
#include "glm/gtc/quaternion.hpp"
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

const quat NO_ROTATION = quat(1, 0, 0, 0);

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

inline float distance(vec2 v, vec2 w)
{
    return length(v - w);
}

/**
 * calculates distance between a line segment (v, w) and p
 * https://stackoverflow.com/a/1501725/10386780
 */
inline float minimumDistance(vec2 v, vec2 w, vec2 p) {
    // Return minimum distance between line segment vw and point p
    const float l2 = length2(w - v);  // i.e. |w-v|^2 -  avoid a sqrt
    if (l2 == 0.0) return distance(p, v);   // v == w case
    // Consider the line extending the segment, parameterized as v + t (w - v).
    // We find projection of point p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    // We clamp t from [0,1] to handle points outside the segment vw.
    const float t = max<float>(0, min<float>(1, dot(p - v, w - v) / l2));
    const vec2 projection = v + t * (w - v);  // Projection falls on the segment
    return distance(p, projection);
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

/**
 * Calculates the 2 intersection points of 2 circles.
 * d must be <= r0 + r1
 *
 * @param i0 Intersection point 0   (will be set)
 * @param i1 Intersection point 1   (will be set)
 * @param c0 Center of circle 0
 * @param c1 Center of circle 1
 * @param r0 Radius of circle 0
 * @param r1 Radius of circle 1
 * @param d  Distance between circles
 */
inline void circleIntersections(
        vec2 &i0, vec2 &i1, const vec2 &c0, const vec2 &c1, float r0, float r1, float d
) {

    float
        r0Sq = r0 * r0,
        r1Sq = r1 * r1,
        dSq = d * d,

        a = (r0Sq - r1Sq + dSq) / (2 * d),
        h = sqrt(r0Sq - (a * a)),

        x = c0.x + a * (c1.x - c0.x) / d,
        y = c0.y + a * (c1.y - c0.y) / d;

    i0 = vec2(
        x + h * (c1.y - c0.y) / d,
        y - h * (c1.x - c0.x) / d
    );
    i1 = vec2(
        x - h * (c1.y - c0.y) / d,
        y + h * (c1.x - c0.x) / d
    );
}

template<typename vec>
inline void quadraticBezier(const vec &p0, const vec &p1, const vec &p2, float t, vec &out)
{
    // B2(t) = (1 - t) * (1 - t) * p0 + 2 * (1-t) * t * p1 + t*t*p2

    float dt = 1 - t;
    out = dt * dt * p0 + 2 * dt * t * p1 + t * t * p2;
}

template<typename vec>
inline void cubicBezier(const vec &p0, const vec &p1, const vec &p2, const vec &p3, float t, vec &out)
{
    // B3(t) = (1-t) * (1-t) * (1-t) * p0 + 3 * (1-t) * (1-t) * t * p1 + 3 * (1-t) * t * t * p2 + t * t * t * p3

    float dt = 1 - t;
    out = dt * dt * dt * p0 + 3 * dt * dt * t * p1 + 3 * dt * t * t * p2 + t * t * t * p3;
}



inline void floatColorToByteVec(const vec4 &rgba, u8vec4 &rgbaOut)
{
    rgbaOut = u8vec4(rgba * 255.f);
}

} // namespace mu

#endif
