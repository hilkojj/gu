
#include "../gu_error.h"
#include "Aseprite.h"

const aseprite::Slice &aseprite::Sprite::getSliceByName(const char *name, int frame) const
{
    for (auto &slice : slices)
    {
        if (slice.frame == frame && slice.name == name)
            return slice;
    }
    throw gu_err("Sprite '" + this->name + "' does not have a slice at frame " + std::to_string(frame) + " named " + std::string(name));
}
