#pragma once

#include <Aero.hpp>


namespace ag
{
  const float MOVE_THRESHOLD = 1.0f;

  enum class TransformSetting
  {
    None, Scale, Rotate, Move
  };

  enum class TransformAxis
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


    TransformSetting get_transform_setting() const { return m_current_transform_setting; }
    TransformAxis get_transform_axis() const { return m_current_transform_axis; }

    void is_mouse_inside_window(const bool inside_window) { m_mouse_inside_window = inside_window; }

    Entity get_selected_entity() const { return m_selected_entity; }
    void set_selected_entity(Entity entity);

    void draw_node_hierarchy(Entity entity, int level = 0);
    void draw_properties_panel();
    void draw_create_object();
    void draw_selected_text();


    void set_current_mouse_position(const vec2f& position) { m_current_mouse_position = position; }
    void update_transform_settings();
    void move_transform_setting();
    void rotate_transform_setting();
    void scale_transform_setting();
    void reset_transform_setting();


    void add_scripts();

    void update_tilemap();

    bool texture_selector(const AG_ref<Texture2D>& texture, const vec2u& tile_size, uint_rect& texture_rect);


    void draw_scene_top_panel();

    bool on_key_pressed(KeyPressedEvent& e);
    bool on_mouse_pressed(MouseButtonPressedEvent& e);

  private:
    AG_ref<Scene> m_scene;
    Entity m_selected_entity;
    bool m_show_create_panel = false;

    vec2f m_last_mouse_position;
    vec2f m_current_mouse_position;
    bool m_mouse_inside_window;
    vec2f m_delta;

    uint_rect m_texture_rect;
    bool m_is_texture_selected;

    bool m_move_flag = false;
    bool m_scale_flag = false;
    bool m_rotate_flag = false;

    Transform m_initial_transform;
    TransformSetting m_current_transform_setting = TransformSetting::None;
    TransformAxis m_current_transform_axis = TransformAxis::None;
  };

}
