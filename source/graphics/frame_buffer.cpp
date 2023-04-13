
#include "frame_buffer.h"

#include <memory>
#include "../gu/game_utils.h"
#include "../utils/math_utils.h"

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

FrameBuffer::FrameBuffer(GLuint width, GLuint height, GLuint samples_)
    : id(create()), width(width), height(height),
      samples(
          #ifdef EMSCRIPTEN
          0
          #else
          samples_
          #endif
      )
{
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "FrameBuffer " << id << " created with " << samples << " samples" << std::endl;
    #endif

    if (samples) sampled = new FrameBuffer(width, height);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &id);
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "FrameBuffer " << id << " destroyed" << std::endl;
    #endif
    if (sampled) delete sampled;
}

void FrameBuffer::bind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, width, height);
}

void FrameBuffer::unbind()
{
    unbindCurrent();
    if (!sampled) return;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sampled->id);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, id);

    for (int i = 0; i < colorTextures.size(); i++)
    {
        GLenum at = GL_COLOR_ATTACHMENT0 + i;
        glDrawBuffer(at);
        glReadBuffer(at);

        GLbitfield mask = GL_COLOR_BUFFER_BIT;
        if (i == 0 && depthTexture) mask |= GL_DEPTH_BUFFER_BIT;

        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST);
    }
    if (colorTextures.empty() && depthTexture)
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    sampled->setDrawBuffers();
    unbindCurrent();
}

void FrameBuffer::addColorTexture(GLuint format, GLuint magFilter, GLuint minFilter, GLenum type)
{
    addColorTexture(format, format, magFilter, minFilter, type);
}

void FrameBuffer::addColorTexture(GLuint internalFormat, GLuint format, GLuint magFilter, GLuint minFilter, GLenum type)
{
    SharedTexture newTexture;
    if (sampled)
    {
        addColorBuffer(internalFormat); // INTERNAL FORMAT, because addColorBuffer() should provide internalFormat to OpenGL even though argument is named format.
        sampled->addColorTexture(internalFormat, format, magFilter, minFilter, type);
        newTexture = sampled->colorTextures.back();
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);

        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

        auto attachment = GL_COLOR_ATTACHMENT0 + colorTextures.size();

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texId, 0);

        newTexture = std::make_shared<Texture>(texId, width, height);
    }
    if (colorTextures.empty())
        colorTexture = newTexture;
    colorTextures.push_back(newTexture);

    setDrawBuffers();

    unbindCurrent();
}

void FrameBuffer::addColorBuffer(GLuint format)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    GLuint id;
    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);

    if (!sampled)
        glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    else
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);

    auto attachment = GL_COLOR_ATTACHMENT0 + colorTextures.size();

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, id);

    unbindCurrent();
}

void FrameBuffer::addDepthTexture(GLuint magFilter, GLuint minFilter, GLenum internalFormat)
{
    if (sampled)
    {
        addDepthBuffer();
        sampled->addDepthTexture(magFilter, minFilter);
        depthTexture = sampled->depthTexture;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    #ifdef EMSCRIPTEN
    magFilter = minFilter = GL_NEAREST;
    #endif

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texId, 0);
    depthTexture = SharedTexture(new Texture(texId, width, height));

    unbindCurrent();
}

void FrameBuffer::addDepthBuffer(GLenum format)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    GLuint id;
    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);

    if (!sampled)
        glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    else
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id);

    unbindCurrent();
}

void FrameBuffer::bindAndGetPixels(GLenum format, std::vector<GLubyte> &out, unsigned int outOffset)
{
    GLuint components = -1;
    switch(format)
    {
        case GL_BGR:
        case GL_RGB:
            components = 3; break;
        case GL_BGRA:
        case GL_RGBA:
            components = 4; break;
        case GL_ALPHA:
        case GL_LUMINANCE:
            components = 1; break;
    }
    out.resize(components * width * height + outOffset);
    bind();
    glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, &out[outOffset]);
}

void FrameBuffer::setDrawBuffers()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
    auto *attachments = new GLenum[colorTextures.size()];
    for (int i = 0; i < colorTextures.size(); i++)
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers(colorTextures.size(), attachments);
    delete[] attachments;
}

float FrameBuffer::getPixelDepth(int x, int y)
{
    bind();

    float depth;

    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    unbindCurrent();

    return depth;
}

void FrameBuffer::blitTo(GLbitfield mask, FrameBuffer *other, GLenum filter)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other ? other->id : 0);

    glBlitFramebuffer(0, 0, width, height, 0, 0, other ? other->width : gu::widthPixels, other ? other->height : gu::heightPixels, mask, filter);
}
