
#include "perspective_camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace glm;

PerspectiveCamera::PerspectiveCamera(float near, float far, float viewportWidth, float viewportHeight, float fOV)
    
    : Camera(near, far, viewportWidth, viewportHeight),
      fOV(fOV)
{
}

void PerspectiveCamera::update()
{
    projection = glm::perspective(glm::radians(fOV), viewportWidth / viewportHeight, near, far);
    combined = projection * view;
}
