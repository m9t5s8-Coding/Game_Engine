#include <Project/Assetmanager.hpp>
#include <Sandbox2D.hpp>

namespace ag
{
Sandbox2D::Sandbox2D()
  : Layer("Sandbox2D")
{
}

void Sandbox2D::on_attach()
{
  auto size         = Application::get().get_window().get_size();
  m_view_controller = ViewController::create(size, size / 2);
  ViewController::set_main_controller(m_view_controller);

  FrameBufferSpecification spec;
  spec.attachments = {FrameBuffer_Texture_Format::RGBA8, FrameBuffer_Texture_Format::RED_INTEGER};
  spec.size        = size;
  m_framebuffer    = FrameBuffer::create(spec);

  load_project_data();
}

void Sandbox2D::on_detach()
{
}

void Sandbox2D::on_update(ag::TimeStamp ts)
{
  ViewController::set_mouse_position();
  m_view_controller->on_update(ts);

  m_framebuffer->bind();
  RenderCommand::set_clear_color(ag::Color(38, 45, 42));
  RenderCommand::clear();
  m_framebuffer->clear_attachment(1, -1);
  Renderer2D::begin_scene(m_view_controller->get_view(),
                          Application::get().get_window().get_size());
  m_scene->on_update(ts);
  Renderer2D::end_scene();
  entity_selection();
  m_framebuffer->unbind();
  Renderer2D::begin_scene(m_view_controller->get_view(),
                          Application::get().get_window().get_size());
  Renderer2D::draw_fullscreen_quad(m_framebuffer->get_colorattachment_id());
  Renderer2D::end_scene();
}

void Sandbox2D::on_event(Event& event)
{
  m_scene->on_event(event);
  EventDispatcher dispatcher(event);
  dispatcher.Dispatch<MouseButtonPressedEvent>(AERO_BIND_EVENT_FN(Sandbox2D::on_mouse_pressed));
  dispatcher.Dispatch<WindowResizeEvent>(AERO_BIND_EVENT_FN(Sandbox2D::on_window_resize));
  dispatcher.Dispatch<WindowCloseEvent>(AERO_BIND_EVENT_FN(Sandbox2D::on_window_close));
}

std::string Sandbox2D::get_appdata_path()
{
#ifdef PLATFORM_WINDOWS
  const char* appdata = std::getenv("APPDATA");
  if (appdata)
    return std::string(appdata);
  else
    return ".";
#elif defined(PLATFORM_LINUX)
  const char* home = std::getenv("HOME");
  if (home)
    return std::string(home) + "/.config";
  else
    return ".";
#endif
  return ".";
}

void Sandbox2D::entity_selection()
{
  vec2i mouse_position = Mouse::get_mouse_position();
  if (mouse_position.x >= 0 && mouse_position.y >= 0)
  {
    int pixel_data = m_framebuffer->read_pixel(1, mouse_position);
    if (pixel_data >= 0)
    {
      Entity e((entt::entity)(pixel_data));
      m_hover_entity = e;
      if (m_hover_entity.has_component<ButtonState_Component>())
      {
        auto& props = m_hover_entity.get_component<ButtonState_Component>();
        props.button_state |= Button_State::Hovered;
      }
    }
    else
    {
      m_hover_entity = Entity();
    }
  }
}

bool Sandbox2D::on_mouse_pressed(MouseButtonPressedEvent& e)
{
  if (e.get_mouse_button() == Button::ButtonLeft)
  {
    if (!m_hover_entity)
      return false;

    if (m_hover_entity.has_component<ButtonState_Component>())
    {
      auto& props = m_hover_entity.get_component<ButtonState_Component>();
      props.button_state |= Button_State::Pressed;
    }
  }
  return false;
}

bool Sandbox2D::on_window_resize(WindowResizeEvent& e)
{
  m_framebuffer->resize(e.get_size());
  return false;
}

bool Sandbox2D::on_window_close(WindowCloseEvent& e)
{
  Application::get().m_running = false;
  return true;
}

void Sandbox2D::load_project_data()
{
  std::string exe_dir  = Application::get_exe_directory();
  std::string pak_path = exe_dir + "/game.pak";

  std::string fallback_base;
  fallback_base = get_appdata_path();
  Helper::normalize_path(fallback_base);
  fallback_base += "/AEROEngine";

  AssetManager::init_project(pak_path, fallback_base);

  json j;
  if (AssetManager::is_packed(AssetManager::Domain::Project))
  {
    j = AssetManager::read_json("settings.json", AssetManager::Domain::Project);
  }
  else
  {
    std::string setting_path = get_appdata_path() + "/AEROEngine/settings.json";
    Helper::normalize_path(setting_path);
    Helper::makefile_read_only(setting_path, false);
    std::ifstream file(setting_path);

    if (!file.is_open())
    {
      AERO_CORE_ERROR("Failed to Open File: {0}", setting_path);
      return;
    }
    file >> j;
    file.close();
    Helper::makefile_read_only(setting_path);
  }
  if (j.is_null())
  {
    AERO_CORE_ERROR("Failed to load settings.json");
    return;
  }

  std::string project_path;
  Helper::load_json(j["Project"], "File Path", project_path);
  if (!AssetManager::is_packed(AssetManager::Domain::Project))
  {
    AssetManager::set_fallback_path(project_path, AssetManager::Domain::Project);
  }

  if (project_path.empty())
  {
    AERO_CORE_ERROR("No project path in settings.json");
    return;
  }

  if (AssetManager::is_packed(AssetManager::Domain::Project))
  {
    Project::load_project(project_path);
  }
  else
  {
    Project::load_project(project_path);
  }

  auto project = Project::get_active_project();
  if (!project)
  {
    AERO_CORE_ERROR("Failed to load project");
    return;
  }

  std::string scene_path;

  if (AssetManager::is_packed(AssetManager::Domain::Project))
  {
    std::string proj_file_path = project->get_project_file_directory();
    json        proj_json = AssetManager::read_json(proj_file_path, AssetManager::Domain::Project);
    Helper::load_json(proj_json["Scene"], "Default Path", scene_path);
    AERO_CORE_INFO("Default Path:{0}", scene_path);
  }
  else
  {
    std::string proj_file_path = project->get_project_file_directory();
    Helper::makefile_read_only(proj_file_path, false);
    json          proj_json;
    std::ifstream file(proj_file_path);
    if (!file.is_open())
    {
      AERO_CORE_INFO("Cannot Open File ! {0}", proj_file_path);
      return;
    }
    file >> proj_json;
    file.close();
    Helper::makefile_read_only(proj_file_path);
    Helper::load_json(proj_json["Scene"], "Default Path", scene_path);
  }

  if (scene_path.empty())
  {
    AERO_CORE_ERROR("No default scene path in project file");
    return;
  }

  std::string full_scene_path =
      project->get_directory() + project->get_scene_directory() + scene_path;
  m_scene = SaveScene::load_scene(full_scene_path);

  if (!m_scene)
  {
    AERO_CORE_ERROR("Failed to load scene: {0}", scene_path);
    return;
  }

  Scene::set_active_scene(m_scene);

  auto entities = m_scene->get_view<Camera_Component>();
  for (auto entityID : entities)
  {
    Entity entity(entityID);
    auto&  props = entity.get_component<Camera_Component>();

    vec2f view_size   = props.size;
    m_view_controller = ViewController::create(view_size, props.center);
    ViewController::set_main_controller(m_view_controller);

    if (entity.has_component<Window_Component>())
    {
      auto& win_props = entity.get_component<Window_Component>();
      Application::get().get_window().set_size(win_props.size);
      Application::get().get_window().center_window();
    }
  }
}

}  // namespace ag
