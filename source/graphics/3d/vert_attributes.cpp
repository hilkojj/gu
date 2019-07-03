
#include <iostream>
#include <ostream>
#include <string>

#include "vert_attributes.h"
#include "../../gl_includes.h"
#include "../../utils/gu_error.h"

const struct VertAttr
    VertAttributes::POSITION = {"POSITION", 3},
    VertAttributes::RGB = {"RGB", 3},
    VertAttributes::RGBA = {"RGBA", 4},
    VertAttributes::TEX_COORDS = {"TEX_COORDS", 2},
    VertAttributes::NORMAL = {"NORMAL", 3},
    VertAttributes::TANGENT = {"TANGENT", 3},
    VertAttributes::BI_NORMAL = {"BI_NORMAL", 3};

unsigned int VertAttributes::add(VertAttr attr)
{
    unsigned int offset = vertSize;
    vertSize += attr.size;

    attributes.push_back(attr);

    return offset;
}

VertAttributes &VertAttributes::add_(VertAttr attr)
{
    add(attr);
    return *this;
}

VertAttr &VertAttributes::get(unsigned int i)
{
    return attributes[i];
}

unsigned int VertAttributes::nrOfAttributes() const
{
    return attributes.size();
}

unsigned int VertAttributes::getVertSize() const
{
    return vertSize;
}

unsigned int VertAttributes::getOffset(const VertAttr &attr) const
{
    unsigned int offset = 0;
    for (VertAttr a : attributes)
    {
        if (a.name == attr.name)
            return offset;
        offset += a.size;
    }
    throw gu_err(attr.name + " is not in VertAttributes, add it first with .add(attribute)");
}

bool VertAttributes::contains(const VertAttr &attr) const
{
    for (VertAttr a : attributes)
        if (a.normalized == attr.normalized && a.size == attr.size && a.name == attr.name)
            return true;
    return false;
}

std::ostream &operator<<(std::ostream &stream, const VertAttributes &attrs)
{
    stream << "[ ";
    for (VertAttr a : attrs.attributes)
        stream << "{" << a.name << ", " << a.size << ", " << (a.normalized ? "GL_TRUE" : "GL_FALSE") << "} ";
    return stream << "]";
}
