
#include "../gu_error.h"
#include "Aseprite.h"

aseprite::Slice &aseprite::Sprite::getSliceByName(const char *name, int frame)
{
    for (auto &slice : slices)
    {
        if (slice.frame == frame && slice.name == name)
            return slice;
    }
    throw gu_err("Sprite '" + this->name + "' does not have a slice at frame " + std::to_string(frame) + " named " + std::string(name));
}
