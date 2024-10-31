#pragma once

#include "math_utils.h"
#include "triangle.h"

namespace mu::segment
{

bool segmentsIntersect(const vec2 &seg0Start, const vec2 &seg0End, const vec2 &seg1Start, const vec2 &seg1End);

template<typename vec>
inline vec projectOnLineSegment(const vec &point, const vec &a, const vec &b)
{
    const float segmentLength2 = length2(b - a);
    if (segmentLength2 == 0.0f)
    {
        return a;
    }
    const float projectTime = clamp(dot(point - a, b - a) / segmentLength2, 0.0f, 1.0f);
    return a + projectTime * (b - a);
}

// parameter order is different from previous function, for backwards compatibility.
template<typename vec>
inline float distanceBetweenSegmentAndPoint(const vec &a, const vec &b, const vec &point) {
    return distance(projectOnLineSegment(point, a, b), point);
}

}
