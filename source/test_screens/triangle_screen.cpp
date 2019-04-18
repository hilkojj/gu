#include <iostream>

#include "glad/glad.h"
#include "../graphics/3d/vert_attributes.h"
#include "../game/screen.h"
#include "../game/game.h"
#include "../graphics/shader_program.h"
#include "../graphics/3d/perspective_camera.h"
#include "glm/glm.hpp"
using namespace glm;

class TriangleScreen : public Screen
{

  public:
    ShaderProgram shaderProgram;
    GLuint vaoId;
    GLuint vertBufferId;
    GLfloat verts[9] {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f
    };
    PerspectiveCamera cam;
    float time;

    TriangleScreen() 
        : shaderProgram(ShaderProgram::fromFiles("Testshader", "assets/shaders/test.vert", "assets/shaders/test.frag")),
          cam(PerspectiveCamera(.1, 100, 1600, 900, 75))
    {
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);
        glGenBuffers(1, &vertBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        glVertexAttribPointer(
            0, // location of VertexPosition that is used in vertex shader. 'layout(location = 0)'
            3, // size. x, y, z
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            (void*)0 // offset
        );
    }

    void render(double deltaTime)
    {
        time += deltaTime;
        cam.position.x = glm::sin(time) * 3;
        cam.position.y = 3;
        cam.position.z = glm::cos(time) * 3;
        cam.lookAt(vec3(0));
        cam.update();

        GLuint mvpId = glGetUniformLocation(shaderProgram.getProgramId(), "MVP");
        
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 mvp = cam.combined * model;


        glClearColor(.1, .3, .7, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram.getProgramId());

        glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);

        // enable & bind vertex positions
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufferId);    // bind VBO
        // end

        glDrawArrays(GL_TRIANGLES, 0, 3); // draw

        glDisableVertexAttribArray(0); // disable vertex positions
    }

};
