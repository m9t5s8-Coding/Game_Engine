#pragma once

#include <Editor/EditorSerializer.hpp>
#include <Editor/UndoRedo.hpp>
#include <Scene/Scene.hpp>
#include <string>

#include <nlohmann/json.hpp>

namespace ag {

class SceneSnapshotCommand : public UndoCommand {
public:
  SceneSnapshotCommand(AG_ref<Scene> scene, std::string name, nlohmann::json before, nlohmann::json after)
      : m_scene(std::move(scene)), m_name(std::move(name)),
        m_before(std::move(before)), m_after(std::move(after)) {}

  void undo() override {
    if (m_scene) {
      EditorSerializer::restore_scene_snapshot(m_scene, m_before);
      restore_selection(m_before);
    }
  }
  void redo() override {
    if (m_scene) {
      EditorSerializer::restore_scene_snapshot(m_scene, m_after);
      restore_selection(m_after);
    }
  }
  std::string name() const override { return m_name; }

private:
  void restore_selection(const nlohmann::json& snapshot);

  AG_ref<Scene> m_scene;
  std::string m_name;
  nlohmann::json m_before;
  nlohmann::json m_after;
};

// Snapshot-based helper: captures `scene` before action, executes action, captures after, pushes command.
template <typename Func>
void execute_scene_action(AG_ref<Scene> scene, const std::string& name, Func&& action) {
  if (!scene) return;
  nlohmann::json before = EditorSerializer::snapshot_scene(scene);
  action();
  nlohmann::json after = EditorSerializer::snapshot_scene(scene);
  UndoRedoManager::get().push(std::make_unique<SceneSnapshotCommand>(scene, name, std::move(before), std::move(after)));
}

// Push an already-executed scene action command.
void push_scene_action(AG_ref<Scene> scene, const std::string& name, nlohmann::json before);

// Execute a scene action and push an undo entry only if the serialized scene changed.
template <typename Func>
void record_scene_action(AG_ref<Scene> scene, const std::string& name, Func&& action) {
  if (!scene) return;
  nlohmann::json before = EditorSerializer::snapshot_scene(scene);
  action();
  nlohmann::json after = EditorSerializer::snapshot_scene(scene);
  if (before != after) {
    UndoRedoManager::get().push(std::make_unique<SceneSnapshotCommand>(
        scene, name, std::move(before), std::move(after)));
  }
}

} // namespace ag
