
#include "graphics/orthographic_camera.h"

OrthographicCamera::OrthographicCamera(float near_, float far_, float viewportWidth, float viewportHeight)
    : Camera(near_, far_, viewportWidth, viewportHeight)
{
}

void OrthographicCamera::update()
{
    projection = glm::ortho<float>(viewportWidth * -.5, viewportWidth * .5, viewportHeight * -.5, viewportHeight * .5, near_, far_);
    view = glm::lookAt(
        position,
        position + direction,
        up
    );
    combined = projection * view;
}
