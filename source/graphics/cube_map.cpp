
#include <iostream>
#include "cube_map.h"
#include "texture.h"
#include "../utils/gu_error.h"

SharedCubeMap CubeMap::fromDDSFiles(const std::vector<std::string> &paths)
{
    if (paths.size() != 6)
        throw gu_err("CubeMap::fromDDSFiles needs 6 DDS files");

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    std::vector<DDSData> datas;
    datas.reserve(6);
    for (auto &path : paths)
        datas.emplace_back(path.c_str());

    DDSData &d = datas[0];

    GLsizei width = d.width, height = d.height;

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLuint blockSize = (d.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

    GLuint size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;

    int i = 0;
    for (auto &data : datas)
    {
        if (data.width != width || data.height != height || data.format != d.format)
            throw gu_err("CubeMap: DDS files cannot have different dimensions/formats/mipmap-levels");

        glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, d.format, width, height, 0, size, data.buffer);
        i++;
    }
    return SharedCubeMap(new CubeMap(id, width, height));
}

CubeMap::CubeMap(GLuint id, GLuint width, GLuint height)
    : id(id), width(width), height(height)
{
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "CubeMap (" << width << "x" << height << ") id: " << id << " created\n";
    #endif
}

void CubeMap::bind(GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

CubeMap::~CubeMap()
{
    glDeleteTextures(1, &id);
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "CubeMap " << id << " destroyed\n";
    #endif
}
