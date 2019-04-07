#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
using namespace glm;

class Camera
{
  public:
    Camera(float near, float far, float viewportWidth, float viewportHeight);

    vec3 position, direction, up;

    mat4 projection, view, combined;

    float
        near,
        far,
        viewportWidth, viewportHeight;

    void lookAt(vec3 position);

    virtual void update() = 0;
};

#endif
