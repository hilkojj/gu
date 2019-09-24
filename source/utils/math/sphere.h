#ifndef SPHERE_H
#define SPHERE_H

#include "glm/glm.hpp"
using namespace glm;

class Sphere
{
  public:
    float radius;
    glm::vec3 center;

    Sphere(float radius);

    Sphere(float radius, vec3 center);

    bool rayIntersection(const vec3 &rayOrigin, const vec3 &rayDirection, vec3 *p0, vec3 *p1=NULL) const;

};

#endif
