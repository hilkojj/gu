
#include "flying_camera_controller.h"
#include "../../input/key_input.h"

FlyingCameraController::FlyingCameraController(Camera *cam)
: cam(cam)
{
}

void FlyingCameraController::update(double deltaTime)
{
    if (KeyInput::pressed(GLFW_KEY_W))
        cam->position += cam->direction * glm::vec3(deltaTime);

    if (KeyInput::pressed(GLFW_KEY_S))
        cam->position += cam->direction * glm::vec3(-deltaTime);

    if (KeyInput::pressed(GLFW_KEY_D))
        cam->position += cam->right * glm::vec3(deltaTime);

    if (KeyInput::pressed(GLFW_KEY_A))
        cam->position += cam->right * glm::vec3(-deltaTime);

    cam->update();
}

