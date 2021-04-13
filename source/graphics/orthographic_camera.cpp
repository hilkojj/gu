
#include <gu/game_utils.h>
#include "graphics/orthographic_camera.h"
#include "../input/mouse_input.h"

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

vec2 OrthographicCamera::cursorTo2DWorldPos(int xAxis, int yAxis)
{
    return worldPos2D(MouseInput::mouseX / gu::widthPixels, MouseInput::mouseY / gu::heightPixels, xAxis, yAxis);
}

vec2 OrthographicCamera::worldPos2D(float viewportX, float viewportY, int xAxis, int yAxis)
{
    vec2 p(position[xAxis], position[yAxis]);

    p.x += (viewportX * 2 - 1) * .5 * viewportWidth;
    p.y += (viewportY * -2 + 1) * .5 * viewportHeight;

    return p;
}

