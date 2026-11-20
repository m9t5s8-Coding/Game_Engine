#include <Core/Application.hpp>
#include <Core/Core.hpp>
#include <Core/Time.hpp>
#include <filesystem>
#include <GameObjects/NodeFactory.hpp>
#include <Helper.hpp>
#include <Project/Assetmanager.hpp>
#include <Renderer/BufferLayout.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Renderer2D.hpp>

#ifdef PLATFORM_WINDOWS
  #include <windows.h>
#elif defined(PLATFORM_LINUX)
  #include <limits.h>
  #include <unistd.h>
#elif defined(PLATFORM_ANDROID)
  #include <Platform/Android/AndroidWindow.hpp>
#endif

#ifdef PLATFORM_WEB
  #include <emscripten/emscripten.h>
#endif

namespace ag {

Application* Application::s_Instance = nullptr;
std::vector<std::string> Application::s_command_line;

void Application::set_command_line(int argc, char** argv) {
  s_command_line.clear();
  for (int i = 1; i < argc; ++i)
  {
    if (argv[i])
      s_command_line.emplace_back(argv[i]);
  }
}

std::string Application::get_project_path_argument() {
  namespace fs = std::filesystem;
  const std::string prefixed = "--project=";

  for (size_t i = 0; i < s_command_line.size(); ++i)
  {
    std::string candidate;
    const std::string& arg = s_command_line[i];

    if (arg == "--project" || arg == "-p")
    {
      if (i + 1 >= s_command_line.size())
        continue;
      candidate = s_command_line[++i];
    }
    else if (arg.rfind(prefixed, 0) == 0)
    {
      candidate = arg.substr(prefixed.size());
    }
    else
    {
      candidate = arg;
    }

    if (candidate.empty() || candidate[0] == '-')
      continue;

    std::error_code ec;
    fs::path        path(candidate);
    if (!fs::exists(path, ec) || ec)
      continue;

    const bool is_project = fs::is_directory(path, ec) || path.extension() == ".aeroproj";
    if (ec || !is_project)
      continue;

    fs::path    absolute = fs::absolute(path, ec);
    std::string result   = ec ? path.string() : absolute.string();
    Helper::normalize_path(result);
    return result;
  }

  return std::string();
}

Application::Application() {
  s_Instance = this;
  AERO_CORE_INFO("Application Contstuctor Created");
}

Application::~Application() = default;

void Application::init(const WindowProps& props) {
#ifdef PLATFORM_ANDROID
  AssetManager::init_engine("assets.pak", "");
  m_Window = std::unique_ptr<Window>(Window::create(props));
  m_Window->set_event_callback(AERO_BIND_EVENT_FN(Application::on_event));
  m_audio_device = AG_scope<Audio_Device>(Audio_Device::create());
  m_audio_device->init();

#else
  ag::AssetManager::init_engine("assets.pak", get_exe_directory() + "/assets");
  m_Window = std::unique_ptr<Window>(Window::create(props));
  m_Window->set_event_callback(AERO_BIND_EVENT_FN(Application::on_event));
  m_audio_device = AG_scope<Audio_Device>(Audio_Device::create());
  m_audio_device->init();

  #ifdef AERO_EDITOR
  m_imgui_layer = new ImGuiLayer();
  push_overlay(m_imgui_layer);
  #endif

#endif
}

void Application::run() {
  on_create();

#ifdef PLATFORM_WEB
  // Browsers cannot run a blocking loop; hand control to the browser and let
  // it call run_frame() once per animation frame.
  emscripten_set_main_loop_arg(
      [](void* arg) { static_cast<Application*>(arg)->run_frame(); },
      this,
      0,
      true);
#else
  while (m_running) {
    float     time      = static_cast<float>(Time::get_time());
    TimeStamp timestamp = time - m_last_frametime;
    delta_time          = timestamp.get_seconds();
    m_last_frametime    = time;

    if (!m_minimized && m_app_active) {
      for (Layer* layer : m_layerstack) layer->on_update(timestamp);
    }
#ifdef AERO_EDITOR
    if (m_imgui_layer) {
      m_imgui_layer->begin();

      for (Layer* layer : m_layerstack) layer->on_imgui_render();

      m_imgui_layer->end();
    }
#endif

    m_Window->on_update();
  }
  on_destroy();
#endif
}

void Application::run_frame() {
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_WEB)
  // A suspended Android app has already had its surface cleared to black by
  // AndroidWindow. Skipping the frame here keeps the screen dark and avoids a
  // huge delta time spike when the app returns from the background.
  if (!is_app_active())
    return;

