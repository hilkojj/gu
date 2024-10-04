
#ifndef POLYGON_H
#define POLYGON_H

#include "math_utils.h"

#include <vector>

class Polygon
{
  public:

    std::vector<vec2> points;

    Polygon();
    Polygon(int points);

    bool contains(float x, float y) const;

};

#endif
