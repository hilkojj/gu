
#ifndef GAME_OGGLOADER_H
#define GAME_OGGLOADER_H

#include "audio.h"

namespace au
{

class OggLoader
{
  public:
    static void load(const char *, Sound &);
};

}
#endif
