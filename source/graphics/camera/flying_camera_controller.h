#pragma once

#include "camera.h"

class FlyingCameraController
{

  public:

    Camera *cam;

    float speedMultiplier = 1, mouseSensivity = 1;

    FlyingCameraController(Camera *cam);

    void update(double deltaTime);

};
