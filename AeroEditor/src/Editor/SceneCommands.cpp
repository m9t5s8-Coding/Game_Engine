#include <Editor/SceneCommands.hpp>

#include <Application/EditorLayer.hpp>
#include <GameObjects/Components/Components.hpp>
#include <Panels/ScenePanel.hpp>

namespace ag {

void SceneSnapshotCommand::restore_selection(const nlohmann::json& snapshot) {
  auto panel = EditorLayer::get().get_panel();
  if (!panel || !m_scene || !snapshot.is_object() || !snapshot.contains("Scene"))
    return;

  const auto& s = snapshot["Scene"];
  auto restore = [this, &s, &panel](const char* key, bool properties) {
    if (!s.contains(key)) return;
    AG_uint index = s[key].get<AG_uint>();
    if (index == 0) return;
    Entity e = m_scene->find_entity_by_index(index);
    if (!e) return;
    if (properties) panel->set_properties_entity(e);
    else panel->set_selected_entity(e);
  };

  restore("SelectedIndex", false);
  restore("PropertiesIndex", true);
}

void push_scene_action(AG_ref<Scene> scene, const std::string& name, nlohmann::json before) {
  if (!scene) return;
  nlohmann::json after = EditorSerializer::snapshot_scene(scene);
  if (before == after) return;
  UndoRedoManager::get().push(std::make_unique<SceneSnapshotCommand>(scene, name, std::move(before), std::move(after)));
}

} // namespace ag
