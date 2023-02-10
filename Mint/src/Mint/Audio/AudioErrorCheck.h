#pragma once
#include"Mint/Core/Base.h"
#include <AL/alc.h>
#include <AL/al.h>

MT_NAMESPACE_BEGIN
static void ALC_CheckAndThrow(ALCdevice* device)
{
	if (alcGetError(device) != ALC_NO_ERROR)
		MT_ERROR("error with alcDevice");
}

static void AL_CheckAndThrow()
{
	if (alGetError() != AL_NO_ERROR)
		MT_ERROR("error with al");
}
MT_NAMESPACE_END