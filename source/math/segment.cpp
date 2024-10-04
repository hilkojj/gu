
#include "segment.h"

bool mu::segment::segmentsIntersect(const vec2 &seg0Start, const vec2 &seg0End, const vec2 &seg1Start, const vec2 &seg1End)
{
    const triangle::orientation orientation0 = triangle::calculateOrientation(seg0Start, seg0End, seg1Start);
    const triangle::orientation orientation1 = triangle::calculateOrientation(seg0Start, seg0End, seg1End);
    const triangle::orientation orientation2 = triangle::calculateOrientation(seg1Start, seg1End, seg0Start);
    const triangle::orientation orientation3 = triangle::calculateOrientation(seg1Start, seg1End, seg0End);

    if (orientation0 != orientation1 && orientation2 != orientation3)
    {
        return true;
    }

    auto onSegment = [] (const vec2 &point, const vec2 &segmentStart, const vec2 &segmentEnd)
    {
        return segmentStart.x <= max(point.x, segmentEnd.x) && segmentStart.x >= min(point.x, segmentEnd.x) &&
            segmentStart.y <= max(point.y, segmentEnd.y) && segmentStart.y >= min(point.y, segmentEnd.y);
    };

    if (orientation0 == triangle::collinear && onSegment(seg0Start, seg1Start, seg0End))
    {
        return true;
    }
    if (orientation1 == triangle::collinear && onSegment(seg0Start, seg1End, seg0End))
    {
        return true;
    }
    if (orientation2 == triangle::collinear && onSegment(seg1Start, seg0Start, seg1End))
    {
        return true;
    }
    if (orientation3 == triangle::collinear && onSegment(seg1Start, seg0End, seg1End))
    {
        return true;
    }
    return false;
}
