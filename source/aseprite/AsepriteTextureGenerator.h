
#ifndef GAME_ASEPRITETEXTUREGENERATOR_H
#define GAME_ASEPRITETEXTUREGENERATOR_H

#include "Aseprite.h"

#include "../graphics/textures/shared_texture.h"

namespace aseprite
{

SharedTexture frameToTexture(const Sprite &sprite, int frameI);

}

#endif
