
#ifndef POLYGON_H
#define POLYGON_H

#include <string>
#include <vector>

#include "glm/glm.hpp"

class Polygon
{

  public:

    std::vector<glm::vec2> points;

    Polygon();
    Polygon(int points);

    bool contains(float x, float y);

    std::string toString();

};

#endif
