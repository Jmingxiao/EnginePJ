#pragma once
#include <AL\alc.h>

MT_NAMESPACE_BEGIN

class AudioListener {

public:
	static void Init();

	inline static AudioListener& Get() { Init(); return *Instance; }
private:
	AudioListener();
	~AudioListener();
	static AudioListener* Instance;

	ALCdevice* m_alCDevice;
	ALCcontext* m_alCContext;
};

MT_NAMESPACE_END