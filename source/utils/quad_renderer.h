#ifndef QUAD_RENDERER_H
#define QUAD_RENDERER_H

#include "../graphics/texture.h"
#include "../graphics/3d/mesh.h"

namespace
{
#define MULTILINE(...) #__VA_ARGS__

static std::string vertSource = MULTILINE(

    layout(location = 0) in vec3 a_pos;

    out vec2 v_texCoords;

    void main()
    {
        gl_Position = vec4(a_pos, 1);
        v_texCoords = a_pos.xy * vec2(.5) + vec2(.5);
    }
);

static std::string fragSource = MULTILINE(

    precision mediump float;
    out vec4 color;
    in vec2 v_texCoords;
    uniform sampler2D tex;

    void main()
    {
        color = texture(tex, v_texCoords);
    }
);

ShaderProgram *shader = NULL;

} // namespace


namespace QuadRenderer
{
    // renders the texture to a full-screen quad.
    void render(SharedTexture texture)
    {
        if (!shader) shader = new ShaderProgram("QuadTextureShader", ("#version 300 es\n" + vertSource).c_str(), ("#version 300 es\n" + fragSource).c_str());

        texture->bind(0);
        shader->use();
        glUniform1i(shader->location("tex"), 0);
        Mesh::getQuad()->render();
    }
}

#endif
