
#ifndef MODEL_INSTANCE_H
#define MODEL_INSTANCE_H

#include <vector>
#include <memory>

#include "model.h"
#include "glm/glm.hpp"

class ModelInstance
{
  public:
    SharedModel model;

    ModelInstance(SharedModel model);

    ~ModelInstance();

    glm::mat4 transform = glm::mat4(1.0f);

    void translate(float x, float y, float z);

    void translate(glm::vec3 translation);

    void scale(float s);

    void scale(float x, float y, float z);

    void scale(glm::vec3 s);

    // rotate 'degrees' around 'axis'
    void rotate(float degrees, glm::vec3 axis);

    // rotate arount X axis
    void rotateX(float degrees);

    // rotate arount Y axis
    void rotateY(float degrees);

    // rotate arount Z axis
    void rotateZ(float degrees);

    // rotate 'radians' around 'axis'
    void rotateRad(float radians, glm::vec3 axis);

};

#endif
