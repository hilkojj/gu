#include <iostream>

#include "../gl_includes.h"
#include "../graphics/3d/vert_buffer.h"
#include "../graphics/3d/vert_attributes.h"
#include "../graphics/3d/mesh.h"
#include "../graphics/3d/model.h"
#include "../graphics/3d/model_instance.h"
#include "../gu/screen.h"
#include "../gu/game_utils.h"
#include "../graphics/shader_program.h"
#include "../graphics/3d/perspective_camera.h"
#include "../utils/json_model_loader.h"
#include "../utils/camera/flying_camera_controller.h"
#include "../utils/math_utils.h"

#include "../graphics/3d/debug_line_renderer.h"

class MeshScreen : public Screen
{

  public:
    ModelInstance *modelInstance;
    ShaderProgram shaderProgram;
    PerspectiveCamera cam;
    FlyingCameraController camController;
    float time;

    DebugLineRenderer lineRenderer;

    MeshScreen()
        : shaderProgram(ShaderProgram::fromFiles("NormalTestShader", "assets/shaders/test.vert", "assets/shaders/normaltest.frag")),
          cam(PerspectiveCamera(.1, 100, 1, 1, 75)), camController(&cam)
    {

        VertAttributes attrs;
        attrs.add(VertAttributes::POSITION);
        attrs.add(VertAttributes::NORMAL);

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
        cam.lookAt(vec3(0), mu::Y);
        cam.update();
        */

        camController.speedMultiplier = 1;
        camController.update(deltaTime); // free camera movement

        glClearColor(.4, .3, .7, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        shaderProgram.use();
        GLuint mvpId = shaderProgram.location("MVP");

        glm::mat4 mvp = cam.combined * modelInstance->transform;

        glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);

        for (ModelPart part : modelInstance->model->parts)
            part.mesh->render();

        glDisable(GL_BLEND);

        lineRenderer.projection = cam.combined;
        lineRenderer.line(glm::vec3(-300, 0, 0), glm::vec3(300, 0, 0), mu::X);
        lineRenderer.line(glm::vec3(0, -300, 0), glm::vec3(0, 300, 0), mu::Y);
        lineRenderer.line(glm::vec3(0, 0, -300), glm::vec3(0, 0, 300), mu::Z);
    }

    void onResize()
    {
        cam.viewportWidth = gu::widthPixels;
        cam.viewportHeight = gu::heightPixels;
    }

    ~MeshScreen()
    {
        delete modelInstance;
    }
};
