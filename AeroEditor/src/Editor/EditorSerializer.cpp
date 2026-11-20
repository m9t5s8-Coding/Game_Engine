#include <Editor/EditorSerializer.hpp>

#include <Application/EditorLayer.hpp>
#include <Panels/ScenePanel.hpp>
#include <algorithm>
#include <GameObjects/Components/Components.hpp>
#include <GameObjects/NodeFactory.hpp>
#include <Helper.hpp>
#include <Scene/SceneComponent.hpp>
#include <unordered_map>
#include <unordered_set>

namespace ag {

namespace {

nlohmann::json serialize_single_entity(Entity e) {
  if (!e || !e.has_component<Tag_Component>())
    return {};

  const auto& tag = e.get_component<Tag_Component>();
  nlohmann::json entityjson = Tag_Component::save_json(e);
  if (!entityjson.is_object()) entityjson = nlohmann::json::object();

  auto it = NodeFactory::save_map.find(tag.node_type);
  if (it != NodeFactory::save_map.end()) {
    nlohmann::json nodejson = it->second(e);
    if (nodejson.is_object()) entityjson.update(nodejson);
  }
  return entityjson;
}

void collect_subtree(Entity e, std::vector<Entity>& out) {
  if (!e || !e.has_component<Tag_Component>()) return;
  out.push_back(e);
  auto& tag = e.get_component<Tag_Component>();
  for (auto& child : tag.children) {
    collect_subtree(child, out);
  }
}

} // namespace

nlohmann::json EditorSerializer::serialize_entity_subtree(Entity root) {
  nlohmann::json payload;
  payload["entities"] = nlohmann::json::array();
  if (!root || !root.has_component<Tag_Component>()) return payload;

  std::vector<Entity> entities;
  collect_subtree(root, entities);

  AG_uint root_index = root.get_component<Tag_Component>().index;
  payload["root"] = root_index;

  for (auto& e : entities) {
    payload["entities"].push_back(serialize_single_entity(e));
  }
  return payload;
}

Entity EditorSerializer::instantiate_entity_subtree(AG_ref<Scene> scene, const nlohmann::json& payload, Entity parent) {
  if (!scene || !payload.is_object() || !payload.contains("entities") || !payload["entities"].is_array())
    return Entity();

  auto prev_active = Scene::get_active_scene();
  Scene::set_active_scene(scene);

  // Build old->new index mapping and rewrite entities JSON
  std::unordered_map<AG_uint, AG_uint> remap;
  AG_uint next_index = scene->get_index();

  // First pass: assign new indices
  for (auto& ej : payload["entities"]) {
    AG_uint old_id = 0;
    Helper::load_json(ej, "ID", old_id, (AG_uint)0);
    if (old_id == 0) continue;
    if (!remap.count(old_id)) {
      remap[old_id] = next_index++;
    }
  }
  scene->set_next_index(next_index);

  // Clone and remap JSON
  nlohmann::json remapped_entities = nlohmann::json::array();
  AG_uint payload_root_old = payload.value("root", (AG_uint)0);
  AG_uint payload_root_new = remap.count(payload_root_old) ? remap[payload_root_old] : (AG_uint)0;

  for (auto ej : payload["entities"]) {
    AG_uint old_id = 0;
    Helper::load_json(ej, "ID", old_id, (AG_uint)0);
    if (!remap.count(old_id)) continue;
    ej["ID"] = remap[old_id];

    if (ej.contains("Parent") && !ej["Parent"].is_null()) {
      AG_uint old_p = ej["Parent"].get<AG_uint>();
      if (remap.count(old_p)) ej["Parent"] = remap[old_p];
      else ej.erase("Parent"); // will attach to provided parent or root
    }
    if (ej.contains("Children") && ej["Children"].is_array()) {
      nlohmann::json new_children = nlohmann::json::array();
      for (auto& cid : ej["Children"]) {
        AG_uint old_c = cid.get<AG_uint>();
        if (remap.count(old_c)) new_children.push_back(remap[old_c]);
      }
      ej["Children"] = new_children;
    }
    remapped_entities.push_back(ej);
  }

  // We need to set the root entity's parent appropriately
  for (auto& ej : remapped_entities) {
    AG_uint id = ej.value("ID", (AG_uint)0);
    if (id == payload_root_new) {
      if (parent && parent.has_component<Tag_Component>()) {
        ej["Parent"] = parent.get_component<Tag_Component>().index;
      } else {
        ej.erase("Parent");
      }
      break;
    }
  }

  // Create entities and load tags
  std::unordered_map<AG_uint, Entity> local_id_map;
  // Preserve existing index_map? load_children uses global index_map; merge carefully.
  // Save existing entries for new indices unlikely, but keep safe.
  std::unordered_map<AG_uint, Entity> saved_index_entries;

  for (auto& entityjson : remapped_entities) {
    NodeType type = NodeType::None;
    std::string name;
    {
      int node = (int)NodeType::None;
      Helper::load_json(entityjson, "NodeType", node, (int)NodeType::None);
      Helper::load_json(entityjson, "Name", name, std::string(""));
      type = static_cast<NodeType>(node);
    }

    Entity e = scene->create_entity(name, type, true /* clone-like: don't run default create_map side effects? */);
    // But we want components to be loaded from json via load_map; create_entity(is_cloning=true) skips NodeFactory::create_map
    // Tag component was added by create_entity with fresh index/name; we need to override with loaded tag data.
    Tag_Component::load_json(e, entityjson);
    auto& tag = e.get_component<Tag_Component>();
    local_id_map[tag.index] = e;
    saved_index_entries[tag.index] = index_map[tag.index];
    index_map[tag.index] = e;
  }

  // Update scene next_index to be >= all assigned
  AG_uint max_idx = scene->get_index();
  for (auto& [id,_] : local_id_map) max_idx = std::max(max_idx, id + 1);
  scene->set_next_index(max_idx);

  // Load children relationships within this set and with existing parent if provided
  for (auto& [id, e] : local_id_map) {
    Tag_Component::load_children(e);
  }
  // Also, if parent is provided, ensure parent's children list includes root (load_children on root sets parent but may not add to parent's children if parent not in local set's load flow in same way?)
  Entity root_entity = local_id_map.count(payload_root_new) ? local_id_map[payload_root_new] : Entity();
  if (root_entity && parent && parent.has_component<Tag_Component>()) {
    auto& ptag = parent.get_component<Tag_Component>();
    auto& rtag = root_entity.get_component<Tag_Component>();
    rtag.parent = parent;
    if (std::find(ptag.children.begin(), ptag.children.end(), root_entity) == ptag.children.end()) {
      ptag.children.push_back(root_entity);
    }
  } else if (root_entity) {
    // root of scene
    auto& rtag = root_entity.get_component<Tag_Component>();
    rtag.parent = Entity{};
    scene->push_back_root(root_entity.get_id());
  }

  // Load entity data/components
  for (auto& entityjson : remapped_entities) {
    AG_uint id = 0;
    Helper::load_json(entityjson, "ID", id, (AG_uint)0);
    auto it_map = local_id_map.find(id);
    if (it_map == local_id_map.end()) continue;
    Entity e = it_map->second;
    int node = 0;
    Helper::load_json(entityjson, "NodeType", node, (int)NodeType::None);
    NodeType type = static_cast<NodeType>(node);
    auto it = NodeFactory::load_map.find(type);
    if (it != NodeFactory::load_map.end()) {
      it->second(e, entityjson);
    }
  }

  // Cleanup: remove our temporary index_map entries for new ids? load_scene clears whole map at end.
  // Better to restore previous state for these ids (likely none) and keep them? Keeping is fine.
  // But we should not leave stale global state; however index_map is used as global registry for load operations.
  for (auto& [id, saved] : saved_index_entries) {
    if (saved && saved.get_id() != INVALID_ENTITY) index_map[id] = saved;
    else index_map.erase(id);
  }

  if (prev_active) Scene::set_active_scene(prev_active);
  return root_entity;
}

nlohmann::json EditorSerializer::snapshot_scene(AG_ref<Scene> scene) {
  nlohmann::json j;
  if (!scene) return j;

  auto prev_active = Scene::get_active_scene();
  Scene::set_active_scene(scene);

  j["Scene"] = nlohmann::json::object();
  Helper::save_json(j["Scene"], "Name", scene->get_name(), std::string(""));
  Helper::save_json(j["Scene"], "Path", scene->get_directory(), std::string(""));
  j["Scene"]["NextIndex"] = scene->get_index();

  // Remember editor-facing selection by stable tag index so it can be restored
  // after a snapshot rebuild invalidates entity handles.
  auto panel = EditorLayer::get().get_panel();
  if (panel) {
    auto selected = panel->get_selected_entity();
    j["Scene"]["SelectedIndex"] =
        (selected && selected.has_component<Tag_Component>())
            ? selected.get_component<Tag_Component>().index
            : (AG_uint)0;
    auto properties = panel->get_properties_entity();
    j["Scene"]["PropertiesIndex"] =
        (properties && properties.has_component<Tag_Component>())
            ? properties.get_component<Tag_Component>().index
            : (AG_uint)0;
  }

  j["Scene"]["Entities"] = nlohmann::json::array();
  {
    nlohmann::json root_json = nlohmann::json::array();
    auto& roots = scene->get_root_entity();
    for (auto id : roots) {
      Entity e(id);
      if (e && e.has_component<Tag_Component>()) {
        root_json.push_back(e.get_component<Tag_Component>().index);
      }
    }
    j["Scene"]["Roots"] = root_json;
  }

  auto view = scene->m_registry.view<Tag_Component>();
  for (auto entityID : view) {
    if (!scene->m_registry.valid(entityID)) continue;
    Entity e(entityID);
    if (!e.has_component<Tag_Component>()) continue;
    j["Scene"]["Entities"].push_back(serialize_single_entity(e));
  }

  if (prev_active) Scene::set_active_scene(prev_active);
  return j;
}

bool EditorSerializer::restore_scene_snapshot(AG_ref<Scene> scene, const nlohmann::json& snapshot) {
  if (!scene || !snapshot.is_object() || !snapshot.contains("Scene")) return false;

  auto prev_active = Scene::get_active_scene();
  Scene::set_active_scene(scene);

  // Clear existing entities
  {
    // collect all entities first
    std::vector<entt::entity> all;
    auto view = scene->m_registry.view<Tag_Component>();
    for (auto e : view) all.push_back(e);
    // destroy via scene API carefully: Scene::destroy() destroys roots recursively, but easier to destroy all
    scene->m_registry.clear();
    scene->get_root_entity().clear();
    scene->m_to_delete_entity.clear();
  }

  const auto& s = snapshot["Scene"];
  // Keep name/dir as-is? snapshot contains them; better restore directory/name only if desired. But scene identity should remain.
  // scene->set_name(s.value("Name", scene->get_name()));
  // scene->set_directory(s.value("Path", scene->get_directory()));
  scene->set_next_index(s.value("NextIndex", (AG_uint)0));

  index_map.clear();
  std::unordered_map<AG_uint, Entity> id_map;

  if (s.contains("Entities") && s["Entities"].is_array()) {
    for (auto& entityjson : s["Entities"]) {
      NodeType type = NodeType::None;
      std::string name;
      int node = (int)NodeType::None;
      Helper::load_json(entityjson, "NodeType", node, (int)NodeType::None);
      Helper::load_json(entityjson, "Name", name, std::string(""));
      type = static_cast<NodeType>(node);

      Entity e = scene->create_entity(name, type, true);
      Tag_Component::load_json(e, entityjson);
      auto& tag = e.get_component<Tag_Component>();
      id_map[tag.index] = e;
      index_map[tag.index] = e;
    }

    // Load children relationships
    for (auto& [id, e] : id_map) {
      (void)id;
      Tag_Component::load_children(e);
    }

    // Load entity data (NodeFactory load functions require a mutable json reference)
    for (auto entityjson : s["Entities"]) {
      AG_uint id = 0;
      Helper::load_json(entityjson, "ID", id, (AG_uint)0);
      auto it_map = id_map.find(id);
      if (it_map == id_map.end()) continue;
      Entity e = it_map->second;
      int node = 0;
      Helper::load_json(entityjson, "NodeType", node, (int)NodeType::None);
      NodeType type = static_cast<NodeType>(node);
      auto it = NodeFactory::load_map.find(type);
      if (it != NodeFactory::load_map.end()) {
        it->second(e, entityjson);
      }
    }
  }

  bool root_available = false;
  if (s.contains("Roots") && s["Roots"].is_array()) {
    root_available = true;
    for (auto& id_json : s["Roots"]) {
      AG_uint root_id = id_json.get<AG_uint>();
      auto it = id_map.find(root_id);
      if (it != id_map.end()) {
        scene->push_back_root(it->second.get_id());
      }
    }
  }
  scene->set_root_available(root_available);

  index_map.clear();
  scene->set_save_required(true);

  if (prev_active) Scene::set_active_scene(prev_active);
  return true;
}

} // namespace ag
