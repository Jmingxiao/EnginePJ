#include "pch.h"
#include "SoundEffect.h"
#include "AudioErrorCheck.h"

MT_NAMESPACE_BEGIN
SoundEffect::SoundEffect()
{
	alGenSources(1, &p_Source);
	alSourcei(p_Source, AL_BUFFER, p_Buffer);
	AL_CheckAndThrow();
}

SoundEffect::~SoundEffect()
{
	alDeleteSources(1, &p_Source);
}

void SoundEffect::Play(unsigned int buffer_to_play)
{
	if (buffer_to_play != p_Buffer)
	{
		p_Buffer = buffer_to_play;
		alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
		AL_CheckAndThrow();
	}
	alSourcePlay(p_Source);
	AL_CheckAndThrow();
}

void SoundEffect::Stop()
{
	alSourceStop(p_Source);
	AL_CheckAndThrow();
}

void SoundEffect::Pause()
{
	alSourcePause(p_Source);
	AL_CheckAndThrow();
}

void SoundEffect::Resume()
{
	alSourcePlay(p_Source);
	AL_CheckAndThrow();
}

void SoundEffect::SetBufferToPlay(unsigned int buffer_to_play)
{
	if (buffer_to_play != p_Buffer)
	{
		p_Buffer = buffer_to_play;
		alSourcei(p_Source, AL_BUFFER, static_cast<int>(p_Buffer));
		AL_CheckAndThrow();
	}
}

void SoundEffect::SetLooping(bool loop)
{
	alSourcei(p_Source, AL_LOOPING, static_cast<int>(loop));
	AL_CheckAndThrow();
}

void SoundEffect::SetPosition(float x, float y, float z)
{
	alSource3f(p_Source, AL_POSITION, x, y, z);
	AL_CheckAndThrow();
}


bool SoundEffect::isPlaying()
{
	int playState;
	alGetSourcei(p_Source, AL_SOURCE_STATE, &playState);
	return (playState == AL_PLAYING);
}

MT_NAMESPACE_END