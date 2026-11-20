#include <Apch.hpp>

#include <GameObjects/Components/Components.hpp>
#include <GameObjects/NodeFactory.hpp>
#include <Project/Assetmanager.hpp>
#include <Project/Project.hpp>
#include <Project/SceneInstance.hpp>
#include <Scene/Entity.hpp>
#include <Scene/SceneComponent.hpp>
#include <algorithm>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace ag {
namespace {

std::string without_leading_slash(std::string value) {
  Helper::normalize_path(value);
  while (!value.empty() && value.front() == '/')
    value.erase(value.begin());
  return value;
}

json read_scene_json(const std::string& path) {
  if (path.empty())
    return {};

  if (AssetManager::is_packed(AssetManager::Domain::Project))
    return AssetManager::read_json(path, AssetManager::Domain::Project);

  std::ifstream file(path);
  if (!file.is_open())
    return {};
  json j;
  file >> j;
  return j;
}

struct SourceSceneData {
  std::vector<json>    entities;
  std::vector<AG_uint> roots;
};

bool collect_source_entities(const json& scene_json, AG_uint source_root, SourceSceneData& out) {
  if (!scene_json.is_object() || !scene_json.contains("Scene") ||
      !scene_json["Scene"].contains("Entities") || !scene_json["Scene"]["Entities"].is_array())
    return false;

  const auto& entities = scene_json["Scene"]["Entities"];
  std::unordered_map<AG_uint, json>               by_id;
  std::unordered_map<AG_uint, std::vector<AG_uint>> children;
  std::vector<AG_uint>                              all_ids;

  for (const auto& entity_json : entities) {
    AG_uint id = entity_json.value("ID", (AG_uint)0);
    if (id == 0)
      continue;
    by_id[id] = entity_json;
    all_ids.push_back(id);

    if (entity_json.contains("Parent") && !entity_json["Parent"].is_null()) {
      AG_uint parent = entity_json["Parent"].get<AG_uint>();
      children[parent].push_back(id);
    }
    if (entity_json.contains("Children") && entity_json["Children"].is_array()) {
      for (const auto& child : entity_json["Children"]) {
        AG_uint child_id = child.get<AG_uint>();
        if (child_id != 0)
          children[id].push_back(child_id);
      }
    }
  }

  for (auto& [id, kids] : children) {
    (void)id;
    std::sort(kids.begin(), kids.end());
    kids.erase(std::unique(kids.begin(), kids.end()), kids.end());
  }

  std::vector<AG_uint> roots;
  if (source_root == 0) {
    if (scene_json["Scene"].contains("Roots") && scene_json["Scene"]["Roots"].is_array()) {
      for (const auto& root : scene_json["Scene"]["Roots"])
        roots.push_back(root.get<AG_uint>());
    } else {
      for (AG_uint id : all_ids) {
        const auto& entity_json = by_id[id];
        if (!entity_json.contains("Parent") || entity_json["Parent"].is_null())
          roots.push_back(id);
      }
    }
  } else {
    roots.push_back(source_root);
  }

  std::unordered_set<AG_uint> visited;
  std::function<void(AG_uint)> visit = [&](AG_uint id) {
    if (!by_id.count(id) || !visited.insert(id).second)
      return;
    out.entities.push_back(by_id[id]);
    auto it = children.find(id);
    if (it == children.end())
      return;
    for (AG_uint child : it->second)
      visit(child);
  };

  for (AG_uint root : roots)
    visit(root);

  if (out.entities.empty())
    return false;

  for (AG_uint root : roots) {
    if (visited.count(root))
      out.roots.push_back(root);
  }
  return true;
}

void load_entity_components(Entity entity, const json& entity_json) {
  int node_type = static_cast<int>(NodeType::None);
  Helper::load_json(entity_json, "NodeType", node_type, static_cast<int>(NodeType::None));
  auto it = NodeFactory::load_map.find(static_cast<NodeType>(node_type));
  if (it != NodeFactory::load_map.end())
    it->second(entity, const_cast<json&>(entity_json));
}

void expand_instance(AG_ref<Scene>     host,
                     Entity            instance_root,
                     AG_uint           owner,
                     const std::string& source_scene,
                     AG_uint           source_root,
                     std::vector<std::string>& source_stack,
                     std::vector<Entity>&      created);

void load_scripts_for(const std::vector<Entity>& entities) {
#ifdef AERO_RUNTIME
  for (Entity entity : entities) {
    if (entity && entity.has_component<Script_Component>())
      Script_Component::load_scripts(entity);
  }
#else
  (void)entities;
#endif
}

}  // namespace

