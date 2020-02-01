#ifndef GAME_ORTHOGRAPHIC_CAMERA_H
#define GAME_ORTHOGRAPHIC_CAMERA_H

#include "camera.h"

class OrthographicCamera : public Camera
{
  public:

    OrthographicCamera(float near_, float far_, float viewportWidth, float viewportHeight);

    void update() override;

    vec2 cursorTo2DWorldPos(int xAxis=0, int yAxis=1);

    vec2 worldPos2D(float viewportX, float viewportY, int xAxis=0, int yAxis=1);

};


#endif
