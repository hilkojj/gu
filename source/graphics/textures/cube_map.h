#pragma once

class CubeMap
{
  public:
    static CubeMap fromImageFiles(const char *paths[6]);

    const unsigned int id;

    CubeMap(unsigned int id);

    void bind(unsigned int unit);
};
