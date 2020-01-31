
#ifndef VERT_ATTRIBUTES_H
#define VERT_ATTRIBUTES_H

#include <iostream>
#include <vector>

#include "../../gl_includes.h"

struct VertAttr
{
    std::string name;
    GLint size, byteSize = sizeof(float) * size;
    GLenum type = GL_FLOAT;
    GLboolean normalized = GL_FALSE;
};

class VertAttributes
{

  public:
    static const struct VertAttr
            POSITION, RGB, RGBA,
            TEX_COORDS,
            NORMAL, TANGENT, BI_NORMAL,

            /* A transform is a 4x4 matrix = 16 floats.
             * An attribute in OpenGL cannot be bigger than 4 floats,
             * therefore the solution is to store a transform in 4 separate columns:
             */
            TRANSFORM_COL_A, TRANSFORM_COL_B, TRANSFORM_COL_C, TRANSFORM_COL_D;

    /**
     * adds the attribute, and returns the offset in bytes
     *
     * Example:
     * // attrs = [POSITION]
     *
     * attrs.add(NORMAL); // returns 3 * sizeof(float)
     *
     * // attrs = [POSITION, NORMAL]
     */
    unsigned int add(const VertAttr& attr);

    // same as above but returns this.
    VertAttributes &add_(const VertAttr& attr);

    const VertAttr &get(unsigned int i) const;

    unsigned int nrOfAttributes() const;

    // returns the number of bytes per vertex. [POSITION, NORMAL] will return 6 * sizeof(float).
    unsigned int getVertSize() const;

    // returns the number of components per vertex. [POSITION, NORMAL] will return 6.
    unsigned int getNrOfComponents() const;

    // returns the offset (in bytes)
    unsigned int getOffset(const VertAttr &attr) const;

    bool contains(const VertAttr &attr) const;

    // to string
    friend std::ostream &operator<<(std::ostream &stream, const VertAttributes &attrs);

  private:
    unsigned int vertSize = 0, nrOfComponents = 0;
    std::vector<VertAttr> attributes;
};


inline const struct VertAttr
        VertAttributes::POSITION = {"POSITION", 3},
        VertAttributes::RGB = {"RGB", 3},
        VertAttributes::RGBA = {"RGBA", 4},
        VertAttributes::TEX_COORDS = {"TEX_COORDS", 2},
        VertAttributes::NORMAL = {"NORMAL", 3},
        VertAttributes::TANGENT = {"TANGENT", 3},
        VertAttributes::BI_NORMAL = {"BI_NORMAL", 3},
        VertAttributes::TRANSFORM_COL_A = {"TRANSFORM_COL_A", 4},
        VertAttributes::TRANSFORM_COL_B = {"TRANSFORM_COL_B", 4},
        VertAttributes::TRANSFORM_COL_C = {"TRANSFORM_COL_C", 4},
        VertAttributes::TRANSFORM_COL_D = {"TRANSFORM_COL_D", 4};

#endif
