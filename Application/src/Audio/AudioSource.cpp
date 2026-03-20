#include <Audio/AudioAPI.hpp>
#include <Audio/AudioSource.hpp>

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
  #include <Platform/OpenAL/OpenALAudioSource.hpp>
#elif defined(PLATFORM_ANDROID)

#endif

namespace ag
{
AG_scope<AudioSource> AudioSource::create()
{
  switch (AudioAPI::get_api())
  {
    case AudioAPI::API::None:
      return nullptr;

    case AudioAPI::API::OpenAL:
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
      return AG_cscope<OpenALAudioSource>();
#else
      return nullptr;
#endif
    case AudioAPI::API::OpenSL:
    {
#ifdef PLATFORM_ANDROID
      return nullptr;  // For now
#else
      return nullptr;
#endif
    };
    default:
      return nullptr;
  }
}
}  // namespace ag
