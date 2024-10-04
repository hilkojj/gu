
#ifndef GAME_ASEPRITELOADER_H
#define GAME_ASEPRITELOADER_H


#include "files/FileReader.h"
#include "Aseprite.h"
#include "math/math_utils.h"

namespace aseprite
{

/**
 *
 * Utility class that can load .ase files created by Aseprite.
 *
 * File format specifications: https://github.com/aseprite/aseprite/blob/master/docs/ase-file-specs.md
 * Based on: https://gist.github.com/NoelFB/778d190e5d17f1b86ebf39325346fcc5
 */
class Loader : public FileReader
{

    const char *filePath;
    Sprite &sprite;

    // https://github.com/aseprite/aseprite/blob/master/docs/ase-file-specs.md#references
    using BYTE = char;
    using WORD = uint16;
    using SHORT = int16;
    using DWORD = uint32;
    using LONG = int32;

  public:
    Loader(const char *filePath, Sprite &output);

  private:

    void loadHeader();

    UserData *lastUserData = NULL;

    void loadFrames();

    void loadLayer();

    void loadCel(Frame &frame, int celEnd);

    void loadPalette();

    void loadUserData();

    void loadFrameTags();

    void loadSlices();

    void celToFrame(Frame &frame, Cel &cel);

    std::string loadString();

    ColorRGBA loadColorRGBA();

    ColorRGB loadColorRGB();

};

enum ChunkType
{
    oldPaletteA = 0x0004,
    oldPaletteB = 0x0011,
    layer = 0x2004,
    cel = 0x2005,
    celExtra = 0x2006,
    mask = 0x2016,
    path = 0x2017,
    frameTags = 0x2018,
    palette = 0x2019,
    userData = 0x2020,
    slice = 0x2022
};

}

#endif
