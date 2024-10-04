#include "sphere.h"

bool Sphere::rayIntersection(const vec3 &rayOrigin, const vec3 &rayDirection, vec3 *p0, vec3 *p1) const
{
    // https://www.youtube.com/watch?v=HFPlKQGChpE
    const vec3 dir = normalize(rayDirection);
    const float t = dot(center - rayOrigin, dir);
    const vec3 p = rayOrigin + dir * vec3(t);

    const float y = length(center - p);

    if (y > radius)
    {
        return false;
    }

    const float x = sqrt((radius * radius) - (y * y));

    if (p0)
    {
        const vec3 point0 = rayOrigin + dir * vec3(t - x);

        p0->x = point0.x;
        p0->y = point0.y;
        p0->z = point0.z;
    }
    if (p1)
    {
        const vec3 point1 = rayOrigin + dir * vec3(t + x);

        p1->x = point1.x;
        p1->y = point1.y;
        p1->z = point1.z;
    }
    return true;
}

