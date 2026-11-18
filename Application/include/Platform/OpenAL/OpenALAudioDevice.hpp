#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_WEB)
  #pragma once

  #include <AL/al.h>
  #include <AL/alc.h>

  #include <Audio/AudioDevice.hpp>

namespace ag
{
class OpenALAudioDevice : public Audio_Device
{
public:
  OpenALAudioDevice();
  virtual ~OpenALAudioDevice();

  virtual bool init() override;
  virtual void shutdown() override;

private:
  ALCdevice*  device  = nullptr;
  ALCcontext* context = nullptr;
};
}  // namespace ag

#endif
