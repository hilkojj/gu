#include <iostream>
#include <cstring>
#include <string>

#include "texture.h"
#include "../utils/gu_error.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
// https://gist.github.com/tilkinsc/13191c0c1e5d6b25fbe79bbd2288a673
SharedTexture Texture::fromDDSFile(const char *path)
{
    DDSData data(path);

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, data.mipMapCount - 1); // opengl likes array length of mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    unsigned int blockSize = (data.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0, w = data.width, h = data.height;
    for (unsigned int level = 0; level < data.mipMapCount && (data.width || data.height); ++level)
    {
        unsigned int size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, data.format, w, h,
                               0, size, data.buffer + offset);
        offset += size;
        w /= 2;
        h /= 2;
    }
    return SharedTexture(new Texture(id, data.width, data.height));
}

Texture::Texture(GLuint id, GLuint width, GLuint height)
    : id(id), width(width), height(height)
{
    std::cout << "Texture (" << width << "x" << height << ") id: " << id << " created\n";
}

void Texture::bind(GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
    std::cout << "Texture " << id << " destroyed\n";
}

DDSData::DDSData(const char *path)
{
    FILE *fp;
    fp = fopen(path, "rb");
    if (!fp)
        throw gu_err("Cannot open DDS texture: " + std::string(path));

    char fileCode[4];
    size_t s = fread(fileCode, 1, 4, fp);
    if (std::strncmp(fileCode, "DDS ", 4) != 0)
    {
        fclose(fp);
        throw gu_err("File is not a DDS texture: " + std::string(path));
    }
    unsigned char header[124];
    s = fread(&header, 124, 1, fp);

    height = *(unsigned int *)&(header[8]);
    width = *(unsigned int *)&(header[12]);
    linearSize = *(unsigned int *)&(header[16]);
    mipMapCount = *(unsigned int *)&(header[24]);
    fourCC = *(unsigned int *)&(header[80]);

    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = new unsigned char[bufsize];
    s = fread(buffer, 1, bufsize, fp);
    fclose(fp);

    switch (fourCC)
    {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        delete[] buffer;
        throw gu_err("DDS texture is not DXT1 DXT3 or DXT5: " + std::string(path));
    }
}

DDSData::~DDSData()
{
    delete[] buffer;
}