std::string SceneInstance::normalize_source_path(const std::string& path) {
  std::string normalized = without_leading_slash(path);
  if (normalized.empty())
    return {};
  return "/" + normalized;
}

std::string SceneInstance::resolve_source_path(const std::string& project_relative) {
  auto project = Project::get_active_project();
  if (!project)
    return {};

  const std::string relative = without_leading_slash(project_relative);
  if (relative.empty())
    return {};

  if (AssetManager::is_packed(AssetManager::Domain::Project)) {
    std::string root = project->get_directory() + project->get_scene_directory();
    while (!root.empty() && root.back() == '/')
      root.pop_back();
    return root + "/" + relative;
  }

  return project->resolve_relative_path("/" + relative, ProjectDirectory::Scenes);
}

bool SceneInstance::is_instance_root(Entity entity) {
  return entity && entity.has_component<SceneInstance_Component>();
}

Entity SceneInstance::get_instance_root(Entity entity) {
  Entity current = entity;
  std::unordered_set<AG_uint> visited;
  while (current && current.has_component<Tag_Component>()) {
    if (!visited.insert(current.get_id()).second)
      return Entity();
    if (current.has_component<SceneInstance_Component>())
      return current;
    current = current.get_component<Tag_Component>().parent;
  }
  return Entity();
}

bool SceneInstance::is_generated(Entity entity) {
  return entity && entity.has_component<Tag_Component>() &&
         entity.get_component<Tag_Component>().instance_owner != INVALID_ENTITY;
}

bool SceneInstance::is_read_only(Entity entity) {
  Entity current = entity;
  std::unordered_set<AG_uint> visited;
  while (current && current.has_component<Tag_Component>()) {
    if (!visited.insert(current.get_id()).second)
      return false;
    if (is_generated(current))
      return true;
    current = current.get_component<Tag_Component>().parent;
  }
  return false;
}

void SceneInstance::clear_instance_contents(AG_ref<Scene> host, Entity instance_root) {
  if (!host || !instance_root || !instance_root.has_component<Tag_Component>())
    return;

  const AG_uint owner = instance_root.get_component<Tag_Component>().index;
  std::vector<Entity> generated;
  std::vector<AG_uint> indices;
  auto view = host->m_registry.view<Tag_Component>();
  for (auto entity_id : view) {
    Entity entity(entity_id);
    const auto& tag = entity.get_component<Tag_Component>();
    if (tag.instance_owner != owner)
      continue;
    generated.push_back(entity);
    indices.push_back(tag.index);
  }

  // Destroy only roots of the generated forest; Scene::clear_destroyed_entity
  // recursively destroys their children.
  for (Entity entity : generated) {
    const auto& tag   = entity.get_component<Tag_Component>();
    Entity      parent = tag.parent;
    if (parent && parent.has_component<Tag_Component>() &&
        parent.get_component<Tag_Component>().instance_owner == owner)
      continue;
    host->destroy_entity(entity);
  }
  host->clear_destroyed_entity();

  for (AG_uint index : indices)
    index_map.erase(index);
}

Entity SceneInstance::create_instance(AG_ref<Scene>    host,
                                      const std::string& source_scene,
                                      Entity            parent) {
  if (!host || source_scene.empty())
    return Entity();

  const std::string relative = normalize_source_path(source_scene);
  std::string name = std::filesystem::path(without_leading_slash(relative)).stem().string();
  if (name.empty())
    name = "Scene Instance";

  Entity instance = host->create_entity(name, NodeType::SceneInstance);
  auto& comp = instance.get_component<SceneInstance_Component>();
  comp.source_scene = relative;
  comp.source_name  = name;
  comp.instance_id  = instance.get_component<Tag_Component>().index;

  if (parent && parent.has_component<Tag_Component>()) {
    instance.get_component<Tag_Component>().parent = parent;
    parent.get_component<Tag_Component>().children.push_back(instance);
  } else {
    host->get_root_entity().push_back(instance.get_id());
  }

  refresh_instance(host, instance, true);
  host->set_save_required(true);
  return instance;
}

