#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_ANDROID)

  #include <Audio/AudioManager.hpp>
  #include <Platform/OpenAL/OpenALAudioDevice.hpp>

namespace ag
{
OpenALAudioDevice::OpenALAudioDevice()
{
}

OpenALAudioDevice::~OpenALAudioDevice()
{
  shutdown();
}

bool OpenALAudioDevice::init()
{
  device = alcOpenDevice(nullptr);
  if (!device)
  {
    AERO_CORE_WARN("Failed to Open Device");
    return false;
  }

  context = alcCreateContext(device, nullptr);
  if (!context)
  {
    AERO_CORE_WARN("Failed to create context");
    return false;
  }

  if (!alcMakeContextCurrent(context))
  {
    AERO_CORE_WARN("Failed to make current context");
    return false;
  }

  AERO_CORE_INFO("OpenAL Initialized");
  return true;
}

void OpenALAudioDevice::shutdown()
{
  AudioManager::clear();
  if (context)
  {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    context = nullptr;
  }

  if (device)
  {
    alcCloseDevice(device);
    device = nullptr;
  }
}

Audio_Device* Audio_Device::create()
{
  return new OpenALAudioDevice();
}

}  // namespace ag

#endif
