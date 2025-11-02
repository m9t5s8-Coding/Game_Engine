#pragma once

#include <Core/Core.hpp>
#include <Scene/Scene.hpp>
#include <string>

namespace ag
{
	class SaveScene
	{
	public:
		static void save_scene(AG_ref<Scene>& scene, const std::string& path);
		static AG_ref<Scene> load_scene(const std::string& path);
	};
}