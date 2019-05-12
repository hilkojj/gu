
#include "frame_buffer.h"
#include "../gu/game_utils.h"

void FrameBuffer::unbindCurrent()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, gu::widthPixels, gu::heightPixels);
}

GLuint create()
{
    GLuint id;
    glGenFramebuffers(1, &id);
    return id;
}

FrameBuffer::FrameBuffer(GLuint width, GLuint height) : id(create()), width(width), height(height)
{
    std::cout << "FrameBuffer " << id << " created\n";
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &id);
    std::cout << "FrameBuffer " << id << " destroyed\n";
}

void FrameBuffer::bind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, width, height);
}

void FrameBuffer::addColorTexture(GLuint format, GLuint magFilter, GLuint minFilter)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texId, 0);
    colorTexture = SharedTexture(new Texture(texId, width, height));

    unbindCurrent();
}

// use this if you want the depth rendered to a texture, use addDepthBuffer() if you do not want that.
void FrameBuffer::addDepthTexture(GLuint magFilter, GLuint minFilter)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texId, 0);
    depthTexture = SharedTexture(new Texture(texId, width, height));

    unbindCurrent();
}

void FrameBuffer::addDepthBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    GLuint id;
    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id);

    unbindCurrent();
}
