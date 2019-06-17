#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include <memory>
#include <vector>
#include <string>
#include "../gl_includes.h"

class CubeMap;

typedef std::shared_ptr<CubeMap> SharedCubeMap;

class CubeMap
{

  public:

    // note: mipmaps not supported
    static SharedCubeMap fromDDSFiles(const std::vector<std::string> &paths);

    const GLuint id, width, height;

    CubeMap(GLuint id, GLuint width, GLuint height);

    void bind(GLuint unit);

    ~CubeMap();

};

#endif
