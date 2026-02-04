#ifdef PLATFORM_WINDOWS
#pragma once

#include <Audio/AudioDevice.hpp>
#include <AL/al.h>
#include <AL/alc.h>

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
		ALCdevice* device = nullptr;
		ALCcontext* context = nullptr;
	};
}

#endif