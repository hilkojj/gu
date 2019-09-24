#ifndef DEBUG_LINE_RENDERER_H
#define DEBUG_LINE_RENDERER_H

#include "glm/glm.hpp"
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

    glm::mat4 projection;

    // make sure to set 'projection' before calling this.
    void line(glm::vec3 p0, glm::vec3 p1, glm::vec3 color);

  private:
    ShaderProgram shaderProgram;
    GLuint u_colorId, p0Id, p1Id, MVPId;
    SharedMesh lineMesh;
};

#endif
