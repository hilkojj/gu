
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "glm/glm.hpp"

namespace Interpolation
{

inline float circleIn(float x)
{
    return 1 - glm::sqrt(1 - x * x);
}

inline float circleOut(float x)
{
    x--;
    return glm::sqrt(1 - x * x);
}

inline float circle(float x)
{
    if (x < .5) return circleIn(x * 2.) * .5;
    else return circleOut(x * 2. - 1.) * .5 + .5;
}

inline float powIn(float x, int power)
{
    return glm::pow(x, power);
}

inline float powOut(float x, int power)
{
    return 1 + glm::pow(x - 1, power) * (power % 2 == 0 ? -1 : 1);
}

inline float pow(float x, int power)
{
    if (x < .5) return powIn(x * 2., power) * .5;
    else return powOut(x * 2. - 1., power) * .5 + .5;
}

// todo: add more.

}

#endif
