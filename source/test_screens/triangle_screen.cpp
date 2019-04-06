#include <iostream>

#include "glad/glad.h"
#include "../game/screen.h"

class TriangleScreen : public Screen
{

  public:
    GLuint vaoId;
    GLuint vertBufferId;
    GLfloat verts[9] {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f
    };

    TriangleScreen()
    {
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);
        glGenBuffers(1, &vertBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    }

    void render(double deltaTime)
    {
        glClearColor(.1, .3, .7, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufferId);
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0
        );

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);
    }

};
