#include <icons.h>

#include <algorithm>
#include <Application/EditorLayer.hpp>
#include <UI/StyleScope.hpp>
#include <UI/UI.hpp>

namespace ag
{
EditorLayer* EditorLayer::s_instance = nullptr;

EditorLayer::EditorLayer()
  : Layer("EditorLayer")
{
  s_instance = this;
}

void EditorLayer::on_attach()
{
  ag::vec2u window_size = Application::get().get_window().get_size();
  ag::vec2f view_center;
  m_view_controller = ag::AG_cref<ViewController>(window_size, view_center);
  ViewController::set_main_controller(m_view_controller);

  FrameBufferSpecification spec;
  spec.attachments = {FrameBuffer_Texture_Format::RGBA8,
                      FrameBuffer_Texture_Format::RED_INTEGER,
                      FrameBuffer_Texture_Format::Depth};

  spec.size = window_size;

  m_framebuffer = FrameBuffer::create(spec);

  AERO_CORE_INFO("Scene name:{0}", Scene::get_active_scene()->get_name());
  m_scene = Scene::get_active_scene();

  if (!m_scene)
  {
    AERO_CORE_INFO("Invalid Scene");
  }
  m_panel = AG_cref<ScenePanel>(m_scene);

  // todo
  m_scenes[m_scene->get_name()] = m_scene;
}

void EditorLayer::on_detach()
{
}

void EditorLayer::on_update(TimeStamp ts)
{
  {
    update_mouse_position();
    Application::set_mouse_position(m_current_mouse_pos);
    if (Application::get().m_is_closing)
    {
      bool save_required = false;
      for (const auto& [key, scene] : m_scenes)
      {
        if (scene->is_save_required())
        {
          save_required = true;
          break;
        }
      }
      if (save_required)
      {
        UI::get_uistate_panels().save_changes_panel = true;
      }
      else
      {
        Application::get().m_running = false;
      }
    }
  }

  m_framebuffer->bind();
  RenderCommand::set_clear_color(ag::Color(56, 56, 56));
  RenderCommand::clear();

  m_framebuffer->clear_attachment(1, -1);

  m_view_controller->on_update(ts);
  m_panel->on_update();

  Renderer2D::begin_scene(m_view_controller->get_view(), m_viewport_size);

  editor_things();
  m_scene->on_update(ts);
  Renderer2D::start_batch();
  entity_selection();

  // m_panel->draw_selected_text();
  m_panel->draw_collision_shapes();
  m_panel->draw_tilemap_ghosts();
  // m_panel->draw_selection_box();

  Renderer2D::end_scene();

  m_framebuffer->unbind();

  m_last_mouse_pos = m_current_mouse_pos;
}

void EditorLayer::on_imgui_render()
{
  if (begin_dockspace_window())
  {
    if (begin_viewport_window())
    {
      render_toolbar();
      render_settings();
      render_viewport_content();
    }
    end_viewport_window();

    m_panel->on_imgui_render();
  }
  end_dockspace_window();
}

bool EditorLayer::begin_dockspace_window()
{
  static bool               dockspaceOpen   = true;
  static bool               opt_fullscreen  = true;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

  if (opt_fullscreen)
  {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  }

  ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);

  if (opt_fullscreen)
    ImGui::PopStyleVar(2);

  ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

  return true;
}

void EditorLayer::end_dockspace_window()
{
  ImGui::End();
}

bool EditorLayer::begin_viewport_window()
{
  ImGuiWindowFlags viewport_flags =
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoScrollWithMouse;

  StyleScope style;
  style.push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  style.push_style_var(ImGuiStyleVar_WindowRounding, 0.0f);

  return ImGui::Begin("ViewPort", nullptr, viewport_flags);
}

void EditorLayer::end_viewport_window()
{
  ImGui::End();
}

void EditorLayer::render_toolbar()
{
  StyleScope outer_style;
  outer_style.push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
  if (ImGui::BeginChild("SceneBar", ImVec2(0, 40), false, flags))
  {
    render_scene_buttons();
    render_add_scene_button();
  }
  ImGui::EndChild();
}

