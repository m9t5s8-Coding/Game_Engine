#pragma once
#include <AL/al.h>
#include <string>

namespace ag
{
	class AudioBuffer
	{
	public:
		AudioBuffer();
		AudioBuffer(const std::string& path);
		~AudioBuffer();

		ALuint get_id() const { return m_ID; }
		const std::string& get_path() const { return m_path; }
		/*ALsizei get_sample_rate() const { return m_sample_rate; }
		int get_channels() const { return m_channels; }*/
		void delete_buffers();
		bool load_sound(const std::string& path);
		bool load_from_memory(const uint8_t* data, size_t size, const std::string& ext);
		bool load_mp3_from_memory(const uint8_t* data, size_t size);
		bool load_wav_from_memory(const uint8_t* data, size_t size);

		static std::vector<uint8_t> read_audio_bytes(const std::string& path);
		static AG_ref<AudioBuffer> create(const std::string& path);

	private:
		bool load_mp3(const std::string& path);
		bool load_wav(const std::string& path);
		ALuint m_ID;
		std::string m_path = "";
	};
}