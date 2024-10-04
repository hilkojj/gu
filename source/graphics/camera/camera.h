#ifndef CAMERA_H
#define CAMERA_H

#include "../../math/math_utils.h"

class Camera
{
  public:
    Camera(float near_, float far_, float viewportWidth, float viewportHeight);

    vec3 position, direction, up, right;

    mat4 projection, view, combined;

    float 
        near_, far_, // 'near' and 'far' are reserved keywords on wInDOwS, thats why they are called 'near_' and 'far_'
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

    void rotate(float degrees, vec3 axis);

    virtual void update() = 0;

    vec3 getCursorRayDirection() const;

    vec3 getRayDirection(float viewportX, float viewportY) const;

    /**
     * returns the point in 'normalized device space'.
     * If bInViewport points to a bool it will be set to if the point is inside the viewport.
     */
    vec3 project(const vec3 &p, bool *bInViewport = nullptr) const;

    /**
     * returns the point in 'viewport space'.
     * IF inViewport is originally set to false then inViewport will be set to true if the point is inside the viewport.
     */
    vec3 projectPixels(const vec3 &p, bool *bInViewport = nullptr) const;

    virtual ~Camera() = default;

  protected:
    virtual bool isInViewPort(const vec4 &homogeneousCoordinates, const vec3 &h3Coordinates) const;
};

#endif
