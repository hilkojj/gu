
#include <iostream>

#include "mesh_instance.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "../../utils/math_utils.h"

MeshInstance::MeshInstance(SharedMesh mesh)
    : mesh(mesh)
{
    std::cout << "Creating MeshInstance of " << mesh->name << std::endl;
}

MeshInstance::~MeshInstance()
{
    std::cout << "Ending MeshInstance of " << mesh->name << std::endl;
}

void MeshInstance::translate(float x, float y, float z)
{
    translate(glm::vec3(x, y, z));
}

void MeshInstance::translate(glm::vec3 translation)
{
    transform = glm::translate(transform, translation);
}

void MeshInstance::scale(float s)
{
    scale(glm::vec3(s));
}

void MeshInstance::scale(float x, float y, float z)
{
    scale(glm::vec3(x, y, z));
}

void MeshInstance::scale(glm::vec3 s)
{
    transform = glm::scale(transform, s);
}

void MeshInstance::rotateRad(float radians, glm::vec3 axis)
{
    transform = glm::rotate(transform, radians, axis);
}

void MeshInstance::rotate(float degrees, glm::vec3 axis)
{
    rotateRad(degrees * mu::DEGREES_TO_RAD, axis);
}

void MeshInstance::rotateX(float degrees)
{
    rotate(degrees, mu::X);
}

void MeshInstance::rotateY(float degrees)
{
    rotate(degrees, mu::Y);
}

void MeshInstance::rotateZ(float degrees)
{
    rotate(degrees, mu::Y);
}
