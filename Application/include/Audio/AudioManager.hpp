#ifdef PLATFORM_WINDOWS

#pragma once

#include <Audio/AudioSource.hpp>
#include <unordered_map>

namespace ag
{
	class AudioManager
	{
	public:
		static AG_uint load(const std::string& path);
		static void clear();

	private:
		inline static std::unordered_map<std::string, AG_uint> s_path_to_id;
		inline static std::unordered_map<AG_uint, AG_ref<AudioBuffer>> s_buffers;
	};
}

#endif