void EditorLayer::render_settings()
{
  StyleScope outer_style;
  outer_style.push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::Separator();
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
  if (ImGui::BeginChild("Toolbar", ImVec2(0, 40), false, flags))
  {
    auto     entity = m_panel->get_selected_entity();
    NodeType type   = NodeType::None;
    if (entity && entity.get_id() != INVALID_ENTITY && entity.has_component<Tag_Component>())
    {
      type = entity.get_component<Tag_Component>().node_type;
    }
    if (type != NodeType::TileMap)
    {
      render_settings_button();
    }
    else
    {
      render_paint_settings_button();
    }
  }
  ImGui::EndChild();
}

void EditorLayer::render_scene_buttons()
{
  StyleScope style;
  style.push_style_var(ImGuiStyleVar_FrameRounding, 2.0f);
  style.push_style_var(ImGuiStyleVar_FramePadding, ImVec2(8, 5));

  {
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);

    bool is_first = true;

    for (auto& [name, scene] : m_scenes)
    {
      if (!is_first)
        ImGui::SameLine(0, 10);
      is_first = false;

      ImGui::PushID(name.c_str());
      ImGui::BeginGroup();

      // Scene button
      {
        std::string scene_name = name;
        if (scene->is_save_required())
        {
          scene_name = "  " + scene_name + "  *";
        }
        else
        {
          scene_name = "  " + scene_name + "   ";
        }
        bool button_clicked = false;
        if (m_scene == scene)
        {
          ImGui::BeginDisabled();
          button_clicked = ImGui::Button(scene_name.c_str(), ImVec2(0, 30));
          ImGui::EndDisabled();
        }
        else
          button_clicked = ImGui::Button(scene_name.c_str(), ImVec2(0, 30));

        if (button_clicked)
          set_active_scene(scene);
      }

      ImGui::SameLine(0, 1);
      if (ImGui::Button(ICON_FA_X, ImVec2(30, 30)))
      {
        m_scene_to_remove = name;
        set_active_scene(scene);
      }

      ImGui::EndGroup();
      ImGui::PopID();
    }

    if (!m_scene_to_remove.empty())
    {
      UI::get_uistate_panels().save_changes_panel = true;
    }
  }
}

void EditorLayer::render_add_scene_button()
{
  ImGui::SameLine(0, 10);

  {
    StyleScope style;
    style.push_style_var(ImGuiStyleVar_FramePadding, ImVec2(5, 0));

    if (ImGui::Button(ICON_FA_PLUS, ImVec2(30, 30)))
    {
      UI::get_uistate_panels().create_new_scene = true;
    }
  }
}

void EditorLayer::render_paint_settings_button()
{
  float toolbar_height   = ImGui::GetWindowHeight();
  float button_height    = 30.0f;
  float vertical_padding = (toolbar_height - button_height) * 0.5f;
  ImGui::SetCursorPosY(vertical_padding);

  float available_width = ImGui::GetContentRegionAvail().x;

  float group_spacing = 40.0f;

  float paint_group_width = calculate_paint_group_width();
  // float axis_group_width = calculate_axis_group_width();

  float total_width = paint_group_width;
  float start_x     = (available_width - total_width) * 0.5f;

  ImGui::SetCursorPosX(start_x);
  render_paint_settings_group();

  /*ImGui::SameLine(0, group_spacing);
  render_axis_constraints_group();*/
}

void EditorLayer::render_settings_button()
{
  float toolbar_height   = ImGui::GetWindowHeight();
  float button_height    = 30.0f;
  float vertical_padding = (toolbar_height - button_height) * 0.5f;
  ImGui::SetCursorPosY(vertical_padding);

  float available_width = ImGui::GetContentRegionAvail().x;

  float group_spacing = 40.0f;

  float transform_group_width = calculate_transform_group_width();
  float axis_group_width      = calculate_axis_group_width();

  float total_width = transform_group_width + group_spacing + axis_group_width;
  float start_x     = (available_width - total_width) * 0.5f;

  ImGui::SetCursorPosX(start_x);
  render_transform_tools_group();

  ImGui::SameLine(0, group_spacing);
  render_axis_constraints_group();
}

