#include <Apch.hpp>
#include<Audio/AudioBuffer.hpp>
#include <dr_mp3.h>
#include <dr_wav.h>

namespace ag
{

	std::string to_lower(std::string s)
	{
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);
		return s;
	}


	AudioBuffer::AudioBuffer()
		:m_ID(0)
	{

	}

	AudioBuffer::AudioBuffer(const std::string& path)
	{
		load_sound(path);
	}

	AudioBuffer::~AudioBuffer()
	{
		delete_buffers();
	}
	void AudioBuffer::delete_buffers()
	{
		alDeleteBuffers(1, &m_ID);
		m_ID = 0;
	}

	bool AudioBuffer::load_sound(const std::string& path)
	{
		auto ext = to_lower(std::filesystem::path(path).extension().string());

		if (ext == ".wav") return load_wav(path);
		else if (ext == ".mp3") return load_mp3(path);
		else AERO_CORE_INFO("Unsupported Format"); return false;
	}


	bool AudioBuffer::load_mp3(const std::string& path)
	{
		std::vector<short> out_data;
		ALenum out_format;
		ALsizei out_sample_rate;

		drmp3* mp3 = new drmp3();
		if (!drmp3_init_file(mp3, path.c_str(), nullptr)) {
			std::cerr << "Failed to open MP3: " << path << std::endl;
			return false;
		}

		// Get total samples
		uint64_t totalSamples = drmp3_get_pcm_frame_count(mp3) * mp3->channels;
		out_data.resize(totalSamples);

		// Decode into PCM (16-bit signed)
		drmp3_read_pcm_frames_s16(mp3, drmp3_get_pcm_frame_count(mp3), out_data.data());

		// Determine OpenAL format
		if (mp3->channels == 1) out_format = AL_FORMAT_MONO16;
		else if (mp3->channels == 2) out_format = AL_FORMAT_STEREO16;
		else {
			std::cerr << "Unsupported channel count: " << mp3->channels << std::endl;
			drmp3_uninit(mp3);
			return false;
		}

		out_sample_rate = mp3->sampleRate;
		drmp3_uninit(mp3);


		alGenBuffers(1, &m_ID);
		alBufferData(m_ID, out_format, out_data.data(),
			static_cast<ALsizei>(out_data.size() * sizeof(short)), out_sample_rate);


		delete mp3;
		return true;
	}

	bool AudioBuffer::load_wav(const std::string& path)
	{
		drwav wav;
		if (!drwav_init_file(&wav, path.c_str(), nullptr))
			throw std::runtime_error("Failed to load WAV file");

		if (wav.translatedFormatTag != DR_WAVE_FORMAT_PCM)
		{
			drwav_uninit(&wav);
			throw std::runtime_error("Unsupported WAV format");
			return false;
		}

		std::vector<int16_t> pcmData(wav.totalPCMFrameCount * wav.channels);
		drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pcmData.data());

		ALenum format;
		if (wav.channels == 1)
			format = AL_FORMAT_MONO16;
		else if (wav.channels == 2)
			format = AL_FORMAT_STEREO16;
		else
		{
			throw std::runtime_error("Unsupported channel count");
			return false;
		}

		
		

		alGenBuffers(1, &m_ID);
		alBufferData(
			m_ID,
			format,
			pcmData.data(),
			pcmData.size() * sizeof(int16_t),
			wav.sampleRate
		);
		drwav_uninit(&wav);

		return true;
	}


	AG_ref<AudioBuffer> AudioBuffer::create(const std::string& path)
	{
		auto buffer = AG_cref<AudioBuffer>(path);
		return buffer;
	}
}