#ifndef DEBUG_LINE_RENDERER_H
#define DEBUG_LINE_RENDERER_H

#include "../shader_program.h"
#include "mesh.h"
#include "vert_attributes.h"
#include "vert_buffer.h"

/**
 * THIS LINE RENDERER IS VEEEEEEEEEERYYYY SLOOOOOOW, so only use it for debugging
 **/
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

    void arrows(const vec2 &p, float size, const vec3 &color);

    void square(const vec3 &p, float size, const vec3 &color, const vec3 &xAxis=mu::X, const vec3 &yAxis=mu::Y);

    void circle(const vec2 &p, float radius, int resolution, const vec3 &color);

  private:
    ShaderProgram shaderProgram;
    GLuint u_colorId, p0Id, p1Id, MVPId;
    SharedMesh lineMesh;
};

#endif
