#include <Audio/AudioSource.hpp>
#include <Audio/AudioAPI.hpp>

#ifdef PLATFORM_WINDOWS
#include <Platform/OpenAL/OpenALAudioSource.hpp>
#elif defined(PLATFORM_ANDROID)

#endif



namespace ag
{
	AG_scope<AudioSource> AudioSource::create()
	{
		switch (AudioAPI::get_api())
		{
		case AudioAPI::API::None: return nullptr;

		case AudioAPI::API::OpenAL:
#ifdef PLATFORM_WINDOWS
			return AG_cscope<OpenALAudioSource>();
#else
			return nullptr;
#endif
		case AudioAPI::API::OpenSL:
		{
#ifdef PLATFORM_ANDROID
			return nullptr; // For now
#else
			return nullptr;
#endif
		};
		default: return nullptr;
		}
	}
}