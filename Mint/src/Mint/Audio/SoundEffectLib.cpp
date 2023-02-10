#include "pch.h"
#include "SoundEffectLib.h"
#include <sndfile.h>
#include <inttypes.h>
#include <AL\alext.h>

MT_NAMESPACE_BEGIN

SoundEffectsLibrary* SoundEffectsLibrary::Get()
{
    static SoundEffectsLibrary* sndbuf = new SoundEffectsLibrary();
    return sndbuf;
}

uint32_t SoundEffectsLibrary::Load(const char* filename)
{
	int32_t err, format;
	uint32_t buffer;
	SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	int64_t num_frames;
	int32_t num_bytes;

	sndfile = sf_open(filename, SFM_READ, &sfinfo);
	if (!sndfile)
	{
		MT_ERROR("Could not open audio in {0} {1}", filename, sf_strerror(sndfile));
		return 0;
	}
	if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
	{
		MT_ERROR("Bad sample count in {0} {1}", filename, sfinfo.frames);
		sf_close(sndfile);
		return 0;
	}

	/* Get the sound format, and figure out the OpenAL format */
	format = AL_NONE;
	if (sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;
	else if (sfinfo.channels == 3)
	{
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT2D_16;
	}
	else if (sfinfo.channels == 4)
	{
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT3D_16;
	}
	if (!format)
	{
		MT_ERROR("Unsupported channel count: {0}", sfinfo.channels);
		sf_close(sndfile);
		return 0;
	}

	/* Decode the whole audio file to a buffer. */
	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if (num_frames < 1)
	{
		free(membuf);
		sf_close(sndfile);
		MT_ERROR("Failed to read samples in {0} (%" PRId64 ")", filename, num_frames);
		return 0;
	}
	num_bytes = (int32_t)(num_frames * sfinfo.channels) * (int32_t)sizeof(short);

	/* Buffer the audio data into a new buffer object, then free the data and
	 * close the file.
	 */
	buffer = 0;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndfile);

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR)
	{
		MT_ERROR("OpenAL Error: {0}", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return 0;
	}

	p_SoundEffectBuffers.push_back(buffer);  // add to the list of known buffers

	return buffer;

}

bool SoundEffectsLibrary::UnLoad(uint32_t bufferId)
{
	auto it = p_SoundEffectBuffers.begin();
	while (it != p_SoundEffectBuffers.end())
	{
		if (*it == bufferId)
		{
			alDeleteBuffers(1, &*it);

			it = p_SoundEffectBuffers.erase(it);

			return true;
		}
		else {
			++it;
		}
	}
	return false;  // couldn't find to remove
}

SoundEffectsLibrary::SoundEffectsLibrary()
{
	p_SoundEffectBuffers.clear();
}

SoundEffectsLibrary::~SoundEffectsLibrary()
{
	alDeleteBuffers((int32_t)p_SoundEffectBuffers.size(), p_SoundEffectBuffers.data());

	p_SoundEffectBuffers.clear();
}

MT_NAMESPACE_END
