#pragma once
#include <AL\al.h>

MT_NAMESPACE_BEGIN

class SoundEffectsLibrary
{
public:
	static SoundEffectsLibrary* Get();

	uint32_t Load(const char* filename);
	bool UnLoad(uint32_t bufferId);

private:
	SoundEffectsLibrary();
	~SoundEffectsLibrary();

	std::vector<ALuint> p_SoundEffectBuffers;
};

MT_NAMESPACE_END
