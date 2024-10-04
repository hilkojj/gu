
#ifndef GAME_AUDIO_H
#define GAME_AUDIO_H

#include "../utils/gu_error.h"
#include "../math/math_utils.h" // used for integer types.

#include <vector>
#include <string>

namespace au
{

struct Sound
{
    uint8 channels;
    int32 sampleRate;
    uint8 bitsPerSample;

    ~Sound();
};

struct SoundSource
{
    
    SoundSource(const Sound &sound);

    /**
     * starts playing the sound.
     * if pause() was called before, it will continue where it paused.
     * if stop() was called before, it will start at the beginning
     */
    void play();

    void pause();

    void stop();

    bool isPlaying();

    bool isPaused();

    bool hasStopped();

    void setPitch(float);
    void setVolume(float);
    void setPosition(const vec3&);
    void setVelocity(const vec3&);
    void setLooping(bool);

    ~SoundSource();

};

void init();

void terminate();

}
#endif //GAME_AUDIO_H
