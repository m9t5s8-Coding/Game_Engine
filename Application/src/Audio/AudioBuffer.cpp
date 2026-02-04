#include<Audio/AudioBuffer.hpp>
#include <Audio/AudioAPI.hpp>

#ifdef PLATFORM_WINDOWS
#include <Platform/OpenAL/OpenALAudioBuffer.hpp>
#endif


namespace ag
{
	AG_ref<AudioBuffer> AudioBuffer::create(const std::string& path)
	{
		switch (AudioAPI::get_api())
		{
		case AudioAPI::API::None: return nullptr;
		case AudioAPI::API::OpenAL:
		{
#ifdef  PLATFORM_WINDOWS
			return AG_cref<OpenALAudioBuffer>(path);
#else
			return nullptr
#endif
		}
		default:
			break;
		}
	}
}
