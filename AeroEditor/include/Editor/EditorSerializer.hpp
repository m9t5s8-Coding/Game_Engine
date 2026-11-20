#pragma once

#include <nlohmann/json.hpp>
#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
#include <string>
#include <vector>

namespace ag {

class EditorSerializer {
public:
  // Serialize a single entity and its full subtree into a self-contained JSON payload.
  // Payload contains { "root": <index>, "entities": [ ... ] } with original tag indices.
  static nlohmann::json serialize_entity_subtree(Entity root);

  // Deserialize entities previously serialized with serialize_entity_subtree into `scene`.
  // Returns the root entity of the instantiated subtree, or invalid Entity on failure.
  // If `parent` is valid, the root will be attached as a child of `parent`; otherwise added as root.
  static Entity instantiate_entity_subtree(AG_ref<Scene> scene, const nlohmann::json& payload, Entity parent = Entity());

  // Serialize full scene state for undo snapshots (entities + roots + next index + save_required context not included).
  static nlohmann::json snapshot_scene(AG_ref<Scene> scene);

  // Restore scene from snapshot. Clears current entities and rebuilds from snapshot.
  // Preserves scene name/directory. Returns true on success.
  static bool restore_scene_snapshot(AG_ref<Scene> scene, const nlohmann::json& snapshot);
};

} // namespace ag
