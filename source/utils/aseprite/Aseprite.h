
#ifndef GAME_ASEPRITE_H
#define GAME_ASEPRITE_H


#include <vector>
#include <optional>
#include "../math_utils.h"

namespace aseprite
{

using ColorRGBA = u8vec4;
using ColorRGB = u8vec3;

struct UserData
{
    std::string text;
    ColorRGBA color;
};

struct Slice : UserData
{
    int frame;
    std::string name;
    int originX, originY, width, height;
    std::optional<ivec2> pivot;

    struct NineSlice
    {
        ivec2 topLeftOffset, innerSize;
    };

    std::optional<NineSlice> nineSlice;
};

struct Cel : UserData
{
    std::vector<uint8> pixels;

    int layer;
    int x, y, width, height;
    float alpha;
};

struct Frame
{
    float duration;
    std::vector<Cel> cels;
    std::vector<uint8> pixels;
};

struct Tag
{
    enum LoopDirection
    {
        forward = 0,
        reverse = 1,
        pingPong = 2
    };

    std::string name;
    LoopDirection loopDirection;
    int from;
    int to;
    float duration;
    ColorRGB color;
};

struct Layer : UserData
{
    enum Type
    {
        normal = 0,
        group = 1
    };

    std::string name;
    Type type;
    int childLevel;
    int blendMode;
    float alpha;
    bool visible;
};

enum Mode
{
    indexed = 1,
    grayscale = 2,
    rgba = 4
};

class Sprite
{
  public:

    std::string name;

    int width, height, frameCount;
    Mode mode;

    std::vector<Frame> frames;
    std::vector<Layer> layers;
    std::vector<Tag>   tags;
    std::vector<Slice> slices;

    ColorRGBA palette[256] = {ColorRGBA(0)};

    /**
     * Get a slice from the 'slices' vector using name and frame.
     * Warning: this method is not really fast.
     * Note: returns slice from other frame, when no slice for specified frame was found.
     */
    const Slice &getSliceByName(const char *name, int frame) const;

    const Tag &getTagByName(const char *name) const;
};

}

#endif
