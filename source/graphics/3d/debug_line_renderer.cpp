#include <string>
#include "debug_line_renderer.h"

namespace
{
#define MULTILINE(...) #__VA_ARGS__

static const char *vertSource = MULTILINE(

    layout(location = 0) in int nr;

    uniform mat4 MVP;
    uniform vec3 p0, p1;

    void main()
    {
        gl_Position = MVP * vec4(nr == 0 ? p0 : p1, 1);
    }

);

static const char *fragSource = MULTILINE(

    precision mediump float;

    uniform vec3 u_color;
    out vec4 color;

    void main()
    {
        color = vec4(u_color, 1.0);
        // We must explicitly set alpha to 1.0 in WebGL, otherwise the screen becomes black.
    }

);

} // namespace

DebugLineRenderer::DebugLineRenderer()
    : shaderProgram("DebugLineShader", vertSource, fragSource)
{
    VertAttributes attrs;
    attrs.add({"nr", 1, 1, GL_BYTE});
    lineMesh = SharedMesh(new Mesh("debug_line", 2, attrs));
    lineMesh->set(true, 0, 0);
    lineMesh->set(false, 1, 0);
    VertBuffer::uploadSingleMesh(lineMesh);

    u_colorId = shaderProgram.location("u_color");
    MVPId = shaderProgram.location("MVP");
    p0Id = shaderProgram.location("p0");
    p1Id = shaderProgram.location("p1");
}

void DebugLineRenderer::line(const vec3 &p0, const vec3 &p1, const vec3 &color)
{
    shaderProgram.use();
    glUniform3f(u_colorId, color.r, color.g, color.b);
    glUniform3f(p0Id, p0.x * scale, p0.y * scale, p0.z * scale);
    glUniform3f(p1Id, p1.x * scale, p1.y * scale, p1.z * scale);
    glUniformMatrix4fv(MVPId, 1, GL_FALSE, &projection[0][0]);
    lineMesh->renderArrays(GL_LINES);
}

void DebugLineRenderer::axes(const vec3 &p, float size, const vec3 &color)
{
    line(p - vec3(size, 0, 0), p + vec3(size, 0, 0), color);
    line(p - vec3(0, size, 0), p + vec3(0, size, 0), color);
    line(p - vec3(0, 0, size), p + vec3(0, 0, size), color);
}

void DebugLineRenderer::square(const vec3 &p, float size, const vec3 &color, const vec3 &xAxis, const vec3 &yAxis)
{
    line(p, p + size * xAxis, color);
    line(p + size * yAxis, p + size * yAxis + size * xAxis, color);
    line(p, p + size * yAxis, color);
    line(p + size * xAxis, p + size * yAxis + size * xAxis, color);
}

void DebugLineRenderer::arrow(const vec2 &p0, const vec2 &p1, float size, const vec3 &color)
{
    line(p0, p1, color);
    const vec2 dir = p1 - p0;
    auto line0 = normalize(rotate(dir, -135 * mu::DEGREES_TO_RAD)) * size;
    line(p1, p1 + line0, color);

    auto line1 = normalize(rotate(dir, 135 * mu::DEGREES_TO_RAD)) * size;
    line(p1, p1 + line1, color);
}

void DebugLineRenderer::arrow(const vec3 &p0, const vec3 &p1, float size, const vec3 &color, const vec3 &arrowHeadAxis)
{
    line(p0, p1, color);
    const vec3 dir = p1 - p0;

    auto line0 = normalize(rotate(dir, -135 * mu::DEGREES_TO_RAD, arrowHeadAxis)) * size;
    line(p1, p1 + line0, color);

    auto line1 = normalize(rotate(dir, 135 * mu::DEGREES_TO_RAD, arrowHeadAxis)) * size;
    line(p1, p1 + line1, color);
}

void DebugLineRenderer::arrows(const vec2 &p_, float size, const vec3 &color)
{
    vec3 p(p_, 0);

    line(p - vec3(size, 0, 0), p + vec3(size, 0, 0), color);
    line(p - vec3(0, size, 0), p + vec3(0, size, 0), color);

    float arrowSize = size * .3;

    line(p - vec3(size, 0, 0), p - vec3(size - arrowSize, arrowSize, 0), color);
    line(p - vec3(size, 0, 0), p - vec3(size - arrowSize, -arrowSize, 0), color);

    line(p + vec3(size, 0, 0), p + vec3(size - arrowSize, arrowSize, 0), color);
    line(p + vec3(size, 0, 0), p + vec3(size - arrowSize, -arrowSize, 0), color);

    line(p - vec3(0, size, 0), p - vec3(arrowSize, size - arrowSize, 0), color);
    line(p - vec3(0, size, 0), p - vec3(-arrowSize, size - arrowSize, 0), color);

    line(p + vec3(0, size, 0), p + vec3(arrowSize, size - arrowSize, 0), color);
    line(p + vec3(0, size, 0), p + vec3(-arrowSize, size - arrowSize, 0), color);
}

void DebugLineRenderer::circle(const vec2 &p, float radius, int resolution, const vec3 &color)
{
    vec2 prev;
    for (int i = 0; i <= resolution; i++)
    {
        vec2 off = vec2(0, radius);
        off = rotate(off, mu::PI * (2.f / resolution) * i);
        vec2 curr = p + off;

        if (i != 0)
            line(prev, curr, color);
        prev = curr;
    }
}

void DebugLineRenderer::circle(const vec3 &p, float radius, int resolution, const vec3 &color, const vec3 &xAxis,
        const vec3 &yAxis)
{
    vec3 prev;
    for (int i = 0; i <= resolution; i++)
    {
        vec3 off = xAxis * radius;
        off = rotate(off, mu::PI * (2.f / resolution) * i, yAxis);
        vec3 curr = p + off;

        if (i != 0)
            line(prev, curr, color);
        prev = curr;
    }
}
