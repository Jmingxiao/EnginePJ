#pragma once
#include <AL\al.h>

MT_NAMESPACE_BEGIN
class SoundEffect
{

public:
	SoundEffect();
	~SoundEffect();

	void Play(unsigned int buffer_to_play);
	void Stop();
	void Pause();
	void Resume();

	void SetBufferToPlay(unsigned int buffer_to_play);
	void SetLooping(bool loop);
	void SetPosition(float x,float y,float z);

	bool isPlaying();

private:
	unsigned int p_Source;
	unsigned int  p_Buffer = 0;

};
MT_NAMESPACE_END