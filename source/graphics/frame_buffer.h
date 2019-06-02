#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "texture.h"
#include "glad/glad.h"

class FrameBuffer
{

  public:

    const GLuint id, width, height, samples;

    SharedTexture colorTexture;
    SharedTexture depthTexture;

    FrameBuffer(GLuint width, GLuint height);

    FrameBuffer(GLuint width, GLuint height, GLuint samples);

    ~FrameBuffer();

    void bind();

    void unbind();

    // format can be GL_RGB or GL_RGBA. magFilter and minFilter can be GL_LINEAR for example.
    void addColorTexture(GLuint format, GLuint magFilter, GLuint minFilter);

    void addColorBuffer(GLuint format);

    void addDepthTexture(GLuint magFilter, GLuint minFilter);

    void addDepthBuffer();

  private:
    FrameBuffer *sampled = NULL;

    static void unbindCurrent();

};

#endif
