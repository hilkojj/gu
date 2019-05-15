
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

    GLsizei width = 2, height = 2, layerCount = 2, mipMapCount = 1;

    GLubyte texels[32] = 
    {
        // Texels for first image.
        0,   0,   0,   255,
        255, 0,   0,   255,
        0,   255, 0,   255,
        0,   0,   255, 255,
        // Texels for second image.
        255, 255, 255, 255,
        255, 255,   0, 255,
        0,   255, 255, 255,
        255, 0,   255, 255,
    };
    
    // Allocate the storage.
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipMapCount, GL_RGBA8, width, height, layerCount);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layerCount, GL_RGBA, GL_UNSIGNED_BYTE, texels);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // std::vector<DDSData> datas;
    // for (auto &path : paths)
    //     datas.emplace_back(path.c_str());

    // DDSData &d = datas[0];

    // // Allocate the storage.
    // glTexStorage3D(GL_TEXTURE_2D_ARRAY, d.mipMapCount, d.format, d.width, d.height, datas.size());

    // GLuint blockSize = (d.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

    // unsigned int offset = 0, w = d.width, h = d.height;
    // for (int mipMapLevel = 0; mipMapLevel < d.mipMapCount; mipMapLevel++)
    // {
    //     // glTexSubImage3D(GL_TEXTURE_2D_ARRAY, mipMapLevel, 0, 0, 0, d.width, d.height, datas.size(), d.format, GL_UNSIGNED_BYTE, )

    //     int size = ((d.width + 3) / 4) * ((d.height + 3) / 4) * blockSize;

    //     for (auto &data : datas)
    //     {
    //         glCompressedTexImage3D(GL_TEXTURE_2D_ARRAY, mipMapLevel, d.format, d.width, d.height, datas.size(), 0, datas.size() * size, data.buffer + offset);
    //     }


    //     offset += size;
    //     w /= 2;
    //     h /= 2;
    // }
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
