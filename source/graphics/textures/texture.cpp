#include "texture.h"

#include "../shader_program.h"
#include "../external/stb_image.h"

#include "../../utils/gu_error.h"

Texture Texture::fromImageFile(const char *path)
{
    int width, height, channels;
    unsigned char *imgData = stbi_load(path, &width, &height, &channels, 0);

    if (imgData == NULL)
        throw gu_err("Could not load image using STB: " + std::string(path));

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // todo: shouldnt be hardcoded
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // todo: mipmapping stuff
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
    else if (channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
    else
        throw gu_err("Loaded an image with unsupported amount of channels (" + std::to_string(channels) + ").");

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(imgData);
    return Texture(id, width, height);
}

Texture::Texture(GLuint id, GLuint width, GLuint height)
    : id(id), width(width), height(height)
{
}

void Texture::bind(GLuint unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::bind(GLuint unit, const ShaderProgram &shader, const char* name) const
{
    bind(unit);
    glUniform1i(shader.location(name), unit);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

Texture Texture::fromByteData(
    const GLubyte *data, GLint internalFormat, GLenum format, GLsizei width, GLsizei height, GLuint magFilter,
    GLuint minFilter, bool bGenerateMipMaps, GLenum type
)
{
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);

    if (bGenerateMipMaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    return Texture(id, width, height);
}
