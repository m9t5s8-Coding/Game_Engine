#include <Audio/AudioAPI.hpp>
#include <Audio/AudioSource.hpp>

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_ANDROID)
  #include <Platform/OpenAL/OpenALAudioSource.hpp>

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
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_ANDROID)
      return AG_cscope<OpenALAudioSource>();
#else
      return nullptr;
#endif

    default:
      return nullptr;
  }
}
}  // namespace ag