void EditorLayer::render_transform_tools_group()
{
  StyleScope style;
  style.push_style_var(ImGuiStyleVar_FrameRounding, 2.0f);
  style.push_style_var(ImGuiStyleVar_FramePadding, ImVec2(8, 5));

  bool has_selection = m_panel && m_panel->selected_has_transform();

  // Move button
  render_transform_button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT,
                          "Grab ( G )",
                          TransformSetting::Move,
                          !has_selection);
  ImGui::SameLine(0, 8);

  // Rotate button
  render_transform_button(ICON_FA_ROTATE, "Rotate ( R )", TransformSetting::Rotate, !has_selection);
  ImGui::SameLine(0, 8);

  // Scale button
  render_transform_button(ICON_FA_EXPAND, "Scale ( S )", TransformSetting::Scale, !has_selection);
}

void EditorLayer::render_axis_constraints_group()
{
  StyleScope style;
  style.push_style_var(ImGuiStyleVar_FrameRounding, 2.0f);
  style.push_style_var(ImGuiStyleVar_FramePadding, ImVec2(8, 5));

  auto current_transform_mode = m_panel->get_transform_setting();
  bool can_constrain          = current_transform_mode != TransformSetting::None && m_panel &&
                       m_panel->selected_has_transform();

  render_axis_button(ICON_FA_X, "Along X - axis ( X )", TransformAxis::X, !can_constrain);
  ImGui::SameLine(0, 5);

  render_axis_button(ICON_FA_Y, "Along Y - axis ( Y )", TransformAxis::Y, !can_constrain);
  ImGui::SameLine(0, 5);
}

void EditorLayer::render_paint_settings_group()
{
  StyleScope style;
  style.push_style_var(ImGuiStyleVar_FrameRounding, 2.0f);
  style.push_style_var(ImGuiStyleVar_FramePadding, ImVec2(8, 5));

  // Move button
  render_paint_button(ICON_FA_PAINTBRUSH, "Paint", TileMap_Paint_Settings::Paint);
  ImGui::SameLine(0, 8);

  // Rotate button
  render_paint_button(ICON_FA_MINUS, "Line", TileMap_Paint_Settings::Line);
  ImGui::SameLine(0, 8);

  render_paint_button(ICON_FA_BORDER_ALL, "Rectangle", TileMap_Paint_Settings::Rectangle);
  ImGui::SameLine(0, 8);

  render_paint_button(ICON_FA_FILL_DRIP, "Fill", TileMap_Paint_Settings::Fill);
}

void EditorLayer::render_transform_button(const char*      label,
                                          const char*      shortcut,
                                          TransformSetting mode,
                                          bool             disabled)
{
  auto current_transform_mode = m_panel->get_transform_setting();

  GUI_Button button;
  button.label   = label;
  button.size    = {60.0, 30.0};
  button.active  = (current_transform_mode == mode);
  button.enabled = !disabled;

  if (UI::draw_button(button))
  {
    if (!disabled)
    {
      set_transform_mode(mode);
    }
  }

  if (ImGui::IsItemHovered() && !disabled)
  {
    ImGui::SetTooltip("%s", shortcut);
  }
}

void EditorLayer::render_axis_button(const char*   label,
                                     const char*   shortcut,
                                     TransformAxis axis,
                                     bool          disabled)
{
  auto current_axis = m_panel->get_transform_axis();

  GUI_Button button;
  button.label   = label;
  button.size    = {60.0, 30.0};
  button.active  = (current_axis == axis);
  button.enabled = !disabled;

  if (UI::draw_button(button))
  {
    if (!disabled)
    {
      set_axis_mode(axis);
    }
  }

  if (ImGui::IsItemHovered() && !disabled)
  {
    ImGui::SetTooltip("%s", shortcut);
  }
}

