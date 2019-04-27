#version 330 core
out vec3 color;
in vec3 v_normal;

void main()
{
    color = v_normal;
}
