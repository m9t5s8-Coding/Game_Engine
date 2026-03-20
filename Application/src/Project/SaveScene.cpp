#include <Apch.hpp>
#include <GameObjects/Components/Components.hpp>
#include <GameObjects/NodeFactory.hpp>
#include <Helper.hpp>
#include <Project/Assetmanager.hpp>
#include <Project/SaveScene.hpp>
#include <Scene/Entity.hpp>
#include <Scene/SceneComponent.hpp>

namespace fs = std::filesystem;

namespace ag
{
void SaveScene::save_scene(AG_ref<Scene>& scene, const std::string& path)
{
  if (!scene->is_save_required())
    return;

  json j;
  Helper::save_json(j["Scene"], "Name", scene->get_name(), std::string(""));
  Helper::save_json(j["Scene"], "Path", scene->get_directory(), std::string(""));

  Scene::set_active_scene(scene);

  j["Scene"]["Entities"] = json::array();

  {
    json  root_json = json::array();
    auto& roots     = scene->get_root_entity();
    for (auto id : roots)
    {
      Entity e(id);
      auto   index = e.get_component<Tag_Component>().index;
      root_json.push_back(index);
    }
    j["Scene"]["Roots"] = root_json;
  }

  auto view = scene->m_registry.view<Tag_Component>();
  if (!view.empty())
  {
    for (auto entityID : view)
    {
      if (!scene->m_registry.valid(entityID))
        continue;

      Entity e(entityID);

      if (!e.has_component<Tag_Component>())
        continue;

      const auto& tag        = e.get_component<Tag_Component>();
      json        entityjson = Tag_Component::save_json(e);
      if (!entityjson.is_object())
      {
        entityjson = json::object();
      }

      auto it = NodeFactory::save_map.find(tag.node_type);
      if (it != NodeFactory::save_map.end())
      {
        json nodejson = it->second(e);

        if (!nodejson.is_object())
        {
          nodejson = json::object();
        }

        entityjson.update(nodejson);
      }
      j["Scene"]["Entities"].push_back(entityjson);
    }
  }

  Helper::makefile_read_only(path, false);
  std::ofstream file(path, std::ios::trunc);
  if (!file.is_open())
  {
    AERO_CORE_INFO("Failed to Open File for Saving: {}", path);
    return;
  }

  file << j.dump(4);
  file.close();
  AERO_CORE_INFO("Scene Saved! {0}  {1}", scene->get_name(), scene->get_directory());
  scene->set_save_required(false);
  Helper::makefile_read_only(path);
}

AG_ref<Scene> SaveScene::load_scene(const std::string& path, bool load_scripts)
{
  AG_ref<Scene> scene = Scene::create();

  json j;

  if (AssetManager::is_packed(AssetManager::Domain::Project))
  {
    j = AssetManager::read_json(path, AssetManager::Domain::Project);
  }
  else
  {
    Helper::makefile_read_only(path, false);
    std::ifstream file(path);
    if (!file.is_open())
    {
      AERO_CORE_ERROR("Failed to Open File!");
      return Scene::create("default");
    }

    file >> j;
    file.close();
    Helper::makefile_read_only(path);
  }

  bool root_available = false;

  std::string scene_name, scene_path;
  Helper::load_json(j["Scene"], "Name", scene_name, std::string(""));
  Helper::load_json(j["Scene"], "Path", scene_path, std::string(""));
  scene->set_name(scene_name);
  scene->set_directory(scene_path);

  auto active_scene = Scene::get_active_scene();
  Scene::set_active_scene(scene);
  std::unordered_map<AG_uint, Entity> id_map;

  // Load and Create Entity
  for (auto& entityjson : j["Scene"]["Entities"])
  {
    NodeType    type;
    std::string name;
    {
      int node;
      Helper::load_json(entityjson, "NodeType", node, (int)NodeType::None);
      Helper::load_json(entityjson, "Name", name, std::string(""));

      type = static_cast<NodeType>(node);
    }
    Entity e = scene->create_entity(name, type);

    Tag_Component::load_json(e, entityjson);
    {
      auto& tag = e.get_component<Tag_Component>();
      scene->set_next_index(std::max(scene->get_index(), tag.index + 1));
      id_map[tag.index] = e;
    }
  }

  // Load Childrens
  {
    auto view = scene->m_registry.view<Tag_Component>();
    for (auto entityID : view)
    {
      Entity e(entityID);
      Tag_Component::load_children(e);
    }
  }

  // Load Entity Data
  for (auto& entityjson : j["Scene"]["Entities"])
  {
    AG_uint id;
    Helper::load_json(entityjson, "ID", id, (AG_uint)0);
    auto it_map = id_map.find(id);
    if (it_map == id_map.end())
    {
      AERO_CORE_ERROR("Entity ID {0} not found in id_map — skipping", id);
      continue;
    }
    Entity e = it_map->second;
    int    node;
    Helper::load_json(entityjson, "NodeType", node, (int)NodeType::None);
    NodeType type = static_cast<NodeType>(node);

    auto it = NodeFactory::load_map.find(type);
    if (it != NodeFactory::load_map.end())
    {
      it->second(e, entityjson);
    }
  }

  // Make Roots Entities
  if (j["Scene"].contains("Roots") && j["Scene"]["Roots"].is_array())
  {
    const auto& root_ids = j["Scene"]["Roots"];
    root_available       = true;
    for (auto& id_json : root_ids)
    {
      AG_uint root_id = id_json.get<AG_uint>();
      auto    it      = id_map.find(root_id);
      if (it == id_map.end())
      {
        continue;
      }

      scene->push_back_root(it->second.get_id());
    }
  }

  scene->set_root_available(root_available);

  // Load Scripts
  if (load_scripts)
  {
    load_scene_scripts(scene);
  }

  id_map.clear();
  index_map.clear();
  scene->set_save_required(false);
  AERO_CORE_INFO("Scene Loaded! {0}  {1}", scene->get_name(), scene->get_directory());

  if (active_scene)
    Scene::set_active_scene(active_scene);

  return scene;
}

void SaveScene::load_scene_scripts(AG_ref<Scene> scene)
{
  if (!scene)
    return;

  Scene::set_active_scene(scene);
  if (Engine::is_runtime())
  {
    auto view = scene->m_registry.view<Script_Component>();
    for (auto entityID : view)
    {
      Entity e(entityID);
      Script_Component::load_scripts(e);
    }
  }
}

}  // namespace ag
