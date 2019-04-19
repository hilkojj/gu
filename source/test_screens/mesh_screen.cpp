#include <iostream>

#include "glad/glad.h"
#include "../graphics/3d/vert_attributes.h"
#include "../graphics/3d/mesh.h"
#include "../graphics/3d/mesh_instance.h"
#include "../game/screen.h"
#include "../game/game.h"
#include "../graphics/shader_program.h"
#include "../graphics/3d/perspective_camera.h"
#include "glm/glm.hpp"
using namespace glm;

class MeshScreen : public Screen
{

  public:
    MeshInstance* meshInstance;
    ShaderProgram shaderProgram;
    PerspectiveCamera cam;
    float time;

    MeshScreen() 
        : shaderProgram(ShaderProgram::fromFiles("Testshader", "assets/shaders/test.vert", "assets/shaders/test.frag")),
          cam(PerspectiveCamera(.1, 100, 1600, 900, 75))
    {

        VertAttributes attrs;
        attrs.add(VertAttributes::POSITION);
        attrs.add(VertAttributes::NORMAL);

        std::cout << attrs.getVertSize() << std::endl;

        SharedMesh mesh = SharedMesh(new Mesh("testMesh", 3, 3, attrs));

        mesh->vertices[0] = 4;

        std::cout << mesh->vertices[0] << std::endl;

        meshInstance = new MeshInstance(mesh);

    }

    void render(double deltaTime)
    {
        time += deltaTime;
        cam.position.x = glm::sin(time) * 3;
        cam.position.y = 3;
        cam.position.z = glm::cos(time) * 3;
        cam.lookAt(vec3(0));
        cam.update();

    }

    ~MeshScreen()
    {
        delete meshInstance;
    }

};
