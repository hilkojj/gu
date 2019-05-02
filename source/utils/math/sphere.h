#ifndef SPHERE_H
#define SPHERE_H

#include "glm/glm.hpp"

class Sphere
{
  public:
    float radius;
    glm::vec3 center;

    Sphere(float radius);

    Sphere(float radius, glm::vec3 center);

};

#endif
