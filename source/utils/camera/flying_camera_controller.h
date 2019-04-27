
#ifndef FLYING_CAMERA_CONTROLLER_H
#define FLYING_CAMERA_CONTROLLER_H

#include "../../graphics/3d/perspective_camera.h"

class FlyingCameraController
{

  public:

    PerspectiveCamera *cam;

    float speedMultiplier = 1, mouseSensivity = 1;

    FlyingCameraController(PerspectiveCamera *cam);

    void update(double deltaTime);

};

#endif
