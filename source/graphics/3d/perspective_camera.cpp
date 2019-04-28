
#include "perspective_camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace glm;

PerspectiveCamera::PerspectiveCamera(float near_, float far_, float viewportWidth, float viewportHeight, float fOV)
    
    : Camera(near_, far_, viewportWidth, viewportHeight),
      fOV(fOV)
{
}

void PerspectiveCamera::update()
{
    projection = glm::perspective(glm::radians(fOV), viewportWidth / viewportHeight, near_, far_);
    view = glm::lookAt(
        position,
        position + direction,
        up
    );
    combined = projection * view;
}
