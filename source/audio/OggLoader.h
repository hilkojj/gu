#pragma once

#include "audio.h"

namespace au
{

class OggLoader
{
  public:
    static void load(const char *, Sound &);
};

}
