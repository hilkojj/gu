
#include "OggLoader.h"

namespace au { // fixes clash with glm functions
#include "../../external/stb/stb_vorbis.c"
}

void au::OggLoader::load(const char *path, Sound &s)
{
    short *decoded;

    int channels, sampleRate;

    int samples = stb_vorbis_decode_filename(path, &channels, &sampleRate, &decoded);

    if (samples == -1)
        throw gu_err(".ogg file not found");
    else if (samples == -2)
        throw gu_err(".ogg file invalid");

    s.channels = channels;
    s.sampleRate = sampleRate;
    s.bitsPerSample = 16;

    alGenBuffers(1, &s.buffer);

    int format;

    if (s.channels == 1)
        format = AL_FORMAT_MONO16;
    else if (s.channels == 2)
        format = AL_FORMAT_STEREO16;
    else
        throw gu_err("unrecognised .ogg format");

    alCall(alBufferData, s.buffer, format, decoded, samples * s.channels * sizeof(short), s.sampleRate);
    free(decoded);
}
