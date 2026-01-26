#pragma once

#include<AL/al.h>
#include <AL/alc.h>

namespace ag
{
	class Audio_Device
	{
	public:
		Audio_Device();
		~Audio_Device();

		bool init();
		void shutdown();

	private:
		ALCdevice* device = nullptr;
		ALCcontext* context = nullptr;
	};
}