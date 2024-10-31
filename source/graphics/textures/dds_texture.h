#pragma once

#include "texture.h"
#include "shared_texture.h"

namespace DDSTexture
{
    SharedTexture fromDDSFile(const char *path);

    class DDSData
    {
      public:

        GLuint height, width, linearSize, mipMapCount, fourCC, format, bufsize;

        unsigned char *buffer;

        DDSData(const char *path);

        ~DDSData();
    };
}
