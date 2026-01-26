#pragma once

#include<Audio/AudioBuffer.hpp>
#include<Math/Math.hpp>

namespace ag
{
	class AudioSource
	{
	public:
		AudioSource();
		~AudioSource();

		void set_buffer(const AG_ref<AudioBuffer>& buffer) const;
		void set_buffer(AG_uint buffer_id) const;
		void set_pitch(float pitch) const;
		float get_pitch() const;
		void play() const;
		void pause() const;
		bool is_paused() const;
		void stop() const;
		bool is_playing() const;
		void set_loop(bool loop);
		bool is_looping() const { return m_looping; };
		void set_volume(float volume) const;
		float get_volume() const;
		void set_position(const vec2f& position) const;

	private:
		AG_uint m_ID;
		bool m_looping = false;
	};
}