void EditorLayer::render_paint_button(const char*            label,
                                      const char*            shortcut,
                                      TileMap_Paint_Settings mode)
{
  auto current_mode = m_panel->get_paint_settings();
  bool is_active    = current_mode == mode;

  GUI_Button button;
  button.label  = label;
  button.size   = {50, 30};
  button.active = current_mode == mode;

  if (UI::draw_button(button))
  {
    set_paint_mode(mode);
  }

  if (ImGui::IsItemHovered())
  {
    ImGui::SetTooltip("%s", shortcut);
  }
}

float EditorLayer::calculate_transform_group_width() const
{
  const float button_width = 60.0f;
  const float spacing      = 10.0f;
  const int   button_count = 3;

  return (button_count * button_width) + ((button_count - 1) * spacing);
}

float EditorLayer::calculate_axis_group_width() const
{
  const float button_width = 50.0f;
  const float spacing      = 8.0f;
  const int   button_count = 2;

  return (button_count * button_width) + ((button_count - 1) * spacing);
}

float EditorLayer::calculate_paint_group_width() const
{
  const float button_width = 60.0f;
  const float spacing      = 10.0f;
  const int   button_count = 4;

  return (button_count * button_width) + ((button_count - 1) * spacing);
}

const char* EditorLayer::axis_to_string(TransformAxis axis) const
{
  switch (axis)
  {
    case TransformAxis::X:
      return "X";
    case TransformAxis::Y:
      return "Y";
    default:
      return "";
  }
}

void EditorLayer::set_transform_mode(TransformSetting mode)
{
  // Update scene panel
  if (m_panel)
  {
    switch (mode)
    {
      case TransformSetting::Move:
        m_panel->set_transform_setting(TransformSetting::Move);
        break;
      case TransformSetting::Rotate:
        m_panel->set_transform_setting(TransformSetting::Rotate);
        break;
      case TransformSetting::Scale:
        m_panel->set_transform_setting(TransformSetting::Scale);
        break;
      case TransformSetting::None:
        m_panel->set_transform_setting(TransformSetting::None);
        break;
    }
  }
}

void EditorLayer::set_axis_mode(TransformAxis axis)
{
  if (m_panel)
  {
    switch (axis)
    {
      case TransformAxis::X:
        AERO_CORE_INFO("Switched to X axis");
        m_panel->set_transform_axis(TransformAxis::X);
        break;
      case TransformAxis::Y:
        m_panel->set_transform_axis(TransformAxis::Y);
        break;
      case TransformAxis::None:
        m_panel->set_transform_axis(TransformAxis::None);
        break;
    }
  }
}

void EditorLayer::set_paint_mode(TileMap_Paint_Settings mode)
{
  if (m_panel)
  {
    m_panel->set_paint_settings(mode);
  }
}

void EditorLayer::handle_scene_deletion()
{
  auto scene = Scene::get_active_scene();
  handle_scene_deletion(scene->get_name());
}

void EditorLayer::handle_scene_deletion(const std::string& scene_name)
{
  if (m_scenes.size() == 1)
    return;

  auto it = m_scenes.find(scene_name);
  if (it == m_scenes.end())
  {
    m_scene_to_remove = "";
    return;
  }

  if (it->second == m_scene)
  {
    for (auto& [name, scene] : m_scenes)
    {
      if (name != scene_name)
      {
        set_active_scene(scene);
        break;
      }
    }
  }

  AERO_CORE_INFO("Scene Deleted: {0}", it->first);
  m_scenes.erase(it);
  m_scene_to_remove = "";
}

void EditorLayer::set_active_scene(AG_ref<Scene> scene)
{
  m_scene = scene;
  if (m_scene)
  {
    m_scene->set_active_scene(scene);
    AERO_CORE_INFO("Active Scene: {0}", scene->get_name());
  }
  m_panel->set_scene(scene);
}

