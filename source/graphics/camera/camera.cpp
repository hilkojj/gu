
#include "camera.h"

#include "../../math/math_utils.h"
#include "../../input/mouse_input.h"
#include "../../gu/game_utils.h"

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
    return getRayDirection(MouseInput::mouseX / gu::widthPixels * viewportWidth,
        MouseInput::mouseY / gu::heightPixels * viewportHeight);
}

vec3 Camera::project(const vec3 &p, bool *bInViewport) const
{
    vec4 homogeneous = combined * vec4(p, 1);
    vec3 h3 = homogeneous / homogeneous.w;
    if (bInViewport != nullptr && isInViewPort(homogeneous, h3))
        *bInViewport = true;
    return h3;
}

vec3 Camera::projectPixels(const vec3 &p, bool *bInViewport) const
{
    return (project(p, bInViewport) + vec3(1, -1, 0)) * vec3(.5, -.5, 1) * vec3(viewportWidth, viewportHeight, 1);
}

bool Camera::isInViewPort(const vec4 &homogeneousCoordinates, const vec3 &h3Coordinates) const
{
    return all(lessThanEqual(h3Coordinates, vec3(1))) && all(greaterThanEqual(h3Coordinates, vec3(-1)));
}
