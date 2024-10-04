#ifndef PERSPECTIVE_CAMERA
#define PERSPECTIVE_CAMERA

#include "camera.h"

class PerspectiveCamera : public Camera {

  public:

    PerspectiveCamera(float near_, float far_, float viewportWidth, float viewportHeight, float fOV);

    float fOV;

    void update() override;

    ~PerspectiveCamera() override = default;

  protected:
    bool isInViewPort(const vec4 &homogeneousCoordinates, const vec3 &h3Coordinates) const override;
};

#endif
