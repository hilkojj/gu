
#include "audio.h"

au::Sound::~Sound()
{
}

au::SoundSource::SoundSource(const au::Sound &sound)
{
}

au::SoundSource::~SoundSource()
{
}

bool au::SoundSource::isPlaying()
{
    return false;
}

void au::SoundSource::stop()
{
}

void au::SoundSource::play()
{
}

void au::SoundSource::setPitch(float p)
{
}

void au::SoundSource::setVolume(float g)
{
}

void au::SoundSource::setPosition(const vec3 &v)
{
}

void au::SoundSource::setVelocity(const vec3 &v)
{
}

void au::SoundSource::setLooping(bool b)
{
}

void au::SoundSource::pause()
{
}

bool au::SoundSource::hasStopped()
{
    return true;
}

bool au::SoundSource::isPaused()
{
    return false;
}

void au::init()
{
}

void au::terminate()
{
}
