#ifndef GAME_ORTHOGRAPHIC_CAMERA_H
#define GAME_ORTHOGRAPHIC_CAMERA_H

#include "camera.h"

class OrthographicCamera : public Camera
{
  public:

    OrthographicCamera(float near_, float far_, float viewportWidth, float viewportHeight);

    void update() override;

    ~OrthographicCamera() override = default;
};


#endif
