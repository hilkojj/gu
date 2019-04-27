#include <iostream>

#include "glad/glad.h"
#include "../graphics/3d/vert_buffer.h"
#include "../graphics/3d/vert_attributes.h"
#include "../graphics/3d/mesh.h"
#include "../graphics/3d/model.h"
#include "../graphics/3d/model_instance.h"
#include "../game/screen.h"
#include "../game/game.h"
#include "../graphics/shader_program.h"
#include "../graphics/3d/perspective_camera.h"
#include "../utils/json_model_loader.h"
#include "../utils/camera/flying_camera_controller.h"
#include "../utils/math_utils.h"
#include "glm/glm.hpp"
using namespace glm;

class MeshScreen : public Screen
{

  public:
    ModelInstance *modelInstance;
    ShaderProgram shaderProgram;
    PerspectiveCamera cam;
    FlyingCameraController camController;
    float time;

    MeshScreen()
        : shaderProgram(ShaderProgram::fromFiles("NormalTestShader", "assets/shaders/test.vert", "assets/shaders/normaltest.frag")),
          cam(PerspectiveCamera(.1, 100, 1, 1, 75)), camController(&cam)
    {

        VertAttributes attrs;
        attrs.add(VertAttributes::POSITION);
        attrs.add(VertAttributes::NORMAL);

        SharedMesh mesh = SharedMesh(new Mesh("testMesh", 3, 3, attrs));

        mesh->vertices.insert(mesh->vertices.begin(), {-1.0f, -1.0f, 0.0f,
                                                       1.0f, -1.0f, 0.0f,
                                                       0.0f, 1.0f, 0.0f});

        mesh->indices.insert(mesh->indices.begin(), {0, 1, 2});

        SharedModel model = SharedModel(new Model("testModel"));
        model->parts.push_back({mesh});

        // VertBuffer *buffer = VertBuffer::with(attrs);
        // buffer->add(mesh)->upload(true);

        SharedModel loadedModel = JsonModelLoader::fromUbjsonFile(
            "assets/models/example.ubj", &attrs
        )[0];

        std::cout << loadedModel->parts[0].mesh->attributes << std::endl;

        VertBuffer *buffer = VertBuffer::with(loadedModel->parts[0].mesh->attributes);
        buffer->add(loadedModel->parts[0].mesh)->upload(true);        

        modelInstance = new ModelInstance(loadedModel);
        // modelInstance->scale(1, 3, 1);

        cam.position = glm::vec3(0, 0, 3);
        cam.lookAt(glm::vec3(0));
        cam.update();
    }

    void render(double deltaTime)
    {
        time += deltaTime;
        /* circular camera movement:
        cam.position.x = glm::sin(time) * 3;
        cam.position.y = 3;
        cam.position.z = glm::cos(time) * 3;
        cam.lookAt(vec3(0), -mu::Y);
        cam.update();
        */

        camController.update(deltaTime); // free camera movement

        glClearColor(.4, .3, .7, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glUseProgram(shaderProgram.getProgramId());
        GLuint mvpId = glGetUniformLocation(shaderProgram.getProgramId(), "MVP");

        glm::mat4 mvp = cam.combined * modelInstance->transform;

        glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);

        for (ModelPart part : modelInstance->model->parts)
            part.mesh->render();
    }

    void onResize()
    {
        cam.viewportWidth = Game::widthPixels;
        cam.viewportHeight = Game::heightPixels;
    }

    ~MeshScreen()
    {
        delete modelInstance;
    }
};
