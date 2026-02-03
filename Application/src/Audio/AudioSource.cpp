#include <Audio/AudioSource.hpp>

namespace ag
{
	AudioSource::AudioSource()
		:m_ID(0)
	{
		alGenSources(1, &m_ID);

		if (m_ID == 0)
			AERO_CORE_INFO("Failed to create audio source");

		alSourcef(m_ID, AL_PITCH, 1.0f);
		alSourcef(m_ID, AL_GAIN, 1.0f);
		alSource3f(m_ID, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSource3f(m_ID, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		alSourcei(m_ID, AL_LOOPING, m_looping);
	}
	AudioSource::~AudioSource()
	{
		delete_source();
	}

	void AudioSource::delete_source()
	{
		if (m_ID != 0)
		{
			alSourceStop(m_ID);
			alDeleteSources(1, &m_ID);
			m_ID = 0;
		}
	}

	void AudioSource::set_buffer(const AG_ref<AudioBuffer>& buffer) const
	{
		if (!buffer)
		{
			AERO_CORE_ERROR("There is no buffer");
			return;
		}
		stop();
		alSourcei(m_ID, AL_BUFFER, buffer->get_id());
	}

	void AudioSource::set_buffer(AG_uint buffer_id) const
	{
		if (buffer_id)
		{
			stop();
			alSourcei(m_ID, AL_BUFFER, buffer_id);
		}
	}
	
	void AudioSource::set_pitch(float pitch) const
	{
		alSourcef(m_ID, AL_PITCH, pitch);
	}

	float AudioSource::get_pitch() const
	{
		float pitch;
		alGetSourcef(m_ID, AL_PITCH, &pitch);
		return pitch;
	}

	void AudioSource::play() const
	{
		alSourcePlay(m_ID);
	}
	void AudioSource::pause() const
	{
		alSourcePause(m_ID);
	}
	
	bool AudioSource::is_paused() const
	{
		ALint paused;
		alGetSourcei(m_ID, AL_SOURCE_STATE, &paused);
		return paused == AL_TRUE;
	}



	void AudioSource::stop() const
	{
		alSourceStop(m_ID);
	}

	bool AudioSource::is_playing() const
	{
		ALint state;
		alGetSourcei(m_ID, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}

	void AudioSource::set_loop(bool loop)
	{
		m_looping = loop;
		alSourcei(m_ID, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
	}

	void AudioSource::set_volume(float volume) const
	{
		alSourcef(m_ID, AL_GAIN, volume);
	}
	
	float AudioSource::get_volume() const
	{
		float volume;
		alGetSourcef(m_ID, AL_GAIN, &volume);
		return volume;
	}

	void AudioSource::set_position(const vec2f& position) const
	{
		alSource3f(m_ID, AL_POSITION, position.x, position.y, 0.0f);
	}
}