#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
using namespace glm;

class Camera
{
  public:
    Camera(float near, float far, float viewportWidth, float viewportHeight);

    vec3 position, direction, up, right;

    mat4 projection, view, combined;

    float
        near,
        far,
        viewportWidth, viewportHeight;

    /**
     * sets 'direction', 'up' and 'right' so that the Camera looks at the 'target'. 
     * 'up' will be uppish in relation to the Y axis.
     */
    void lookAt(vec3 target);

    /**
     * sets 'direction', 'up' and 'right' so that the Camera looks at the 'target'. 
     * 'up' will be uppish in relation to 'localYAxis'.
     */
    void lookAt(vec3 target, vec3 localYAxis);

    virtual void update() = 0;
};

#endif
