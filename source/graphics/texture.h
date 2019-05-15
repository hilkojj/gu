#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include "glad/glad.h"

class Texture;

typedef std::shared_ptr<Texture> SharedTexture;

class Texture
{

  public:

    /**
     * Load a DXT1/3/5 texture directly to OpenGL
     */
    static SharedTexture fromDDSFile(const char *path);

    const GLuint id, width, height;

    Texture(GLuint id, GLuint width, GLuint height);

    void bind(GLuint unit);

    ~Texture();

};

class DDSData
{
  public:

    GLuint height, width, linearSize, mipMapCount, fourCC, format, bufsize;

    unsigned char *buffer;

    DDSData(const char *path);

    ~DDSData();
};

#endif
