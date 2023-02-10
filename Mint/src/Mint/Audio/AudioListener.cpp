#include "pch.h"
#include "AudioListener.h"
#include "AudioErrorCheck.h"

MT_NAMESPACE_BEGIN

AudioListener* AudioListener::Instance = nullptr;

void AudioListener::Init()
{
	if (Instance == nullptr)
		Instance = new AudioListener();
}

AudioListener::AudioListener()
{
	m_alCDevice = alcOpenDevice(nullptr); // nullptr = get default device
	if (!m_alCDevice)
		MT_ERROR("failed to get sound device");

	m_alCContext = alcCreateContext(m_alCDevice, nullptr);  // create context
	if (!m_alCContext)
		MT_ERROR("Failed to set sound context");

	if (!alcMakeContextCurrent(m_alCContext))   // make context current
		MT_ERROR("failed to make context current");

	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(m_alCDevice, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(m_alCDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(m_alCDevice) != AL_NO_ERROR)
		name = alcGetString(m_alCDevice, ALC_DEVICE_SPECIFIER);
	MT_INFO("Opened {0}", name);

}

AudioListener::~AudioListener()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(m_alCContext);
	alcCloseDevice(m_alCDevice);
}

MT_NAMESPACE_END