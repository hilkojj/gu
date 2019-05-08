#include <iostream>
#include <cstring>
#include <string>

#include "texture.h"
#include "../utils/gu_error.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
SharedTexture Texture::fromDDSFile(const char *path)
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

    unsigned int height = *(unsigned int *)&(header[8]),
                 width = *(unsigned int *)&(header[12]),
                 linearSize = *(unsigned int *)&(header[16]),
                 mipMapCount = *(unsigned int *)&(header[24]),
                 fourCC = *(unsigned int *)&(header[80]);

    unsigned char *buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char *)malloc(bufsize * sizeof(unsigned char));
    s = fread(buffer, 1, bufsize, fp);
    fclose(fp);

    unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
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
        free(buffer);
        throw gu_err("DDS texture is not DXT1 DXT3 or DXT5: " + std::string(path));
    }
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0, w = width, h = height;
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, w, h,
                               0, size, buffer + offset);
        offset += size;
        w /= 2;
        h /= 2;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1); // https://gist.github.com/tilkinsc/13191c0c1e5d6b25fbe79bbd2288a673
    free(buffer);
    return SharedTexture(new Texture(id, width, height));
}

Texture::Texture(GLuint id, GLuint width, GLuint height)
    : id(id), width(width), height(height)
{
    std::cout << "Texture " << id << " created\n";
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
