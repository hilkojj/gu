
#include "vert_attributes_string.h"
#include "vert_attributes.h"

std::ostream &operator<<(std::ostream &stream, const VertAttributes &attrs)
{
    stream << "[ ";
    for (const VertAttr &attr : attrs.attributes)
    {
        stream << "{" << attr.name << ", " << attr.size << ", " << (attr.normalized ? "GL_TRUE" : "GL_FALSE") << "} ";
    }
    return stream << "]";
}
