#version 300 es
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 a_normal;

// per instance attribute:
layout(location = 2) in vec3 instanceOffset;

uniform mat4 MVP;

out vec3 v_normal;

void main()
{
    gl_Position = MVP * vec4(vertexPosition_modelspace + instanceOffset, 1);
    v_normal = a_normal;
}
