#ifndef TEXTURE_ARRAY_H
#define TEXTURE_ARRAY_H

#include <memory>
#include <vector>
#include <string>
#include "../gl_includes.h"

class TextureArray;

typedef std::shared_ptr<TextureArray> SharedTexArray;

class TextureArray
{

  public:

    static SharedTexArray fromDDSFiles(const std::vector<std::string> &paths);

    const GLuint id, width, height, layers;

    TextureArray(GLuint id, GLuint width, GLuint height, GLuint layers);

    void bind(GLuint unit);

    ~TextureArray();

};


#endif
