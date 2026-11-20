#pragma once

#include <Scene/Scene.hpp>
#include <string>
#include <vector>

namespace ag {

class EditorAutosave {
public:
  static std::string directory();
  static std::string path_for(AG_ref<Scene> scene);

  static bool write_scene(AG_ref<Scene> scene, std::string* path_out = nullptr);
  static void remove_scene(const std::string& scene_name);
  static void clear_all();

  static std::vector<std::string> list_files();
  static std::vector<std::string> list_names();
  static bool has_recovery_data();

  // Recovered scenes are loaded as dirty so the user must explicitly save them.
  static AG_ref<Scene> load_recovery(const std::string& file);
};

} // namespace ag
