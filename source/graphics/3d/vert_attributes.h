
#ifndef VERT_ATTRIBUTES_H
#define VERT_ATTRIBUTES_H

#include <iostream>
#include <vector>

#include "glad/glad.h"

struct VertAttr
{
    std::string name;
    GLint size;
    GLboolean normalized;
};

class VertAttributes
{

  public:
    static const struct VertAttr POSITION, RGB, RGBA, TEX_COORDS, NORMAL, TANGENT, BI_NORMAL;

    // adds the attribute, and returns the offset that can be used in an interleaved vertices array (VNCVNCVNCVNC https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices#Formatting_VBO_Data)
    unsigned int add(VertAttr attr);

    VertAttr &get(unsigned int i);

    unsigned int nrOfAttributes() const;

    // returns the number of floats per vertex. [POSITION, NORMAL] will return 6.
    unsigned int getVertSize() const;

    // returns the offset that can be used in an interleaved vertices array (VNCVNCVNCVNC https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices#Formatting_VBO_Data)
    unsigned int getOffset(const VertAttr &attr) const;

    bool contains(const VertAttr &attr) const;

    // to string
    friend std::ostream &operator<<(std::ostream &stream, const VertAttributes &attrs);

  private:
    unsigned int vertSize = 0;
    std::vector<VertAttr> attributes;
};

#endif
