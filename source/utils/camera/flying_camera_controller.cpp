
#include "../../gu/game_utils.h"
#include "flying_camera_controller.h"
#include "../../input/key_input.h"
#include "../../input/mouse_input.h"
#include "../../utils/math_utils.h"

FlyingCameraController::FlyingCameraController(Camera *cam)
: cam(cam)
{
    MouseInput::setLockedMode(true);
}

void FlyingCameraController::update(double deltaTime)
{
    speedMultiplier += MouseInput::yScroll * .5;
    if (speedMultiplier < 0) speedMultiplier = 0;

    if (KeyInput::pressed(GLFW_KEY_W))
        cam->position += cam->direction * glm::vec3(deltaTime * speedMultiplier);

    if (KeyInput::pressed(GLFW_KEY_S))
        cam->position += cam->direction * glm::vec3(-deltaTime * speedMultiplier);

    if (KeyInput::pressed(GLFW_KEY_D))
        cam->position += cam->right * glm::vec3(deltaTime * speedMultiplier);

    if (KeyInput::pressed(GLFW_KEY_A))
        cam->position += cam->right * glm::vec3(-deltaTime * speedMultiplier);

    if (KeyInput::pressed(GLFW_KEY_LEFT_SHIFT))
        cam->position.y -= deltaTime * speedMultiplier;

    if (KeyInput::pressed(GLFW_KEY_SPACE))
        cam->position.y += deltaTime * speedMultiplier;

    if (MouseInput::deltaMouseX != 0)
        cam->rotate(MouseInput::deltaMouseX / gu::width * -100 * mouseSensivity, mu::Y);

    if (MouseInput::deltaMouseY != 0)
        cam->rotate(MouseInput::deltaMouseY / gu::height * -100 * mouseSensivity, cam->right);
    

    cam->update();
}

