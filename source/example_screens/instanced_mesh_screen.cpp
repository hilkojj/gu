#include <iostream>

#include "../graphics/external/gl_includes.h"
#include "../graphics/shader_program.h"
#include "../graphics/camera/perspective_camera.h"
#include "../graphics/camera/flying_camera_controller.h"
#include "../graphics/3d/mesh.h"
#include "../graphics/3d/vert_buffer.h"
#include "../graphics/3d/loaders/json_model_loader.h"
#include "../graphics/3d/renderers/debug_line_renderer.h"

#include "../gu/game_utils.h"
#include "../math/math_utils.h"

class InstancedMeshScreen : public Screen
{

  public:
    SharedMesh mesh;
    ShaderProgram shaderProgram;
    PerspectiveCamera cam;
    FlyingCameraController camController;
    float time;

    DebugLineRenderer lineRenderer;

    InstancedMeshScreen()
        : shaderProgram(ShaderProgram::fromFiles("NormalTestShader", "gu/assets/shaders/test.vert", "gu/assets/shaders/normaltest.frag")),
          cam(PerspectiveCamera(.1, 100, 1, 1, 75)), camController(&cam)
    {
        VertAttributes attrs;
        attrs.add(VertAttributes::POSITION);
        attrs.add(VertAttributes::NORMAL);

        mesh = JsonModelLoader::fromUbjsonFile(
            "assets/models/example.ubj", &attrs
        )[0]->parts[0].mesh;

        VertBuffer *buffer = VertBuffer::with(mesh->attributes);
        buffer->add(mesh)->upload(true);

        VertData instanceOffsets(VertAttributes().add_({"OFFSET", 3}), std::vector<unsigned char>(sizeof(float) * 3 * 20 * 20 * 20));
        int i = 0;
        for (int x = 0; x < 20; x++)
            for (int y = 0; y < 20; y++)
                for (int z = 0; z < 20; z++)
                    instanceOffsets.get<vec3>(i++, 0) = vec3(x * 2, y * 2, z * 2);

        buffer->uploadPerInstanceData(instanceOffsets);

        cam.position = glm::vec3(0, 0, 3);
        cam.lookAt(glm::vec3(0));
        cam.update();
    }

    void render(double deltaTime)
    {
        time += deltaTime;

        camController.speedMultiplier = 3;
        camController.update(deltaTime); // free camera movement

        glClearColor(.4, .3, .7, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        shaderProgram.use();
        GLuint mvpId = shaderProgram.location("MVP");
        glUniformMatrix4fv(mvpId, 1, GL_FALSE, &cam.combined[0][0]);

        mesh->renderInstances(20 * 20 * 20);

        glDisable(GL_BLEND);

        lineRenderer.projection = cam.combined;
        lineRenderer.line(glm::vec3(-300, 0, 0), glm::vec3(300, 0, 0), mu::X);
        lineRenderer.line(glm::vec3(0, -300, 0), glm::vec3(0, 300, 0), mu::Y);
        lineRenderer.line(glm::vec3(0, 0, -300), glm::vec3(0, 0, 300), mu::Z);
    }

    void onResize()
    {
        cam.viewportWidth = gu::pixelWidth;
        cam.viewportHeight = gu::pixelHeight;
    }

};
