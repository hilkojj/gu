
#include <iostream>
#include "texture_array.h"
#include "texture.h"
#include "../utils/gu_error.h"

SharedTexArray TextureArray::fromDDSFiles(const std::vector<std::string> &paths)
{
    if (paths.size() == 0)
        throw gu_err("TextureArray::fromDDSFiles needs at least 1 DDS file");

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
    std::cout << "TextureArray (" << width << "x" << height << " x " << layers << ") id: " << id << " created\n";
}

void TextureArray::bind(GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

TextureArray::~TextureArray()
{
    glDeleteTextures(1, &id);
    std::cout << "TextureArray " << id << " destroyed\n";
}
