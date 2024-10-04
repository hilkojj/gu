
#ifndef GAME_ASEPRITETEXTUREGENERATOR_H
#define GAME_ASEPRITETEXTUREGENERATOR_H

#include "Aseprite.h"

#include "../graphics/textures/texture.h"

namespace aseprite
{

class TextureGenerator
{
  public:
    static SharedTexture frameToTexture(const Sprite &sprite, int frameI);
};

}

#endif
