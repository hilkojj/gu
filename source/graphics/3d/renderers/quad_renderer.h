#ifndef QUAD_RENDERER_H
#define QUAD_RENDERER_H

#include "../mesh.h"
#include "../../textures/texture.h"

namespace QuadRenderer
{

namespace
{
#define MULTILINE(...) #__VA_ARGS__

static const char *vertSource = MULTILINE(

        layout(location = 0) in vec3 a_pos;

        out vec2 v_texCoords;

        void main()
        {
            gl_Position = vec4(a_pos, 1);
            v_texCoords = a_pos.xy * vec2(.5) + vec2(.5);
        }
);

static const char *fragSource = MULTILINE(

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


inline void render(Texture *tex)
{
    if (!shader) shader = new ShaderProgram("QuadTextureShader", vertSource, fragSource);

    tex->bind(0);
    shader->use();
    glUniform1i(shader->location("tex"), 0);
    Mesh::getQuad()->render();
}

// renders the texture to a full-screen quad.
inline void render(SharedTexture texture)
{
    render(texture.get());
}

}

#endif
