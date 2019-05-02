
#include "sphere.h"

Sphere::Sphere(float radius) : radius(radius), center(glm::vec3(0))
{}

Sphere::Sphere(float radius, glm::vec3 center) : radius(radius), center(center)
{}

