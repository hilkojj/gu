
#include "Aseprite.h"

#include "../utils/gu_error.h"

const aseprite::Slice &aseprite::Sprite::getSliceByName(const char *name, int frame) const
{
    const Slice *best = NULL;
    for (auto &slice : slices)
    {
        if (slice.name == name)
        {
            if (slice.frame == frame)
                return slice;
            best = &slice;
        }
    }
    if (best)
        return *best;
    throw gu_err("Sprite '" + this->name + "' does not have a slice at frame " + std::to_string(frame) + " named " + std::string(name));
}

const aseprite::Tag &aseprite::Sprite::getTagByName(const char *name) const
{
    for (auto &tag : tags)
        if (tag.name == name)
            return tag;
    throw gu_err("Sprite '" + this->name + "' does not have a tag named " + std::string(name));
}

const aseprite::Tag *aseprite::Sprite::tryGetTagByName(const char *name) const
{
    for (auto &tag : tags)
        if (tag.name == name)
            return &tag;
    return nullptr;
}
