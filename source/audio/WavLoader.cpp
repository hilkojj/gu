
#include "WavLoader.h"

/**
 * based on https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/
 */

au::WavLoader::WavLoader(const char *filePath, au::Sound &output) : FileReader(filePath), output(output)
{
    loadHeader();
    loadData();
    bufferData();
}

void au::WavLoader::loadHeader()
{
    std::vector<char> buffer;

    read(4, buffer); // the RIFF
    if (std::strncmp(&buffer[0], "RIFF", 4) != 0)
        throw gu_err("header doesn't begin with RIFF");

    skip(4); // the size of the file

    buffer.clear();
    read(4, buffer); // the WAVE
    if (std::strncmp(&buffer[0], "WAVE", 4) != 0)
        throw gu_err("header doesn't contain WAVE");


    skip(4); // "fmt/0"

    skip(4); // size of fmt data chunk

    skip(2); // PCM should be 1?

    // the number of channels
    buffer.clear();
    read(2, buffer);
    if (buffer.size() != 2)
        throw gu_err("could not read number of channels");

    output.channels = 0;
    memcpy(&output.channels, &buffer[0], 2);

    // sample rate
    buffer.clear();
    read(4, buffer);
    if (buffer.size() != 4)
        throw gu_err("could not read sample rate");

    output.sampleRate = 0;
    memcpy(&output.sampleRate, &buffer[0], 4);

    skip(4); // (sampleRate * bitsPerSample * channels) / 8

    skip(2); // ?? dafaq

    // bitsPerSample
    buffer.clear();
    read(2, buffer);
    if (buffer.size() != 2)
        throw gu_err("could not read bits per sample");

    output.bitsPerSample = 0;
    memcpy(&output.bitsPerSample, &buffer[0], 2);

    // data chunk header "data"
    buffer.clear();
    read(4, buffer); // the WAVE
    if (std::strncmp(&buffer[0], "data", 4) != 0)
        throw gu_err("WAVE file doesn't have 'data' tag");
}

void au::WavLoader::loadData()
{
    // size of data
    int size = read<int32>();

    if (!hasNMoreBytes(size))
        throw gu_err("size incorrect");

    read(size, data);
}

void au::WavLoader::bufferData()
{
    /* TODO
    alCall(alGenBuffers, 1, &output.buffer);

    ALenum format;
    if (output.channels == 1 && output.bitsPerSample == 8)
        format = AL_FORMAT_MONO8;
    else if (output.channels == 1 && output.bitsPerSample == 16)
        format = AL_FORMAT_MONO16;
    else if (output.channels == 2 && output.bitsPerSample == 8)
        format = AL_FORMAT_STEREO8;
    else if (output.channels == 2 && output.bitsPerSample == 16)
        format = AL_FORMAT_STEREO16;
    else
        throw gu_err("unrecognised wave format");

    alCall(alBufferData, output.buffer, format, data.data(), data.size(), output.sampleRate);
     */
}
