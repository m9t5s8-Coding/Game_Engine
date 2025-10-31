#pragma once

#include <Core/Core.hpp>
#include <Scene/Scene.hpp>
#include <string>

namespace ag
{
	class SaveScene
	{
	public:
		static void save_scene(const AG_ref<Scene>& scene, const std::string& path);
		static AG_ref<Scene> load_scene(const std::string& path);

		static AG_ref<Scene> get_active_scene() { return s_active_scene; }
		static void set_active_scene(const AG_ref<Scene>& scene) { s_active_scene = scene; }

	private:
		static AG_ref<Scene> s_active_scene;
	};
}