
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdlib.h>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
using namespace glm;

namespace mu
{

const vec3
    X = vec3(1, 0, 0),
    Y = vec3(0, 1, 0),
    Z = vec3(0, 0, 1);

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

inline vec3 calculateNormal(const vec3 &p0, const vec3 &p1, const vec3 &p2)
{
    return normalize(cross(p1 - p0, p2 - p0));
}

inline std::string toString(const vec3 &p)
{
    return "(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ", " + std::to_string(p.z) + ")";
}

inline std::string toString(const vec2 &p)
{
    return "(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")";
}

// following 3 functions are 99% copied from: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/

// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
inline bool onSegment(const vec2 &p, const vec2 &q, const vec2 &r)
{
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
       return true;

    return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
inline int orientation(const vec2 &p, const vec2 &q, const vec2 &r)
{
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/
    // for details of below formula.
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // colinear

    return (val > 0)? 1: 2; // clock or counterclock wise
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
    // p1, q1 and p2 are colinear and p2 lies on segment p1q
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // p1, q1 and q2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

     // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false; // Doesn't fall in any of the above cases
}

} // namespace mu

#endif
