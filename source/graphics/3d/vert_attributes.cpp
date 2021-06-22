#include <iostream>
#include <ostream>
#include <string>

#include "vert_attributes.h"
#include "../../gl_includes.h"
#include "../../utils/gu_error.h"

unsigned int VertAttributes::add(const VertAttr& attr)
{
    unsigned int offset = vertSize;
    vertSize += attr.byteSize;
    nrOfComponents += attr.size;

    if (attr.size > 4) throw gu_err("Tried to add a VertAttr with a size > 4");

    attributes.push_back(attr);

    return offset;
}

VertAttributes &VertAttributes::add_(const VertAttr& attr)
{
    add(attr);
    return *this;
}

const VertAttr &VertAttributes::get(unsigned int i) const
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
    for (const VertAttr& a : attributes)
    {
        if (a.name == attr.name)
            return offset;
        offset += a.byteSize;
    }
    throw gu_err(attr.name + " is not in VertAttributes, add it first with .add(attribute)");
}

bool VertAttributes::contains(const VertAttr &attr) const
{
    for (const VertAttr& a : attributes)
        if (a.normalized == attr.normalized && a.size == attr.size && a.byteSize == attr.byteSize && a.type == attr.type && a.name == attr.name)
            return true;
    return false;
}

std::ostream &operator<<(std::ostream &stream, const VertAttributes &attrs)
{
    stream << "[ ";
    for (const VertAttr& a : attrs.attributes)
        stream << "{" << a.name << ", " << a.size << ", " << (a.normalized ? "GL_TRUE" : "GL_FALSE") << "} ";
    return stream << "]";
}

unsigned int VertAttributes::getNrOfComponents() const
{
    return nrOfComponents;
}
