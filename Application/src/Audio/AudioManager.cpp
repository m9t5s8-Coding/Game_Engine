#include <Audio/AudioManager.hpp>


namespace ag
{
	AG_uint AudioManager::load(const std::string& path)
	{
		auto it = s_path_to_id.find(path);
		if (it != s_path_to_id.end())
			return it->second;


		auto buffer = AudioBuffer::create(path);
		auto id = buffer->get_id();
		s_path_to_id[path] = id;
		s_buffers[id] = buffer;
		return id;
	}

	void AudioManager::clear()
	{
		s_path_to_id.clear();
		for (const auto& [id, buffers] : s_buffers)
		{
			buffers->delete_buffers();
		}
	}
}