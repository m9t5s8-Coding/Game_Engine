#include <Audio/AudioDevice.hpp>
#include <Core/Log.hpp>
#include<Audio/AudioManager.hpp>


namespace ag
{
	Audio_Device::Audio_Device()
	{
		
	}

	Audio_Device::~Audio_Device()
	{
		shutdown();
	}

	bool Audio_Device::init()
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

	void Audio_Device::shutdown()
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

		AERO_CORE_INFO("Audio Shutdown");
	}
}