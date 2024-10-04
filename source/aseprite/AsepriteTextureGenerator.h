
#ifndef GAME_ASEPRITETEXTUREGENERATOR_H
#define GAME_ASEPRITETEXTUREGENERATOR_H

#include "graphics/texture.h"
#include "Aseprite.h"

namespace aseprite
{

class TextureGenerator
{
  public:
    static SharedTexture frameToTexture(const Sprite &sprite, int frameI);
};

}

#endif
