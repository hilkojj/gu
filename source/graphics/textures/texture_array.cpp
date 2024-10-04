
#include <iostream>
#include "texture_array.h"
#include "texture.h"
#include "graphics/frame_buffer.h"
#include "utils/gu_error.h"
#include "graphics/3d/renderers/quad_renderer.h"

SharedTexArray TextureArray::fromByteData(const GLubyte *data, GLenum format, GLsizei width, GLsizei height, GLsizei depth,
    GLuint magFilter, GLuint minFilter, bool bGenerateMipMaps)
{
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, depth, 0, format, GL_UNSIGNED_BYTE, data);

    if (bGenerateMipMaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return SharedTexArray(new TextureArray(id, width, height, depth));
}

SharedTexArray TextureArray::fromDDSFiles(const std::vector<std::string> &paths)
{
    if (paths.size() == 0)
        throw gu_err("TextureArray::fromDDSFiles needs at least 1 DDS file");

    #if EMSCRIPTEN
    // DIRTY HACK ALERT (read the comment in header file)
    FrameBuffer *fb = NULL;
    std::vector<GLubyte> data;
    int width_, height_;
    for (auto p : paths)
    {
        SharedTexture tex = Texture::fromDDSFile(p.c_str());
        if (!fb)
        {
            fb = new FrameBuffer(tex->width, tex->height);
            fb->addColorTexture(GL_RGBA, GL_NEAREST, GL_NEAREST);
            fb->bind();
        }
        width_ = tex->width;
        height_ = tex->height;
        QuadRenderer::render(tex);
        fb->bindAndGetPixels(GL_RGBA, data, data.size());
    }
    fb->unbind();
    delete fb;
    return TextureArray::fromByteData(&data[0], GL_RGBA, width_, height_, paths.size(), GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    #endif

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);

    std::vector<DDSData> datas;
    datas.reserve(paths.size());
    for (auto &path : paths)
        datas.emplace_back(path.c_str());

    DDSData &d = datas[0];

    GLsizei width = d.width, height = d.height, layerCount = datas.size(), mipMapCount = d.mipMapCount;

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1); // opengl likes array length of mipmaps
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLuint blockSize = (d.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

    GLuint offset = 0, w = width, h = height;
    for (int mipMapLevel = 0; mipMapLevel < mipMapCount; mipMapLevel++)
    {
        GLuint size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
        glCompressedTexImage3D(GL_TEXTURE_2D_ARRAY, mipMapLevel, d.format, w, h, layerCount, 0, size * layerCount, 0);

        int i = 0;
        for (auto &data : datas)
        {
            if (data.width != width || data.height != height || data.format != d.format || data.mipMapCount != mipMapCount)
                throw gu_err("TextureArray: DDS files cannot have different dimensions/formats/mipmap-levels");

            glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, mipMapLevel, 0, 0, i, w, h, 1, d.format, size, data.buffer + offset);

            i++;
        }
        offset += size;
        w /= 2;
        h /= 2;
    }
    return SharedTexArray(new TextureArray(id, width, height, layerCount));
}

TextureArray::TextureArray(GLuint id, GLuint width, GLuint height, GLuint layers)
    : id(id), width(width), height(height), layers(layers)
{
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "TextureArray (" << width << "x" << height << " x " << layers << ") id: " << id << " created\n";
    #endif
}

void TextureArray::bind(GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

TextureArray::~TextureArray()
{
    glDeleteTextures(1, &id);
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "TextureArray " << id << " destroyed\n";
    #endif
}
