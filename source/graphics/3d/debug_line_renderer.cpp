#include <string>
#include "debug_line_renderer.h"

namespace
{
#define MULTILINE(...) #__VA_ARGS__

static std::string vertSource = MULTILINE(

    layout(location = 0) in float nr;

    uniform mat4 MVP;
    uniform vec3 p0, p1;

    void main()
    {
        gl_Position = MVP * vec4(nr == 0 ? p0 : p1, 1);
    }

);

static std::string fragSource = MULTILINE(

    uniform vec3 u_color;
    out vec3 color;

    void main()
    {
        color = u_color;
    }

);

} // namespace

DebugLineRenderer::DebugLineRenderer()
    : shaderProgram("DebugLineShader", ("#version 330 core\n" + vertSource).c_str(), ("#version 330 core\n" + fragSource).c_str())
{
    VertAttributes attrs;
    attrs.add({"nr", 1, GL_FALSE});
    lineMesh = SharedMesh(new Mesh("debug_line", 2, 2, attrs));
    lineMesh->vertices[0] = 0;
    lineMesh->vertices[1] = 1;
    lineMesh->indices[0] = 0;
    lineMesh->indices[1] = 1;
    lineMesh->mode = GL_LINES;
    VertBuffer::uploadSingleMesh(lineMesh);

    u_colorId = shaderProgram.location("u_color");
    MVPId = shaderProgram.location("MVP");
    p0Id = shaderProgram.location("p0");
    p1Id = shaderProgram.location("p1");
}

void DebugLineRenderer::line(glm::vec3 p0, glm::vec3 p1, glm::vec3 color)
{
    shaderProgram.use();
    glUniform3f(u_colorId, color.r, color.g, color.b);
    glUniform3f(p0Id, p0.x, p0.y, p0.z);
    glUniform3f(p1Id, p1.x, p1.y, p1.z);
    glUniformMatrix4fv(MVPId, 1, GL_FALSE, &projection[0][0]);
    lineMesh->render();
}
