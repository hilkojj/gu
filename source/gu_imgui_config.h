#pragma once

#include "math/math_utils.h"

#define IM_VEC2_CLASS_EXTRA\
    ImVec2(const vec2 &f) { x = f.x; y = f.y; }\
    operator vec2() const { return vec2(x, y); }

#define IM_VEC4_CLASS_EXTRA\
    ImVec4(const vec4 &f) { x = f.x; y = f.y; z = f.z; w = f.w; }\
    operator vec4() const { return vec4(x, y, z, w); }
