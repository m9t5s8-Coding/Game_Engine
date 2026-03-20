#include <Audio/AudioAPI.hpp>
#include <Audio/AudioBuffer.hpp>

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
  #include <Platform/OpenAL/OpenALAudioBuffer.hpp>
#endif

namespace ag
{
AG_ref<AudioBuffer> AudioBuffer::create(const std::string& path)
{
  switch (AudioAPI::get_api())
  {
    case AudioAPI::API::None:
      return nullptr;
    case AudioAPI::API::OpenAL:
    {
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
      return AG_cref<OpenALAudioBuffer>(path);
#else
      return nullptr;
#endif
    }
    default:
      return nullptr;
  }
}
}  // namespace ag
