#include <Project/Project.hpp>
#include <Project/SceneManager.hpp>

namespace ag
{
std::unordered_map<std::string, AG_ref<Scene>> SceneManager::s_scenes;
std::function<void(AG_ref<Scene>)>             SceneManager::scene_changes;

bool SceneManager::load_from_path(const std::string& relative_path)
{
  auto project = Project::get_active_project();
  if (!project)
    return false;

  auto scene_path = project->get_directory() + project->get_scene_directory() + '/' + relative_path;

  auto active_scene = Scene::get_active_scene();
  auto scene        = SaveScene::load_scene(scene_path);

  if (!scene)
    return false;

  if (active_scene != scene)
  {
    s_scenes[scene->get_name()] = scene;
    if (scene_changes)
      scene_changes(scene);

    return true;
  }
  return true;
}

void SceneManager::destroy_scene()
{
  for (auto& [name, scene] : s_scenes)
  {
    scene->destroy();
  }
}

bool SceneManager::load_scene(const std::string& scene_name)
{
  auto it = s_scenes.find(scene_name);
  if (it == s_scenes.end())
    return false;

  auto active_scene = Scene::get_active_scene();
  auto scene        = it->second;
  if (scene)
  {
    Scene::set_active_scene(scene);
    if (scene && active_scene != scene)
    {
      if (scene_changes)
        scene_changes(scene);
    }
    return true;
  }
  return false;
}

void SceneManager::add_scene(AG_ref<Scene> scene)
{
  if (!scene)
    return;

  const auto& name = scene->get_name();

  s_scenes[name] = scene;
}

AG_ref<Scene> SceneManager::get_scene(const std::string& name)
{
  auto it = s_scenes.find(name);
  if (it == s_scenes.end())
    return nullptr;

  return it->second;
}

AG_ref<Scene> SceneManager::get_active_scene()
{
  return Scene::get_active_scene();
}

std::string SceneManager::get_active_scene_name()
{
  auto scene = SceneManager::get_active_scene();
  if (scene)
  {
    return scene->get_name();
  }
  return "";
}

void SceneManager::erase_scene(const std::string& name)
{
  s_scenes.erase(name);
}

void SceneManager::erase_scene(AG_ref<Scene> scene)
{
  if (!scene)
    return;
  const auto& name = scene->get_name();
  erase_scene(name);
}

void SceneManager::set_active_scene(AG_ref<Scene> scene)
{
  if (scene)
  {
    const auto& name  = scene->get_name();
    s_scenes[name]    = scene;
    auto active_scene = Scene::get_active_scene();
    Scene::set_active_scene(scene);
    if (scene != active_scene)
    {
      if (scene_changes)
        scene_changes(scene);
    }
  }
}

void SceneManager::load_scene_scripts(AG_ref<Scene> scene)
{
  SaveScene::load_scene_scripts(scene);
}
}  // namespace ag
