#ifdef PLATFORM_WINDOWS

#pragma once

#include <Audio/AudioBuffer.hpp>

namespace ag
{
	class OpenALAudioBuffer : public AudioBuffer
	{
	public:
		OpenALAudioBuffer(const std::string& path);
		virtual ~OpenALAudioBuffer();

		virtual void delete_buffers() override;

		virtual AG_uint get_id() const override { return m_ID; }
		virtual const std::string& get_path() const override { return m_path; }


		bool load_sound(const std::string& path);
		bool load_mp3_from_memory(const uint8_t* data, size_t size);
		bool load_wav_from_memory(const uint8_t* data, size_t size);

		static std::vector<uint8_t> read_audio_bytes(const std::string& path);

	private:
		bool load_mp3(const std::string& path);
		bool load_wav(const std::string& path);
		AG_uint m_ID;
		std::string m_path = "";
	};
}

#endif