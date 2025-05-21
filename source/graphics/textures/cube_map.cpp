#include "cube_map.h"
#include "../../utils/gu_error.h"

#include "../external/gl_includes.h"

#include "../external/stb_image.h"

CubeMap CubeMap::fromImageFiles(const char *paths[6])
{
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    for (int i = 0; i < 6; i++)
    {
        int width, height, numChannels;
        unsigned char *data = stbi_load(paths[i], &width, &height, &numChannels, 0);
        if (data)
        {
            if (numChannels != 3 && numChannels != 4)
            {
                throw gu_err("Loaded an image with unsupported amount of channels (" + std::to_string(numChannels) + "): " + std::string(paths[i]));
            }
            const GLint format = numChannels == 3 ? GL_RGB : GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            throw gu_err("Failed to load image: " + std::string(paths[i]));
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return CubeMap(id);
}

CubeMap::CubeMap(GLuint id) :
    id(id)
{
}

void CubeMap::bind(GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}
