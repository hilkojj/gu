
#include <iostream>
#include <string>

#include "model_instance.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "../../utils/math_utils.h"

ModelInstance::ModelInstance(SharedModel model)
    : model(model)
{
    std::cout << "Creating ModelInstance of " << model->name << std::endl;
}

ModelInstance::~ModelInstance()
{
    std::cout << "Ending ModelInstance of " << model->name << std::endl;
}

void ModelInstance::translate(float x, float y, float z)
{
    translate(glm::vec3(x, y, z));
}

void ModelInstance::translate(glm::vec3 translation)
{
    transform = glm::translate(transform, translation);
}

void ModelInstance::scale(float s)
{
    scale(glm::vec3(s));
}

void ModelInstance::scale(float x, float y, float z)
{
    scale(glm::vec3(x, y, z));
}

void ModelInstance::scale(glm::vec3 s)
{
    transform = glm::scale(transform, s);
}

void ModelInstance::rotateRad(float radians, glm::vec3 axis)
{
    transform = glm::rotate(transform, radians, axis);
}

void ModelInstance::rotate(float degrees, glm::vec3 axis)
{
    rotateRad(degrees * mu::DEGREES_TO_RAD, axis);
}

void ModelInstance::rotateX(float degrees)
{
    rotate(degrees, mu::X);
}

void ModelInstance::rotateY(float degrees)
{
    rotate(degrees, mu::Y);
}

void ModelInstance::rotateZ(float degrees)
{
    rotate(degrees, mu::Y);
}
