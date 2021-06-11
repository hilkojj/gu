#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include <memory>
#include <vector>
#include <string>
#include "../gl_includes.h"
#include "texture.h"

class CubeMap
{

  public:

    // note: mipmaps not supported
    static CubeMap fromDDSFiles(const std::vector<std::string> &paths);

    static CubeMap fromHDRFile(const char *path, unsigned int resolution=512);

    /**
     * An equirectangular environment map is like a cubemap but with all 6 faces stored in 1 rectangular texture.
     * Use fromHDRFile() instead to load an HDR map as CubeMap.
     */
    static Texture equirectangularFromHDRFile(const char *path);

    const GLuint id, width, height;

    CubeMap(GLuint id, GLuint width, GLuint height);

    void bind(GLuint unit);

    ~CubeMap();

};

typedef std::shared_ptr<CubeMap> SharedCubeMap;

#endif
