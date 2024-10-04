#include <iostream>

#include "../graphics/external/gl_includes.h"
#include "../graphics/3d/vert_attributes.h"
#include "../gu/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/camera/perspective_camera.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

class TriangleScreen : public Screen
{

  public:
    ShaderProgram shaderProgram;
    GLuint vaoId;
    GLuint vertBufferId, iboId;
    GLfloat verts[9] {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f
    };
    GLushort indices[3] = {0, 1, 2};
    PerspectiveCamera cam;
    float time;

    TriangleScreen() 
        : shaderProgram(ShaderProgram::fromFiles("Testshader", "assets/shaders/test.vert", "assets/shaders/test.frag")),
          cam(PerspectiveCamera(.1, 100, 1600, 900, 75))
    {
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);
        glGenBuffers(1, &vertBufferId);
        glGenBuffers(1, &iboId);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(
            0, // location of VertexPosition that is used in vertex shader. 'layout(location = 0)'
            3, // size. x, y, z
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            (void*)0 // offset
        );
        glEnableVertexAttribArray(0);
    }

    void render(double deltaTime)
    {
        time += deltaTime;
        // cam.position.x = glm::sin(time) * 3;
        // cam.position.y = 3;
        // cam.position.z = glm::cos(time) * 3;
        cam.position = glm::vec3(3, 3, 3);
        cam.lookAt(vec3(0));
        cam.update();

        GLuint mvpId = glGetUniformLocation(shaderProgram.id(), "MVP");
        
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        model = glm::rotate(model, 3.14f/2.f, glm::vec3(1, 0, 0));
        // model = glm::scale(model, glm::vec3(glm::sin(time)));
        glm::mat4 mvp = cam.combined * model;


        glClearColor(.1, .3, .7, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram.id());

        glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);

        // glDrawArrays(GL_TRIANGLES, 0, 3); // draw

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)0);

    }

};