void SceneInstance::refresh_instance(AG_ref<Scene> host, Entity instance_root, bool load_scripts) {
  if (!host || !instance_root || !instance_root.has_component<SceneInstance_Component>())
    return;

  auto& comp = instance_root.get_component<SceneInstance_Component>();
  if (comp.instance_id == 0)
    comp.instance_id = instance_root.get_component<Tag_Component>().index;

  clear_instance_contents(host, instance_root);

  std::vector<std::string> stack;
  if (host->has_directory())
    stack.push_back(without_leading_slash(host->get_directory()));
  std::vector<Entity> created;
  expand_instance(host,
                  instance_root,
                  comp.instance_id,
                  comp.source_scene,
                  comp.source_root,
                  stack,
                  created);
  load_scripts_for(created);
}

void SceneInstance::refresh_all(AG_ref<Scene> host, bool load_scripts) {
  if (!host)
    return;

  std::vector<Entity> roots;
  auto view = host->m_registry.view<Tag_Component>();
  for (auto entity_id : view) {
    Entity entity(entity_id);
    if (!entity.has_component<SceneInstance_Component>())
      continue;
    // Generated nested instances are rebuilt by their top-level owner.
    if (entity.get_component<Tag_Component>().instance_owner != INVALID_ENTITY)
      continue;
    roots.push_back(entity);
  }
  for (Entity root : roots)
    refresh_instance(host, root, load_scripts);
}

void SceneInstance::refresh_source(AG_ref<Scene>    host,
                                   const std::string& source_scene,
                                   bool              load_scripts) {
  if (!host)
    return;

  const std::string wanted = without_leading_slash(source_scene);
  std::vector<Entity> roots;
  auto view = host->m_registry.view<Tag_Component>();
  for (auto entity_id : view) {
    Entity entity(entity_id);
    if (!entity.has_component<SceneInstance_Component>())
      continue;
    if (entity.get_component<Tag_Component>().instance_owner != INVALID_ENTITY)
      continue;
    const auto& comp = entity.get_component<SceneInstance_Component>();
    if (without_leading_slash(comp.source_scene) == wanted)
      roots.push_back(entity);
  }
  for (Entity root : roots)
    refresh_instance(host, root, load_scripts);
}

