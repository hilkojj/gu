#pragma once

#include "camera.h"

class OrthographicCamera : public Camera
{
  public:

    OrthographicCamera(float near_, float far_, float viewportWidth, float viewportHeight);

    void update() override;

    ~OrthographicCamera() override = default;
};
