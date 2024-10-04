
#include "AsepriteTextureGenerator.h"

#include <memory>

SharedTexture aseprite::TextureGenerator::frameToTexture(const Sprite &sprite, int frameI)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    const Frame &frame = sprite.frames.at(frameI);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, sprite.width, sprite.height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &frame.pixels[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return std::make_shared<Texture>(textureID, sprite.width, sprite.height);
}