void EditorLayer::render_viewport_content()
{
  handle_viewport_interaction();

  // Render framebuffer
  if (m_framebuffer)
  {
    uint32_t texture_ID    = m_framebuffer->get_colorattachment_id();
    ImVec2   viewport_size = ImGui::GetContentRegionAvail();

    ImGui::Image((void*)(intptr_t)texture_ID, viewport_size, ImVec2(0, 1), ImVec2(1, 0));
    m_viewport_hovered = ImGui::IsItemHovered();
    m_image_pos        = ImGui::GetItemRectMin();

    if (m_viewport_hovered)
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    else
      ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

    if (ImGui::BeginDragDropTarget())
    {
      if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
      {
        const char* dropped_path = (const char*)payload->Data;

        std::filesystem::path path(dropped_path);
        if (path.extension() == ".aeroscene")
        {
          std::string path_string = path.string();
          open_scene(path_string);
        }
      }
      ImGui::EndDragDropTarget();
    }
  }
}

void EditorLayer::handle_viewport_interaction()
{
  bool view_hovered = ImGui::IsWindowHovered();
  if (view_hovered)
    ImGui::SetWindowFocus();

  Application::get().get_imgui_layer()->block_events(!view_hovered);

  // Handle viewport resize
  ImVec2 viewport_size = ImGui::GetContentRegionAvail();
  if (viewport_size.x > 0 && viewport_size.y > 0 &&
      (m_viewport_size.x != viewport_size.x || m_viewport_size.y != viewport_size.y))
  {
    m_viewport_size = viewport_size;

    if (m_framebuffer)
      m_framebuffer->resize(m_viewport_size);

    if (m_view_controller)
    {
      m_view_controller->on_resize(m_viewport_size);
      m_view_controller->set_viewport_size(m_viewport_size);
    }
  }
}

void EditorLayer::update_mouse_position()
{
  vec2f mouse_pos     = Mouse::get_mouse_screen_position();
  m_current_mouse_pos = mouse_pos - m_image_pos;
  m_mouse_position    = m_current_mouse_pos;
  if (m_view_controller)
  {
    m_view_controller->set_viewport_mouse(m_current_mouse_pos);

    // Convert screen to world coordinates
    m_current_mouse_pos = Math::screen_to_world(m_current_mouse_pos,
                                                m_view_controller->get_view().get_float_rect(),
                                                m_viewport_size);
  }

  if (m_panel)
  {
    m_panel->set_current_mouse_position(m_current_mouse_pos);
  }
}

void EditorLayer::entity_selection()
{
  if (m_entity_selected)
  {
    if (m_mouse_position.x >= 0 && m_mouse_position.y >= 0)
    {
      int pixel_data = m_framebuffer->read_pixel(1, m_mouse_position);
      if (pixel_data >= 0)
      {
        Entity e((entt::entity)(pixel_data));
        if (m_panel->has_selected_entity())
        {
          if ((m_panel->get_selected_entity().get_id() != e.get_id() ||
               m_panel->get_selected_entity().has_component<Text_Editor_State>()) &&
              m_panel->get_transform_setting() == TransformSetting::None)
          {
            m_panel->set_selected_entity(e);
            m_panel->set_properties_entity(e);
          }
        }
        else
        {
          m_panel->set_selected_entity(e);
          m_panel->set_properties_entity(e);
        }
      }
      else
      {
        if (m_panel->get_transform_setting() == TransformSetting::None)
        {
          m_panel->set_selected_entity(Entity());
        }
      }
    }
    m_entity_selected = false;
  }
}

void EditorLayer::on_event(ag::Event& e)
{
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<KeyPressedEvent>(AERO_BIND_EVENT_FN(EditorLayer::on_key_pressed));
  dispatcher.Dispatch<TextInputEvent>(AERO_BIND_EVENT_FN(EditorLayer::on_text_input));
  m_view_controller->on_event(e);
  m_panel->on_event(e);
  dispatcher.Dispatch<MouseButtonPressedEvent>(AERO_BIND_EVENT_FN(EditorLayer::on_mouse_clicked));
}

bool EditorLayer::on_key_pressed(KeyPressedEvent& e)
{
  bool control =
      Keyboard::is_key_pressed(Key::LeftControl) || Keyboard::is_key_pressed(Key::RightControl);
  bool shift =
      Keyboard::is_key_pressed(Key::LeftShift) || Keyboard::is_key_pressed(Key::RightShift);
  if (control)
  {
    if (e.get_key_code() == Key::S)
    {
      save_scene();
    }

    if (e.get_key_code() == Key::O)
    {
      open_scene();
    }
    return false;
  }
  return false;
}

