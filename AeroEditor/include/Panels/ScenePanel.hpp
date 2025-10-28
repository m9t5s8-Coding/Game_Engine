#pragma once

#include <Aero.hpp>


namespace ag
{
  enum class TransformSetting
  {
    None, Scale, Rotate, Move
  };

  enum class TransformAxix
  {
    None, X, Y
  };

  class ScenePanel
  {
  public:
    ScenePanel(const AG_ref<Scene>& scene);
    ScenePanel() = default;
    void set_scene(const AG_ref<Scene>& scene);

    ~ScenePanel() = default;

    void on_imgui_render();
    void on_event(Event& e);
    void on_update();

    void is_mouse_inside_window(const bool inside_window) { m_mouse_inside_window = inside_window; }

    Entity get_selected_entity() const { return m_selected_entity; }
    void set_selected_entity(Entity entity);

    void draw_node_hierarchy(Entity entity);
    void draw_properties_panel();
    void draw_create_object();


    void set_current_mouse_position(const vec2f& position) { m_current_mouse_position = position; }
    void update_transform_settings();
    void move_transform_setting();
    void rotate_transform_setting();
    void scale_transform_setting();

    bool on_key_pressed(KeyPressedEvent& e);

  private:
    AG_ref<Scene> m_scene;
    Entity m_selected_entity;
    bool m_show_create_panel = false;

    vec2f m_last_mouse_position;
    vec2f m_current_mouse_position;
    bool m_mouse_inside_window;

    TransformSetting m_current_transform_setting = TransformSetting::None;
    TransformAxix m_current_transform_axix = TransformAxix::None;
  };

}
