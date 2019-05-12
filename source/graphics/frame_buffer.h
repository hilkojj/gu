#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "texture.h"
#include "glad/glad.h"

class FrameBuffer
{

  public:

    // unbinds current FrameBuffer
    static void unbindCurrent();

    const GLuint id, width, height;

    SharedTexture colorTexture;
    SharedTexture depthTexture;

    FrameBuffer(GLuint width, GLuint height);

    ~FrameBuffer();

    void bind();

    // format can be GL_RGB or GL_RGBA. magFilter and minFilter can be GL_LINEAR for example.
    void addColorTexture(GLuint format, GLuint magFilter, GLuint minFilter);

    // use this if you want the depth rendered to a texture, use addDepthBuffer() if you do not want that.
    void addDepthTexture(GLuint magFilter, GLuint minFilter);

    void addDepthBuffer();

};

#endif
