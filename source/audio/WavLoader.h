
#ifndef GAME_WAVLOADER_H
#define GAME_WAVLOADER_H

#include "../files/FileReader.h"
#include "audio.h"

namespace au
{

class WavLoader : public FileReader
{

    Sound &output;
    std::vector<char> data;

  public:
    WavLoader(const char *filePath, Sound &output);

  private:
    void loadHeader();

    void loadData();

    void bufferData();

};

}

#endif
