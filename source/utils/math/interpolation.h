
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "../gu_error.h"
#include "../type_name.h"
#include "../math_utils.h"

namespace Interpolation
{

inline float circleIn(float x)
{
    return 1 - sqrt(1 - x * x);
}

inline float circleOut(float x)
{
    x--;
    return sqrt(1 - x * x);
}

inline float circle(float x)
{
    if (x < .5) return circleIn(x * 2.) * .5;
    else return circleOut(x * 2. - 1.) * .5 + .5;
}

inline float powIn(float x, int power)
{
    return pow(x, power);
}

inline float powOut(float x, int power)
{
    return 1 + pow(x - 1, power) * (power % 2 == 0 ? -1 : 1);
}

inline float pow(float x, int power)
{
    if (x < .5) return powIn(x * 2., power) * .5;
    else return powOut(x * 2. - 1., power) * .5 + .5;
}

// todo: add more.




template<typename Type>
inline void interpolate(const Type &a, const Type &b, float x, Type &out)
{
    if constexpr (std::is_arithmetic_v<Type>)
        out = glm::mix(a, b, x);
    else
        throw gu_err("Cannot interpolate a " + getTypeName<Type>() + " value.");
}

template <int len, typename type, qualifier something>
inline void interpolate(const vec<len, type, something> &a, const vec<len, type, something> &b, float x, vec<len, type, something> &out)
{
    out = glm::mix(a, b, x);
}

template <>
inline void interpolate(const quat &a, const quat &b, float x, quat &out)
{
    out = glm::slerp(a, b, x);
}

}

#endif
