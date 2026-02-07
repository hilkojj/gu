#pragma once

#include "../external/gl_includes.h"

class Texture
{
  public:
    static Texture fromImageFile(const char *path);

    static Texture fromByteData(const GLubyte *data, GLint internalFormat, GLenum format, GLsizei width, GLsizei height,
        GLuint magFilter, GLuint minFilter, bool bGenerateMipMaps = true, GLenum type = GL_UNSIGNED_BYTE);

    const GLuint id, width, height;

    Texture(GLuint id, GLuint width, GLuint height);

    void bind(GLuint unit) const;

    // same as bind() but also sets sampler uniform for shader.
    void bind(GLuint unit, const class ShaderProgram &shader, const char* name) const;

    ~Texture();

};