bool EditorLayer::on_mouse_clicked(MouseButtonPressedEvent& e)
{
  if (e.get_mouse_button() == Button::ButtonLeft)
  {
    if (m_panel->has_selected_entity())
    {
      if (NodeHelper::get_nodetype(m_panel->get_selected_entity()) != NodeType::TileMap)
      {
        m_entity_selected = true;
      }
    }
    else
    {
      m_entity_selected = true;
    }
    return false;
  }
  return false;
}

bool EditorLayer::on_text_input(TextInputEvent& e)
{
  return false;
}

void EditorLayer::create_new_scene(const std::string& path)
{
  std::string full_path = path;
  auto        project   = Project::get_active_project();
  Helper::normalize_path(full_path);

  std::string project_dir = project->get_directory();
  std::string scene_dir   = project->get_scene_directory();

  std::string base_path = project_dir + scene_dir + "/";

  std::string relative_path = full_path;
  if (relative_path.find(base_path) == 0)
    relative_path = relative_path.substr(base_path.size());

  Helper::normalize_path(relative_path);

  std::filesystem::path p(full_path);
  std::string           scene_name = p.stem().string();
  std::string           scene_path = "/" + relative_path;

  m_scene = Scene::create(scene_name, scene_path);
  SaveScene::save_scene(m_scene, full_path);
  Scene::set_active_scene(m_scene);

  m_scenes[scene_name] = m_scene;
  m_panel->set_scene(m_scene);
}

void EditorLayer::open_scene()
{
  auto full_path =
      FileDialogs::open_file("AeroScene Files (*.aeroscene)\0*.aeroscene\0All Files (*.*)\0*.*\0");
  if (full_path.empty())
    return;

  open_scene(full_path);
}

void EditorLayer::open_scene(std::string& path)
{
  Helper::normalize_path(path);
  auto scene = SaveScene::load_scene(path);

  {
    auto it = m_scenes.find(scene->get_name());
    if (it != m_scenes.end())
    {
      save_scene();
    }
  }
  m_scene                       = scene;
  m_scenes[m_scene->get_name()] = m_scene;
  m_panel->set_scene(m_scene);
}

void EditorLayer::save_scene()
{
  auto        project = Project::get_active_project();
  std::string scene_path =
      project->get_directory() + project->get_scene_directory() + m_scene->get_directory();
  SaveScene::save_scene(m_scene, scene_path);
}

void EditorLayer::run_runtime()
{
}

void EditorLayer::save_scene_as_default()
{
  auto        project   = Project::get_active_project();
  const auto& file_path = project->get_project_file_directory();
  Helper::makefile_read_only(file_path, false);
  try
  {
    std::ifstream file(file_path);

    if (!file.is_open())
    {
      AERO_CORE_WARN("Failed to Open File! {0}", file_path);
      throw std::runtime_error("Failed to Open Project File");
    }

    json j;
    file >> j;
    file.close();

    auto scene = Scene::get_active_scene();
    if (!scene)
    {
      throw std::runtime_error("No active scene");
    }

    Helper::save_json(j["Scene"], "Default", scene->get_name());
    Helper::save_json(j["Scene"], "Default Path", scene->get_directory());

    std::ofstream out_file(file_path);
    if (!out_file.is_open())
    {
      AERO_CORE_WARN("Failed to Open File! {0}", file_path);
      throw std::runtime_error("Failed to Open Project File");
    }
    out_file << j.dump(4);
    out_file.close();
  }
  catch (const std::exception& e)
  {
    AERO_CORE_ERROR("Failed to set default scene:{}", e.what());
  }
  Helper::makefile_read_only(file_path, true);
}

