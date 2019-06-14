
#include "polygon.h"

Polygon::Polygon()
{}
Polygon::Polygon(int points) : points(points)
{}

bool Polygon::contains(float x, float y) const
{

    int intersections = 0;
    int size = points.size();

    for (int i = 0; i < size; i++)
    {
        const vec2 &p1 = points[i], &p2 = points[(i + 1) % size];
        
        if (((p1.y <= y && y < p2.y) || (p2.y <= y && y < p1.y)) && x < ((p2.x - p1.x) / (p2.y - p1.y) * (y - p1.y) + p1.x))
            intersections++;
    }
    return (intersections & 1) == 1;
}

std::string Polygon::toString() const
{
    std::string s = "";
    bool first = true;
    for (const vec2 &p : points)
    {
        if (!first) s += ", ";
        first = false;
        s += to_string(p);
    }
    return s;
}