  float     time      = static_cast<float>(Time::get_time());
  TimeStamp timestamp = time - m_last_frametime;
  delta_time          = timestamp.get_seconds();
  m_last_frametime    = time;

  for (Layer* layer : m_layerstack) layer->on_update(timestamp);
  m_Window->on_update();

#ifdef PLATFORM_WEB
  if (!m_running) {
    emscripten_cancel_main_loop();
    on_destroy();
  }
#endif
#endif
}

void Application::push_layer(Layer* layer) {
  m_layerstack.push_layer(layer);
  layer->on_attach();
}

void Application::pop_layer(Layer* layer) {
  m_layerstack.pop_layer(layer);
  layer->on_detach();
}

void Application::push_overlay(Layer* overlay) {
  m_layerstack.push_overlay(overlay);
  overlay->on_attach();
}

void Application::on_event(Event& e) {
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>(AERO_BIND_EVENT_FN(Application::on_window_close));
  dispatcher.Dispatch<WindowResizeEvent>(AERO_BIND_EVENT_FN(Application::on_window_resize));
  dispatcher.Dispatch<KeyPressedEvent>(AERO_BIND_EVENT_FN(Application::on_key_pressed));

#ifdef AERO_EDITOR
  if (m_imgui_layer)
    m_imgui_layer->on_event(e);
#endif

  for (auto it = m_layerstack.end(); it != m_layerstack.begin();) {
    (*--it)->on_event(e);
    if (e.Handled)
      break;
  }
}

void Application::on_app_pause() {
  for (auto it = m_layerstack.end(); it != m_layerstack.begin();) {
    (*--it)->on_app_pause();
  }
}

void Application::on_app_resume() {
  // Reset time so scripted animations do not jump after a long pause.
  m_last_frametime = static_cast<float>(Time::get_time());
  delta_time       = 0.0f;
  for (Layer* layer : m_layerstack) layer->on_app_resume();
}

bool Application::on_window_close(WindowCloseEvent& e) {
  /* m_running = false;*/
  m_is_closing = true;
  return false;
}

bool Application::on_window_resize(WindowResizeEvent& e) {
  if (e.get_width() == 0 || e.get_height() == 0) {
    m_minimized = true;
    return true;
  }
  m_minimized = false;
  Renderer::on_window_resize(e.get_size());
  return false;
}
bool Application::on_key_pressed(KeyPressedEvent& e) {
  if (e.get_key_code() == Key::F11) {
    m_Window->set_full_screen();
  }
  return false;
}

std::string Application::get_exe_directory() {
#ifdef PLATFORM_WINDOWS
  char  path[MAX_PATH];
  DWORD len = GetModuleFileNameA(nullptr, path, MAX_PATH);
  if (len == 0)
    return ".";
  std::string full(path, len);
  size_t      pos = full.find_last_of("\\/");
  if (pos != std::string::npos)
    return full.substr(0, pos);
  return ".";

#elif defined(PLATFORM_LINUX)
  char    result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  if (count == -1)
    return ".";
  std::string full(result, count);
  size_t      pos = full.find_last_of('/');
  if (pos != std::string::npos)
    return full.substr(0, pos);
  return ".";

#else
  return ".";
#endif
}

}  // namespace ag
