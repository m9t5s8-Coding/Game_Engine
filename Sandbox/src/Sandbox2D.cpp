#include <Project/Assetmanager.hpp>
#include <Sandbox2D.hpp>

namespace ag {
Sandbox2D::Sandbox2D()
  : Layer("Sandbox2D") {}

void Sandbox2D::on_attach() {
  m_window_size = Application::get().get_window().get_size();

  m_view_controller = ViewController::create(m_window_size, m_window_size / 2);
  ViewController::set_main_controller(m_view_controller);

  m_viewport_size = calculate_viewport(m_window_size, m_window_size / 2);
  FrameBufferSpecification spec;
  spec.attachments = {FrameBuffer_Texture_Format::RGBA8, FrameBuffer_Texture_Format::RED_INTEGER};
  spec.size        = m_window_size;
  m_framebuffer    = FrameBuffer::create(spec);

#ifndef PLATFORM_ANDROID
  load_project_data();
#else
  load_project_data_android();
#endif

  auto project = Project::get_active_project();
  if (!project)
    return;
  SceneManager::scene_changes = [this](AG_ref<Scene> scene) {
    this->m_scene = scene;
    Scene::set_active_scene(scene);
  };
  Project::get_active_project()->get_global_scripts_manager().load_scripts();
  Project::get_active_project()->get_global_scripts_manager().on_create();

  auto config = project->get_server_config();

  if (!config.enabled)
    return;

  m_networking_enabled = true;
  AERO_CORE_INFO("Netorking Enabled");
  m_client.on_connected = []() {
    auto view = Scene::get_active_scene()->get_view<Script_Component>();
    for (auto entity : view) {
      Entity e(entity);
      auto&  comps = e.get_component<Script_Component>();
      if (comps.on_connected.is_valid())
        comps.on_connected.call();
    }
  };

  m_client.on_disconnected = []() {
    auto view = Scene::get_active_scene()->get_view<Script_Component>();
    for (auto entity : view) {
      Entity e(entity);
      auto&  comps = e.get_component<Script_Component>();
      if (comps.on_disconnected.is_valid())
        comps.on_disconnected.call();
    }
  };

  m_client.on_packet_received = [](const Packet& packet) {
    auto view = Scene::get_active_scene()->get_view<Script_Component>();
    for (auto entity : view) {
      Entity e(entity);
      auto&  comps = e.get_component<Script_Component>();
      if (comps.on_packet_received.is_valid())
        comps.on_packet_received.call(packet);
    }
  };

  if (!m_client.connect(config.server_IP, config.port)) {
    AERO_CORE_ERROR("[Client] Failed to Connect to the Server: {0}:{1}",
                    config.server_IP,
                    config.port);
    return;
  }

  NetworkManager::set_client(&m_client);
}

void Sandbox2D::on_detach() {
  if (m_networking_enabled)
    m_client.disconnect();
}

vec2f Sandbox2D::calculate_viewport(const vec2f& screen, const vec2f& view) {
  float screen_aspect = screen.x / screen.y;
  float view_aspect   = view.x / view.y;
  vec2f viewport;

  if (screen_aspect > view_aspect) {
    viewport.y = screen.y;
    viewport.x = (screen.y * view_aspect);
  } else {
    viewport.x = screen.x;
    viewport.y = (screen.x / view_aspect);
  }
  return viewport;
}

void Sandbox2D::on_update(ag::TimeStamp ts) {
  if (m_networking_enabled)
    m_client.update();

  ViewController::set_mouse_position();
  m_view_controller->on_update(ts);

  vec2f view_size = m_view_controller->get_view().get_size();
  Project::get_active_project()->get_global_scripts_manager().on_update(ts);
  m_framebuffer->bind();
  RenderCommand::set_clear_color(ag::Color(38, 45, 42));
  RenderCommand::clear();
  m_framebuffer->clear_attachment(1, -1);
  Renderer2D::begin_scene(m_view_controller->get_view(), view_size);
  m_scene->on_update(ts);
  Renderer2D::end_scene();
  entity_selection();
  m_framebuffer->unbind();

  RenderCommand::set_clear_color(Color::Black);
  RenderCommand::clear();
  Renderer2D::begin_scene();

  Renderer2D::draw_fullscreen_quad(m_framebuffer->get_colorattachment_id(),
                                   m_window_size,
                                   m_viewport_size);
  Renderer2D::end_scene();
}

void Sandbox2D::on_event(Event& event) {
  m_scene->on_event(event);
  EventDispatcher dispatcher(event);
  dispatcher.Dispatch<MouseButtonPressedEvent>(AERO_BIND_EVENT_FN(Sandbox2D::on_mouse_pressed));
  dispatcher.Dispatch<WindowResizeEvent>(AERO_BIND_EVENT_FN(Sandbox2D::on_window_resize));
  dispatcher.Dispatch<WindowCloseEvent>(AERO_BIND_EVENT_FN(Sandbox2D::on_window_close));
}

std::string Sandbox2D::get_appdata_path() {
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

void Sandbox2D::entity_selection() {
  vec2i mouse_position = Mouse::get_mouse_position();
  if (mouse_position.x >= 0 && mouse_position.y >= 0) {
    int pixel_data = m_framebuffer->read_pixel(1, mouse_position);
    if (pixel_data >= 0) {
      Entity e((entt::entity)(pixel_data));
      m_hover_entity = e;
      if (m_hover_entity.has_component<ButtonState_Component>()) {
        auto& props = m_hover_entity.get_component<ButtonState_Component>();
        props.button_state |= Button_State::Hovered;
      }
    } else {
      m_hover_entity = Entity();
    }
  }
}

bool Sandbox2D::on_mouse_pressed(MouseButtonPressedEvent& e) {
  if (e.get_mouse_button() == Button::ButtonLeft) {
    if (!m_hover_entity)
      return false;

    if (m_hover_entity.has_component<ButtonState_Component>()) {
      auto& props = m_hover_entity.get_component<ButtonState_Component>();
      props.button_state |= Button_State::Pressed;
    }
  }
  return false;
}

bool Sandbox2D::on_window_resize(WindowResizeEvent& e) {
  vec2f view_size = m_view_controller->get_view().get_size();
  m_window_size   = e.get_size();
  m_viewport_size = calculate_viewport(m_window_size, view_size);
  m_framebuffer->resize(m_window_size);
  return false;
}

bool Sandbox2D::on_window_close(WindowCloseEvent& e) {
  Application::get().m_running = false;
  return false;
}

void Sandbox2D::load_project_data() {
  std::string exe_dir  = Application::get_exe_directory();
  std::string pak_path = exe_dir + "/game.pak";

  std::string fallback_base;
  fallback_base = get_appdata_path();
  Helper::normalize_path(fallback_base);
  fallback_base += "/AEROEngine";

  AssetManager::init_project(pak_path, fallback_base);

  json j;
  if (AssetManager::is_packed(AssetManager::Domain::Project)) {
    j = AssetManager::read_json("settings.json", AssetManager::Domain::Project);
  } else {
    std::string setting_path = get_appdata_path() + "/AEROEngine/settings.json";
    Helper::normalize_path(setting_path);
    Helper::makefile_read_only(setting_path, false);
    std::ifstream file(setting_path);

    if (!file.is_open()) {
      AERO_CORE_ERROR("Failed to Open File: {0}", setting_path);
      return;
    }
    file >> j;
    file.close();
    Helper::makefile_read_only(setting_path);
  }
  if (j.is_null()) {
    AERO_CORE_ERROR("Failed to load settings.json");
    return;
  }

  std::string project_path;
  Helper::load_json(j["Project"], "File Path", project_path, std::string(""));
  if (!AssetManager::is_packed(AssetManager::Domain::Project)) {
    AssetManager::set_fallback_path(project_path, AssetManager::Domain::Project);
  }

  if (project_path.empty()) {
    AERO_CORE_ERROR("No project path in settings.json");
    return;
  }

  if (AssetManager::is_packed(AssetManager::Domain::Project)) {
    Project::load_project(project_path);
  } else {
    Project::load_project(project_path);
  }

  auto project = Project::get_active_project();
  if (!project) {
    AERO_CORE_ERROR("Failed to load project");
    return;
  }

  Project::get_active_project()->init();

  std::string scene_path;

  if (AssetManager::is_packed(AssetManager::Domain::Project)) {
    std::string proj_file_path = project->get_project_file_directory();
    json        proj_json = AssetManager::read_json(proj_file_path, AssetManager::Domain::Project);
    Helper::load_json(proj_json["Scene"], "Default Path", scene_path, std::string(""));
    AERO_CORE_INFO("Default Path:{0}", scene_path);
  } else {
    std::string proj_file_path = project->get_project_file_directory();
    Helper::makefile_read_only(proj_file_path, false);
    json          proj_json;
    std::ifstream file(proj_file_path);
    if (!file.is_open()) {
      AERO_CORE_INFO("Cannot Open File ! {0}", proj_file_path);
      return;
    }
    file >> proj_json;
    file.close();
    Helper::makefile_read_only(proj_file_path);
    Helper::load_json(proj_json["Scene"], "Default Path", scene_path, std::string(""));
  }

  if (scene_path.empty()) {
    AERO_CORE_ERROR("No default scene path in project file");
    return;
  }

  std::string full_scene_path =
      project->get_directory() + project->get_scene_directory() + scene_path;
  m_scene = SaveScene::load_scene(full_scene_path);

  if (!m_scene) {
    AERO_CORE_ERROR("Failed to load scene: {0}", scene_path);
    return;
  }

  Scene::set_active_scene(m_scene);
  SceneManager::add_scene(m_scene);

  auto entities = m_scene->get_view<Camera_Component>();
  for (auto entityID : entities) {
    Entity entity(entityID);
    auto&  props = entity.get_component<Camera_Component>();

    vec2f view_size   = props.size;
    m_view_controller = ViewController::create(view_size, props.center);
    ViewController::set_main_controller(m_view_controller);
    m_framebuffer->resize(view_size);

    if (entity.has_component<Window_Component>()) {
      auto& win_props = entity.get_component<Window_Component>();
      Application::get().get_window().set_size(win_props.size);
      Application::get().get_window().center_window();
    }
  }
}

#ifdef PLATFORM_ANDROID
void Sandbox2D::load_project_data_android() {
  AssetManager::init_project("game.pak", "");

  auto files = AssetManager::list_files(AssetManager::Domain::Project);
  for (auto& f : files) AERO_CORE_INFO("PAK contains: {}", f);
  json j;
  j = AssetManager::read_json("settings.json", AssetManager::Domain::Project);
  if (j.is_null()) {
    AERO_CORE_ERROR("Failed to load settings.json");
    return;
  }

  std::string project_path;
  Helper::load_json(j["Project"], "File Path", project_path, std::string(""));
  if (project_path.empty()) {
    AERO_CORE_ERROR("No project path in settings.json");
    return;
  }

  Project::load_project(project_path);

  auto project = Project::get_active_project();
  if (!project) {
    AERO_CORE_ERROR("Failed to load project");
    return;
  }

  Project::get_active_project()->init();

  std::string scene_path;

  {
    std::string proj_file_path = project->get_project_file_directory();
    json        proj_json = AssetManager::read_json(proj_file_path, AssetManager::Domain::Project);
    Helper::load_json(proj_json["Scene"], "Default Path", scene_path, std::string(""));
    AERO_CORE_INFO("Default Path:{0}", scene_path);
  }

  if (scene_path.empty()) {
    AERO_CORE_ERROR("No default scene path in project file");
    return;
  }

  std::string full_scene_path =
      project->get_directory() + project->get_scene_directory() + scene_path;
  m_scene = SaveScene::load_scene(full_scene_path);

  if (!m_scene) {
    AERO_CORE_ERROR("Failed to load scene: {0}", scene_path);
    return;
  }

  Scene::set_active_scene(m_scene);
  SceneManager::add_scene(m_scene);

  auto entities = m_scene->get_view<Camera_Component>();
  for (auto entityID : entities) {
    Entity entity(entityID);
    auto&  props = entity.get_component<Camera_Component>();

    m_view_controller = ViewController::create(props.size, props.center);
    ViewController::set_main_controller(m_view_controller);
    m_framebuffer->resize(props.size);
  }
}
#endif

}  // namespace ag
