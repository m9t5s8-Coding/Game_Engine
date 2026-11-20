#pragma once

#include <GameObjects/Components/Components.hpp>

namespace ag {
// Container node for a Godot-style instance of another .aeroscene. The node
// itself is editable (transform/name/script); the scene contents expanded
// beneath it are generated and read-only in the host scene.
struct SceneInstanceNode {
  static void create_node(Entity entity);
  static void delete_node(Entity entity);
  static void clone_node(Entity original, Entity clone);
  static json save_json(Entity entity);
  static void load_json(Entity entity, const json& j);
  static void update(Entity entity, TimeStamp ts);
  static void draw(Entity entity);
};
}  // namespace ag
