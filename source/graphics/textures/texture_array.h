#pragma once

#include "shared_texture.h"

#include "../external/gl_includes.h"

#include <vector>
#include <string>

class TextureArray
{
  public:
    /*
    WARNING when using fromDDSFiles() in WebGL:

    There is a bug in the WebGL implementation of Chrome/Chromium that makes it
    impossible to load multiple DDS textures to a TextureArray.

    This function contains an extremely dirty hack to fix that problem (this hack is only used in WebGL and not in Desktop-OpenGL)
    It will convert the DDS data to RGB data (by rendering the DDS texture to a FrameBuffer), this RGB data will then be loaded to a TextureArray using fromByteData()

    This hack makes the process of loading a TextureArray 10-20 times slower :(
    ALSO BE SURE NOT TO USE THIS FUNCTION WHILE USING A FRAMEBUFFER OR SHADERPROGRAM!!
     */
    static SharedTexArray fromDDSFiles(const std::vector<std::string> &paths);

    static SharedTexArray fromByteData(const GLubyte *data, GLenum format, GLsizei width, GLsizei height, GLsizei depth, GLuint magFilter, GLuint minFilter, bool bGenerateMipMaps = true);

    const GLuint id, width, height, layers;

    TextureArray(GLuint id, GLuint width, GLuint height, GLuint layers);

    void bind(GLuint unit);

    ~TextureArray();
};
