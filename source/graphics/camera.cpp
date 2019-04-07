
#include "camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace glm;

Camera::Camera(float near, float far, float viewportWidth, float viewportHeight)
    : direction(vec3(0, 0, -1)),
      up(vec3(0, 1, 0)),
      near(near), far(far),
      viewportWidth(viewportWidth), viewportHeight(viewportHeight)
{
}

void Camera::lookAt(vec3 target)
{
    view = glm::lookAt(
        position,
        target,
        up
    );
}
