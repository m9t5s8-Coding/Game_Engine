#ifdef PLATFORM_WINDOWS

#include <Apch.hpp>
#include <Platform/OpenAL/OpenALAudioBuffer.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#include <dr_mp3.h>
#include <dr_wav.h>
#include <Project/Assetmanager.hpp>

namespace ag
{
	std::string to_lower(std::string s)
	{
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);
		return s;
	}

	OpenALAudioBuffer::OpenALAudioBuffer(const std::string& path)
	{
		load_sound(path);
	}

	OpenALAudioBuffer::~OpenALAudioBuffer()
	{
		delete_buffers();
	}

	void OpenALAudioBuffer::delete_buffers()
	{
		alDeleteBuffers(1, &m_ID);
		m_ID = 0;
	}

	bool OpenALAudioBuffer::load_sound(const std::string& path)
	{
		auto bytes = read_audio_bytes(path);
		if (bytes.empty())
		{
			AERO_CORE_ERROR("Failed to read audio: {0}", path);
			return false;
		}

		auto ext = to_lower(std::filesystem::path(path).extension().string());

		if (ext == ".wav")
			return load_wav_from_memory(bytes.data(), bytes.size());

		if (ext == ".mp3")
			return load_mp3_from_memory(bytes.data(), bytes.size());

		AERO_CORE_ERROR("Unsupported audio format: {0}", ext);
		return false;
	}


	bool OpenALAudioBuffer::load_mp3(const std::string& path)
	{
		std::vector<short> out_data;
		ALenum out_format;
		ALsizei out_sample_rate;

		drmp3* mp3 = new drmp3();
		if (!drmp3_init_file(mp3, path.c_str(), nullptr)) {
			std::cerr << "Failed to open MP3: " << path << std::endl;
			return false;
		}

		uint64_t totalSamples = drmp3_get_pcm_frame_count(mp3) * mp3->channels;
		out_data.resize(totalSamples);

		drmp3_read_pcm_frames_s16(mp3, drmp3_get_pcm_frame_count(mp3), out_data.data());

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

	bool OpenALAudioBuffer::load_mp3_from_memory(const uint8_t* data, size_t size)
	{
		drmp3 mp3;
		if (!drmp3_init_memory(&mp3, data, size, nullptr))
		{
			AERO_CORE_ERROR("Failed to decode MP3 from memory");
			return false;
		}

		uint64_t frameCount = drmp3_get_pcm_frame_count(&mp3);
		std::vector<int16_t> pcm(frameCount * mp3.channels);

		drmp3_read_pcm_frames_s16(&mp3, frameCount, pcm.data());

		ALenum format;
		if (mp3.channels == 1)      format = AL_FORMAT_MONO16;
		else if (mp3.channels == 2) format = AL_FORMAT_STEREO16;
		else
		{
			drmp3_uninit(&mp3);
			AERO_CORE_ERROR("Unsupported MP3 channel count");
			return false;
		}

		alGenBuffers(1, &m_ID);
		alBufferData(
			m_ID,
			format,
			pcm.data(),
			(ALsizei)(pcm.size() * sizeof(int16_t)),
			mp3.sampleRate
		);

		drmp3_uninit(&mp3);
		return true;
	}


	bool OpenALAudioBuffer::load_wav(const std::string& path)
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

	bool OpenALAudioBuffer::load_wav_from_memory(const uint8_t* data, size_t size)
	{
		drwav wav;
		if (!drwav_init_memory(&wav, data, size, nullptr))
		{
			AERO_CORE_ERROR("Failed to decode WAV from memory");
			return false;
		}

		if (wav.translatedFormatTag != DR_WAVE_FORMAT_PCM)
		{
			drwav_uninit(&wav);
			AERO_CORE_ERROR("Unsupported WAV format");
			return false;
		}

		std::vector<int16_t> pcm(wav.totalPCMFrameCount * wav.channels);
		drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pcm.data());

		ALenum format;
		if (wav.channels == 1)      format = AL_FORMAT_MONO16;
		else if (wav.channels == 2) format = AL_FORMAT_STEREO16;
		else
		{
			drwav_uninit(&wav);
			AERO_CORE_ERROR("Unsupported WAV channels");
			return false;
		}

		alGenBuffers(1, &m_ID);
		alBufferData(
			m_ID,
			format,
			pcm.data(),
			(ALsizei)(pcm.size() * sizeof(int16_t)),
			wav.sampleRate
		);

		drwav_uninit(&wav);
		return true;
	}


	std::vector<uint8_t> OpenALAudioBuffer::read_audio_bytes(const std::string& path)
	{
		if (AssetManager::is_packed())
			return AssetManager::read_bytes(path);

		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file)
			return {};

		size_t size = (size_t)file.tellg();
		std::vector<uint8_t> data(size);

		file.seekg(0);
		file.read((char*)data.data(), size);
		return data;
	}
}

#endif