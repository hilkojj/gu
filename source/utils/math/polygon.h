
#ifndef POLYGON_H
#define POLYGON_H

#include <string>
#include <vector>

#include "../math_utils.h"

class Polygon
{

  public:

    std::vector<vec2> points;

    Polygon();
    Polygon(int points);

    bool contains(float x, float y) const;

    std::string toString() const;

};

#endif
