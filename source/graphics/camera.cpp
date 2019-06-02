#include "camera.h"
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../utils/math_utils.h"
#include "../input/mouse_input.h"
using namespace glm;

Camera::Camera(float near_, float far_, float viewportWidth, float viewportHeight)
    : direction(vec3(0, 0, -1)),
      up(vec3(0, 1, 0)),
      right(vec3(1, 0, 0)),
      near_(near_), far_(far_),
      viewportWidth(viewportWidth), viewportHeight(viewportHeight)
{
}

void Camera::lookAt(vec3 target)
{
    lookAt(target, mu::Y);
}

void Camera::lookAt(vec3 target, vec3 localYAxis)
{
    direction = glm::normalize(target - position);
    right = glm::normalize(glm::cross(direction, localYAxis));
    up = glm::normalize(glm::cross(right, direction));
}

void Camera::rotate(float degrees, vec3 axis)
{
    float rads = degrees * mu::DEGREES_TO_RAD;
    direction = glm::rotate(direction, rads, axis);
    right = glm::rotate(right, rads, axis);
    up = glm::rotate(up, rads, axis);
}

vec3 Camera::getRayDirection(float viewportX, float viewportY) const
{
    vec2 point = vec2(viewportX / viewportWidth, 1 - viewportY / viewportHeight) * vec2(2) - vec2(1);

    vec4 eyeCoords = inverse(projection) * vec4(point.x, point.y, -1, 1);
    eyeCoords = vec4(eyeCoords.x, eyeCoords.y, eyeCoords.z, 0);
    vec3 worldCoords = inverse(view) * eyeCoords;

    return worldCoords;
}

vec3 Camera::getCursorRayDirection() const
{
    return getRayDirection(MouseInput::mouseX, MouseInput::mouseY);
}

vec3 Camera::project(const vec3 &p, bool &inViewport) const
{
    vec4 homogeneous = combined * vec4(p, 1);
    vec3 h3 = homogeneous / homogeneous.w;
    if (!inViewport && homogeneous.z >= 0 && all(lessThanEqual(h3, vec3(1))) && all(greaterThanEqual(h3, vec3(-1))))
        inViewport = true;
    return h3;
}

bool blah = true;

vec3 Camera::project(const vec3 &p) const
{
    return project(p, blah);
}

vec3 Camera::projectPixels(const vec3 &p, bool &inViewport) const
{
    return (project(p, inViewport) + vec3(1, -1, 0)) * vec3(.5, -.5, 1) * vec3(viewportWidth, viewportHeight, 1);
}

vec3 Camera::projectPixels(const vec3 &p) const
{
    return projectPixels(p, blah);
}
