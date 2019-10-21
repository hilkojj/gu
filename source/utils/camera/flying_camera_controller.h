
#ifndef FLYING_CAMERA_CONTROLLER_H
#define FLYING_CAMERA_CONTROLLER_H

#include "../../graphics/camera.h"

class FlyingCameraController
{

  public:

    Camera *cam;

    float speedMultiplier = 1, mouseSensivity = 1;

    FlyingCameraController(Camera *cam);

    void update(double deltaTime);

};

#endif