namespace {

void expand_instance(AG_ref<Scene>     host,
                     Entity            instance_root,
                     AG_uint           owner,
                     const std::string& source_scene,
                     AG_uint           source_root,
                     std::vector<std::string>& source_stack,
                     std::vector<Entity>&      created) {
  if (!host || !instance_root || source_scene.empty())
    return;

  auto& comp = instance_root.get_component<SceneInstance_Component>();
  if (!comp.enabled)
    return;

  const std::string normalized = without_leading_slash(source_scene);
  if (std::find(source_stack.begin(), source_stack.end(), normalized) != source_stack.end()) {
    AERO_CORE_WARN("Scene instance cycle detected for \"{0}\"; skipping expansion", normalized);
    return;
  }
  source_stack.push_back(normalized);

  const std::string path = SceneInstance::resolve_source_path(normalized);
  json scene_json = read_scene_json(path);
  SourceSceneData data;
  if (!collect_source_entities(scene_json, source_root, data)) {
    AERO_CORE_WARN("Failed to load source scene \"{0}\" for instance", path);
    source_stack.pop_back();
    return;
  }

  std::string source_name;
  Helper::load_json(scene_json["Scene"], "Name", source_name, std::string(""));
  if (!source_name.empty())
    comp.source_name = source_name;

  std::unordered_map<AG_uint, AG_uint> remap;
  AG_uint next_index = host->get_index();
  for (const auto& entity_json : data.entities) {
    AG_uint old_id = entity_json.value("ID", (AG_uint)0);
    if (old_id == 0 || remap.count(old_id))
      continue;
    remap[old_id] = next_index++;
  }
  host->set_next_index(next_index);

  const AG_uint instance_index = instance_root.get_component<Tag_Component>().index;
  index_map[instance_index] = instance_root;

  std::vector<json> remapped_entities;
  for (auto entity_json : data.entities) {
    const AG_uint old_id = entity_json.value("ID", (AG_uint)0);
    auto remap_it = remap.find(old_id);
    if (remap_it == remap.end())
      continue;

    const bool is_root = std::find(data.roots.begin(), data.roots.end(), old_id) != data.roots.end();
    entity_json["ID"]           = remap_it->second;
    entity_json["InstanceOwner"] = owner;
    entity_json["Locked"]        = true;

    if (entity_json.contains("Parent")) {
      if (entity_json["Parent"].is_null()) {
        if (!is_root)
          entity_json.erase("Parent");
      } else {
        AG_uint old_parent = entity_json["Parent"].get<AG_uint>();
        auto parent_it = remap.find(old_parent);
        if (parent_it != remap.end())
          entity_json["Parent"] = parent_it->second;
        else
          entity_json.erase("Parent");
      }
    }
    if (is_root)
      entity_json["Parent"] = instance_index;

    if (entity_json.contains("Children") && entity_json["Children"].is_array()) {
      json children = json::array();
      for (const auto& child : entity_json["Children"]) {
        auto child_it = remap.find(child.get<AG_uint>());
        if (child_it != remap.end())
          children.push_back(child_it->second);
      }
      entity_json["Children"] = children;
    }

    remapped_entities.push_back(std::move(entity_json));
  }

  std::unordered_map<AG_uint, Entity> local_entities;
  for (const auto& entity_json : remapped_entities) {
    int node_type = static_cast<int>(NodeType::None);
    std::string name;
    Helper::load_json(entity_json, "NodeType", node_type, static_cast<int>(NodeType::None));
    Helper::load_json(entity_json, "Name", name, std::string(""));

    Entity entity = host->create_entity(name, static_cast<NodeType>(node_type), true);
    Tag_Component::load_json(entity, entity_json);
    auto& tag = entity.get_component<Tag_Component>();
    tag.instance_owner = owner;
    tag.locked         = true;
    local_entities[tag.index] = entity;
    index_map[tag.index]      = entity;
    created.push_back(entity);
  }

  for (auto& [id, entity] : local_entities) {
    (void)id;
    Tag_Component::load_children(entity);
  }

  auto& root_tag = instance_root.get_component<Tag_Component>();
  for (AG_uint old_root : data.roots) {
    auto remap_it = remap.find(old_root);
    if (remap_it == remap.end())
      continue;
    auto entity_it = local_entities.find(remap_it->second);
    if (entity_it == local_entities.end())
      continue;
    Entity source_root_entity = entity_it->second;
    source_root_entity.get_component<Tag_Component>().parent = instance_root;
    if (std::find(root_tag.children.begin(), root_tag.children.end(), source_root_entity) ==
        root_tag.children.end())
      root_tag.children.push_back(source_root_entity);
  }

  for (const auto& entity_json : remapped_entities) {
    AG_uint id = entity_json.value("ID", (AG_uint)0);
    auto it = local_entities.find(id);
    if (it == local_entities.end())
      continue;
    load_entity_components(it->second, entity_json);
  }

  // Nested instances are expanded with the same owner so the entire generated
  // subtree is saved/refreshed/cleared as one unit.
  for (auto& [id, entity] : local_entities) {
    (void)id;
    if (!entity.has_component<SceneInstance_Component>())
      continue;
    auto& nested = entity.get_component<SceneInstance_Component>();
    expand_instance(host,
                    entity,
                    owner,
                    nested.source_scene,
                    nested.source_root,
                    source_stack,
                    created);
  }

  source_stack.pop_back();
}

}  // namespace

}  // namespace ag
