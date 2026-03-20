#pragma once

#include <functional>
#include <Project/SaveScene.hpp>
#include <string>
#include <unordered_map>

namespace ag
{
class SceneManager
{
public:
  static bool          load_from_path(const std::string& relative_path);
  static bool          load_scene(const std::string& scene_name);
  static void          add_scene(AG_ref<Scene> scene);
  static AG_ref<Scene> get_scene(const std::string& name);
  static void          erase_scene(const std::string& name);
  static void          erase_scene(AG_ref<Scene> scene);
  static void          load_scene_scripts(AG_ref<Scene> scene);
  static AG_ref<Scene> get_active_scene();
  static std::string   get_active_scene_name();

  static void set_active_scene(AG_ref<Scene> scene);

  static std::function<void(AG_ref<Scene>)> scene_changes;

private:
  static std::unordered_map<std::string, AG_ref<Scene>> s_scenes;
};
}  // namespace ag