void EditorLayer::save_all_scene()
{
  auto project = Project::get_active_project();
  for (auto& [name, scene] : m_scenes)
  {
    if (!scene)
      continue;

    if (!scene->is_save_required())
      continue;

    std::string scene_path =
        project->get_directory() + project->get_scene_directory() + scene->get_directory();

    try
    {
      SaveScene::save_scene(scene, scene_path);
    }
    catch (const std::exception& e)
    {
      AERO_CORE_ERROR("Failed to save scene: {0}, {1}", scene->get_name(), e.what());
    }
  }
}

void EditorLayer::create_new_script(const std::string& path)
{
  auto        project   = Project::get_active_project();
  std::string full_path = path;
  Helper::normalize_path(full_path);

  std::string project_dir = project->get_directory();
  std::string script_dir  = project->get_scripts_directory();

  std::string base_path = project_dir + script_dir + "/";

  std::string relative_path = full_path;
  if (relative_path.find(base_path) == 0)
    relative_path = relative_path.substr(base_path.size());

  Helper::normalize_path(relative_path);

  std::filesystem::path p(full_path);
  std::string           script_path = "/" + relative_path;

  auto entity = m_panel->get_selected_entity();

  if (!entity.has_component<Script_Component>())
  {
    Script_Component comp;
    comp.path = script_path;
    entity.add_component<Script_Component>(comp);
  }
  else
  {
    auto& comp = entity.get_component<Script_Component>();
    comp.path  = script_path;
  }
}

void EditorLayer::try_exit()
{
  Application::get().m_is_closing = true;
}

void EditorLayer::print_scene_name(bool all_scene)
{
  std::string name;
  if (all_scene)
  {
    for (const auto& [key, scene] : m_scenes)
    {
      if (scene && scene->is_save_required())
      {
        name = key + ".aeroscene";
        ImGui::Text("%s", name.c_str());
      }
    }
  }
  else
  {
    auto scene = Scene::get_active_scene();
    name       = scene->get_name() + ".aeroscene";
    ImGui::Text("%s", name.c_str());
  }
}

void EditorLayer::editor_things()
{
  Rectangle           x_axis, y_axis;
  Transform_Component x_axis_transform, y_axis_transform;
  x_axis.fill_color = Color(255, 107, 107, 200);
  y_axis.fill_color = Color(78, 205, 196, 200);

  x_axis.size = vec2f(m_viewport_size.x, 1);
  y_axis.size = vec2f(1, m_viewport_size.y);

  x_axis.mode = RenderMode::Screen;
  y_axis.mode = RenderMode::Screen;

  const auto& view = m_view_controller->get_view();
  // x axis and y axis
  {
    x_axis_transform.position = vec2i(view.get_center().x, 0);
    y_axis_transform.position = vec2i(0, view.get_center().y);
    x_axis_transform.position =
        Math::world_to_screen(x_axis_transform.position, view.get_float_rect(), m_viewport_size);
    y_axis_transform.position =
        Math::world_to_screen(y_axis_transform.position, view.get_float_rect(), m_viewport_size);
    Renderer2D::draw_rectangle(y_axis, y_axis_transform);
    Renderer2D::draw_rectangle(x_axis, x_axis_transform);
  }

  // Transformation axix
  {
    Entity e = m_panel->get_selected_entity();
    if (e)
    {
      TransformSetting t_setting = m_panel->get_transform_setting();

      switch (t_setting)
      {
        case ag::TransformSetting::None:
          break;
        case ag::TransformSetting::Scale:
        {
          draw_transform_settings(e);
          break;
        }
        case ag::TransformSetting::Rotate:
        {
          break;
        }
        case ag::TransformSetting::Move:
        {
          draw_transform_settings(e);
          break;
        }
        default:
          break;
      }
    }
  }

  // Camera
  {
    auto        scene    = Scene::get_active_scene();
    const auto& entities = scene->m_registry.view<Camera_Component>();
    for (auto& entityID : entities)
    {
      Entity entity(entityID);
      auto&  type = entity.get_component<Tag_Component>().node_type;

      if (type != NodeType::Camera)
        continue;

      if (entity.has_component<Camera_Component>())
      {
        auto& props = entity.get_component<Camera_Component>();

        vec2f top_left = props.center - (props.size) / 2;

        vec2f bottom_right = props.center + (props.size) / 2;

        vec2f top_right = {bottom_right.x, top_left.y};

        vec2f bottom_left = {top_left.x, bottom_right.y};

        top_left     = Math::world_to_screen(top_left, view.get_float_rect(), m_viewport_size);
        top_right    = Math::world_to_screen(top_right, view.get_float_rect(), m_viewport_size);
        bottom_left  = Math::world_to_screen(bottom_left, view.get_float_rect(), m_viewport_size);
        bottom_right = Math::world_to_screen(bottom_right, view.get_float_rect(), m_viewport_size);

        vec2f size;
        size.x = top_right.x - top_left.x;
        size.y = bottom_right.y - top_right.y;

        Rectangle width, height;

        width.size  = {size.x, 1.0f};
        height.size = {1.0f, size.y};

        width.fill_color  = Color(120, 120, 120, 255);
        height.fill_color = Color(120, 120, 120, 255);

        width.mode  = RenderMode::Screen;
        height.mode = RenderMode::Screen;

        Transform_Component transform;

        // Top
        transform.position = Math::mid_point(top_left, top_right);
        Renderer2D::draw_rectangle(width, transform);

        // Left
        transform.position = Math::mid_point(top_left, bottom_left);
        Renderer2D::draw_rectangle(height, transform);

        // Bottom
        transform.position = Math::mid_point(bottom_left, bottom_right);
        Renderer2D::draw_rectangle(width, transform);

        // Right
        transform.position = Math::mid_point(bottom_right, top_right);
        Renderer2D::draw_rectangle(height, transform);
      }
    }
  }
}

