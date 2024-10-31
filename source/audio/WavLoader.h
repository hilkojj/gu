#pragma once

#include "audio.h"

#include "../files/FileReader.h"

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
