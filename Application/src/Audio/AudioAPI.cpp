#include <Audio/AudioAPI.hpp>

namespace ag
{
#ifdef PLATFORM_WINDOWS
	AudioAPI::API AudioAPI::s_API = AudioAPI::API::OpenAL;
#elif defined(PLATFORM_ANDROID)
	AudioAPI::API AudioAPI::s_API = AudioAPI::API::OpenSL;
#else
	AudioAPI::API AudioAPI::s_API = AudioAPI::API::None;
#endif
}