void EditorLayer::draw_transform_settings(Entity e)
{
  const auto&   view   = m_view_controller->get_view();
  TransformAxis t_axis = m_panel->get_transform_axis();

  Rectangle           x_axis, y_axis;
  Transform_Component x_axis_transform, y_axis_transform;

  // Axis visual properties
  x_axis.fill_color = Color(255, 107, 107, 180);
  y_axis.fill_color = Color(78, 205, 196, 180);

  x_axis.size = vec2f(m_viewport_size.x, 1);
  y_axis.size = vec2f(1, m_viewport_size.y);

  x_axis.mode = RenderMode::Screen;
  y_axis.mode = RenderMode::Screen;

  if (!e.has_component<Transform_Component>())
    return;

  const auto& transform = Transform_Component::get_world_transform(e);

  vec2i x_pos = vec2i(view.get_center().x, transform.position.y);
  vec2i y_pos = vec2i(transform.position.x, view.get_center().y);

  x_axis_transform.position = Math::world_to_screen(x_pos, view.get_float_rect(), m_viewport_size);
  y_axis_transform.position = Math::world_to_screen(y_pos, view.get_float_rect(), m_viewport_size);

  if (t_axis == TransformAxis::None || t_axis == TransformAxis::X)
    Renderer2D::draw_rectangle(x_axis, x_axis_transform);

  if (t_axis == TransformAxis::None || t_axis == TransformAxis::Y)
    Renderer2D::draw_rectangle(y_axis, y_axis_transform);
}

void EditorLayer::load_texture(Entity entity)
{
  std::string selected_path =
      FileDialogs::open_file("Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.tga\0All Files\0*.*\0");
  auto& props = entity.get_component<Texture_Component>();

  if (!selected_path.empty())
  {
    try
    {
      props.path    = selected_path;
      props.texture = NodeHelper::load_texture(props.path, true, props.filter_mode);
      if (entity.has_component<Render2D_Component>())
      {
        auto& render = entity.get_component<Render2D_Component>();
        render.size  = props.texture->get_size();
      }
    }
    catch (const std::exception& e)
    {
      props.texture.reset();
      props.path.clear();
    }
  }
}

void EditorLayer::open_export_panel()
{
  m_export_panel.open();
}

void EditorLayer::render_export_panel()
{
  m_export_panel.render();
}
}  // namespace ag
