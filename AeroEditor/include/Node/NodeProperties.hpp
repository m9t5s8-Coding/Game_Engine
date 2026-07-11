#pragma once

#include <icons.h>
#include <imgui.h>

#include <functional>
#include <Scene/Entity.hpp>

namespace ag {
struct Component_Info {
  const char* name;
  bool (*is_compatible)(NodeType);
  std::function<void(Entity)> create;
  std::function<void(Entity)> imgui_render;
};

class NodeProperties {
public:
  static void register_components();

  static void add_component(Entity entity);

  static void animated_sprite_2D(Entity entity);
  static void button_2D(Entity entity);
  static void camera_2D(Entity entity);
  static void character_body_2D(Entity entity);
  static void circle_2D(Entity entity);
  static void rectangle_2D(Entity entity);
  static void scene_2D(Entity entity);
  static void sprite_2D(Entity entity);
  static void text_2D(Entity entity);
  static void texture_button_2D(Entity entity);
  static void tilemap_2D(Entity entity);
  static void audio_2D(Entity entity);

  template <typename T>
  inline static bool draw_component_node(const std::string&      name,
                                         Entity                  entity,
                                         std::function<void(T&)> draw_content,
                                         bool                    can_remove = false) {
    if (!entity.has_component<T>())
      return false;

    ImGui::PushID(name.c_str());

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 6.0f));

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                               ImGuiTreeNodeFlags_SpanAvailWidth |
                               ImGuiTreeNodeFlags_AllowItemOverlap;

    bool open   = ImGui::TreeNodeEx(name.c_str(), flags);
    bool remove = false;

    if (ImGui::BeginPopupContextItem("ComponentContextMenu")) {
      if (can_remove && ImGui::MenuItem("Remove")) {
        remove = true;
      }

      if (ImGui::MenuItem("Reset to Default")) {
      }

      if (ImGui::MenuItem("Copy Component")) {
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Properties...")) {
      }

      ImGui::EndPopup();
    }

    if (open) {
      auto& component = entity.get_component<T>();
      draw_content(component);
      ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopStyleVar();

    if (remove) {
      T::remove_component(entity);
      Scene::save_required();
    }

    return open;
  }

private:
  inline static std::vector<Component_Info> comps;

  static void draw_added_components(Entity entity);

  template <typename T>
  static void add(Entity entity) {
    if (!entity.has_component<T>()) {
      T::add_component(entity);

      Scene::get_active_scene()->set_save_required();
    }
  }
};
}  // namespace ag
