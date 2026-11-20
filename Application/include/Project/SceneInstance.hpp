#pragma once

#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
#include <string>
#include <vector>

namespace ag {

// Runtime/editor support for Godot-style scene instances.
//
// A scene instance is a normal SceneInstance node that stores a project-
// relative source scene path. The source scene's entity tree is expanded as
// generated children of that node. Generated children carry
// Tag_Component::instance_owner and are never written back to the host scene,
// so saving the source scene is the only way to change them.
class SceneInstance {
public:
  // Creates a SceneInstance node, points it at `source_scene`, expands it and
  // attaches it under `parent` (or as a root when `parent` is invalid).
  static Entity create_instance(AG_ref<Scene>    host,
                                const std::string& source_scene,
                                Entity            parent = Entity());

  // Rebuilds the generated contents of `instance_root` from its source scene.
  static void refresh_instance(AG_ref<Scene> host, Entity instance_root, bool load_scripts = true);

  // Refreshes every top-level instance in `host`.
  static void refresh_all(AG_ref<Scene> host, bool load_scripts = true);

  // Refreshes instances whose source matches `source_scene` (used by the editor
  // when the original scene is saved).
  static void refresh_source(AG_ref<Scene>    host,
                             const std::string& source_scene,
                             bool              load_scripts = true);

  // Destroys all entities generated for `instance_root`.
  static void clear_instance_contents(AG_ref<Scene> host, Entity instance_root);

  static bool   is_instance_root(Entity entity);
  static Entity get_instance_root(Entity entity);
  static bool   is_generated(Entity entity);
  static bool   is_read_only(Entity entity);

  // Source paths are stored project-relative, e.g. "/player.aeroscene".
  static std::string normalize_source_path(const std::string& path);
  static std::string resolve_source_path(const std::string& project_relative);
};

}  // namespace ag
