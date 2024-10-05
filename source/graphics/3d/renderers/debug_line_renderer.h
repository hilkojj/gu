#ifndef DEBUG_LINE_RENDERER_H
#define DEBUG_LINE_RENDERER_H

#include "../shared_3d.h"

#include "../../shader_program.h"

#include "../../../math/math_utils.h"

/*
 * Purely for debugging purposes.
 * This line renderer is not batched. Every line is a draw-call, shapes are multiple draw calls.
 */
class DebugLineRenderer
{
  public:
    DebugLineRenderer();

    mat4 projection;

    float scale = 1;

    // make sure to set 'projection' before calling this.
    void line(const vec3 &p0, const vec3 &p1, const vec3 &color);

    inline void line(const vec2 &p0, const vec2 &p1, const vec3 &color) { line(vec3(p0, 0), vec3(p1, 0), color); }

    // make sure to set 'projection' before calling this.
    void axes(const vec3 &p, float size, const vec3 &color);

    inline void axes(const vec2 &p, float size, const vec3 &color) { axes(vec3(p, 0), size, color); }

    void arrow(const vec2 &p0, const vec2 &p1, float size, const vec3 &color);

    void arrow(const vec3 &p0, const vec3 &p1, float size, const vec3 &color, const vec3 &arrowHeadAxis = mu::Y);

    void arrows(const vec2 &p, float size, const vec3 &color);

    void square(const vec3 &p, float size, const vec3 &color, const vec3 &xAxis = mu::X, const vec3 &yAxis = mu::Y);

    void circle(const vec2 &p, float radius, int resolution, const vec3 &color);

    void circle(const vec3 &p, float radius, int resolution, const vec3 &color, const vec3 &xAxis = mu::X, const vec3 &yAxis = mu::Y);

  private:
    ShaderProgram shaderProgram;
    int u_colorLocation, p0Location, p1Location, MVPLocation;
    SharedMesh lineMesh;
};

#endif
