#include <Aero.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Scripting/ScriptBinding/ScriptBindingHelpers.hpp>

namespace ag {

void ScriptBinding::register_scene() {
  auto&      lua   = ScriptManager::get_lua();
  sol::table scene = lua.create_named_table("Scene");

  scene.set_function("load_from_path", [](const std::string path) -> bool {
    return SceneManager::load_from_path(path);
  });

  scene.set_function("get_active_scene_name",
                     []() -> std::string { return SceneManager::get_active_scene_name(); });

  scene.set_function("load_scene", [](const std::string& name) -> bool {
    return SceneManager::load_scene(name);
  });

  lua.set_function("reload_scene", []() {
    auto        scene_path = Scene::get_active_scene()->get_directory();
    auto        project    = Project::get_active_project();
    std::string path       = project->get_directory() + project->get_scene_directory() + scene_path;

    auto scene = SaveScene::load_scene(path);
    Scene::set_active_scene(scene);
  });
}

}  // namespace ag
