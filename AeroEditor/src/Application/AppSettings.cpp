#include "Core/Log.hpp"

#include <Application/AppSettings.hpp>
#include <Core/Application.hpp>
#include <Core/Core.hpp>
#include <filesystem>
#include <Helper.hpp>
#include <nlohmann/json.hpp>
#include <Project/FileDialogs.hpp>
using json = nlohmann::json;

#ifdef PLATFORM_WINDOWS
  #include <windows.h>
#elif defined(PLATFORM_LINUX) && !defined(PLATFORM_ANDROID)
  #include <limits.h>
  #include <unistd.h>
#endif

#if (defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)) && !defined(PLATFORM_ANDROID)
  #include <GLFW/glfw3.h>
#endif

namespace ag {
AppSettings::Mode AppSettings::s_mode                 = AppSettings::Mode::ProjectManager;
WindowState       AppSettings::s_window_state         = WindowState{};
std::string       AppSettings::s_settings_path        = std::string("");
std::string       AppSettings::s_recent_projects_path = std::string("");
bool              AppSettings::s_loaded               = false;

void AppSettings::load() {
  if (s_loaded)
    return;
  create_app_folder();
  s_loaded = true;
}

void AppSettings::ensure_loaded() {
  if (s_loaded)
    return;

  create_app_folder();
  s_loaded = true;
}

std::string AppSettings::get_appdata_path() {
#ifdef PLATFORM_WINDOWS
  const char* appdata = std::getenv("APPDATA");
  if (appdata)
    std::string str_appdata(appdata);

  return std::string(appdata);
  else return ".";

#elif defined(PLATFORM_LINUX)
  const char* home = std::getenv("HOME");
  if (home)
    return std::string(home) + "/.config";
  else
    return ".";

#elif defined(PLATFORM_ANDROID)
  extern android_app* g_android_app;

  if (g_android_app && g_android_app->activity) {
    std::string internal_path = g_android_app->activity->internalDataPath;
    AERO_CORE_INFO("Android Internal Path: {0}", internal_path.c_str());
    return internal_path;
  } else {
    return "/data/data/com.yourname.aeroengine/files";
  }
#endif
}

void AppSettings::create_app_folder() {
  std::string app_data_path = get_appdata_path();
  Helper::normalize_path(app_data_path);
  if (app_data_path.empty()) {
    AERO_CORE_ERROR("App Data Path is Empty");
    return;
  }

  const std::string app_folder = app_data_path + "/AEROEngine";
  s_settings_path              = app_folder + "/settings.json";
  s_recent_projects_path       = app_folder + "/recent_projects.json";

  // create app folder
  try {
    if (!std::filesystem::exists(app_folder)) {
      if (!std::filesystem::create_directories(app_folder)) {
        AERO_CORE_ERROR("Failed to create directories: {}", app_folder);
        return;
      }
      AERO_CORE_INFO("App folder created successfully");
    } else {
      AERO_CORE_INFO("App folder already exists");
    }
  } catch (const std::filesystem::filesystem_error& e) {
    AERO_CORE_ERROR("Failed to create app folder: {}", e.what());
    AERO_CORE_ERROR("Path: {}", app_folder);
    return;
  }

  // Create or Load Settings.json
  create_load_settings();
  create_load_recent_projects();
}

void AppSettings::create_load_settings() {
  Helper::makefile_read_only(s_settings_path, false);

  AERO_CORE_INFO("Settings path: {}", s_settings_path);

  if (!std::filesystem::exists(s_settings_path)) {
    try {
      AERO_CORE_INFO("Creating new settings file");
      std::ofstream file(s_settings_path);
      if (!file.is_open()) {
        AERO_CORE_ERROR("Failed to Open File: {}", s_settings_path);
        return;
      }
      json j;
      Helper::save_json(j,
                        "Mode",
                        static_cast<int>(Mode::ProjectManager),
                        (int)Mode::ProjectManager);
      file << j.dump(4);
      file.close();
      Helper::makefile_read_only(s_settings_path);
      AERO_CORE_INFO("Settings File created at: {}", s_settings_path);
    } catch (const std::exception& e) {
      AERO_CORE_ERROR("Exception creating settings.json: {}", e.what());
    }
  } else {
    try {
      AERO_CORE_INFO("Loading existing settings file");
      std::ifstream file(s_settings_path);

      if (!file.is_open()) {
        AERO_CORE_ERROR("Failed to Open File: {}", s_settings_path);
        return;
      }

      file.seekg(0, std::ios::end);
      if (file.tellg() == 0) {
        AERO_CORE_WARN("Settings Path is empty using Defaults");
        file.close();
        Helper::makefile_read_only(s_settings_path);
        return;
      }
      file.seekg(0, std::ios::beg);

      json j;
      j = json::parse(file, nullptr, false);
      file.close();
      Helper::makefile_read_only(s_settings_path);

      if (j.is_discarded()) {
        AERO_CORE_ERROR("Settings file has invalid JSON, using defaults");
        return;
      }

      int mode;
      Helper::load_json(j, "Mode", mode, (int)Mode::ProjectManager);
      s_mode = static_cast<Mode>(mode);
      AERO_CORE_INFO("Settings loaded, mode: {}", mode);

      if (j.contains("Window"))
      {
        const json& window = j["Window"];
        if (window.contains("Width") && window.contains("Height"))
        {
          Helper::load_json(window, "Width", s_window_state.width, 1280);
          Helper::load_json(window, "Height", s_window_state.height, 720);
          s_window_state.has_size = s_window_state.width > 0 && s_window_state.height > 0;
        }
        if (window.contains("X") && window.contains("Y"))
        {
          Helper::load_json(window, "X", s_window_state.x, 0);
          Helper::load_json(window, "Y", s_window_state.y, 0);
          s_window_state.has_position = true;
        }
        Helper::load_json(window, "FullScreen", s_window_state.fullscreen, false);
        Helper::load_json(window, "Maximized", s_window_state.maximized, false);
        Helper::load_json(window, "Vsync", s_window_state.vsync, true);
      }
    } catch (const std::exception& e) {
      AERO_CORE_ERROR("Exception loading settings: {}", e.what());
    }
  }
}

void AppSettings::create_load_recent_projects() {
  Helper::makefile_read_only(s_recent_projects_path, false);

  AERO_CORE_INFO("Recent projects path: {}", s_recent_projects_path);

  if (!std::filesystem::exists(s_recent_projects_path)) {
    try {
      AERO_CORE_INFO("Creating new recent projects file");
      std::ofstream file(s_recent_projects_path);
      if (!file.is_open()) {
        AERO_CORE_ERROR("Failed to create recent projects file");
        return;
      }
      file << "[]";
      file.close();
      Helper::makefile_read_only(s_recent_projects_path);
      AERO_CORE_INFO("Recent project file created at: {}", s_recent_projects_path);
    } catch (const std::exception& e) {
      AERO_CORE_ERROR("Exception creating recent projects: {}", e.what());
    }
  } else {
    AERO_CORE_INFO("Recent projects file already exists");
  }
}

WindowState AppSettings::get_window_state() {
  ensure_loaded();
  return s_window_state;
}

void AppSettings::save_window_state(const WindowState& state) {
  ensure_loaded();
  s_window_state = state;

  Helper::makefile_read_only(s_settings_path, false);

  json j;
  std::ifstream in_file(s_settings_path);
  if (in_file.is_open())
  {
    try
    {
      in_file >> j;
    }
    catch (const std::exception&)
    {
      j = json::object();
    }
    in_file.close();
  }
  if (!j.is_object())
    j = json::object();

  if (state.has_size && state.width > 0 && state.height > 0)
  {
    j["Window"]["Width"]  = state.width;
    j["Window"]["Height"] = state.height;
  }
  if (state.has_position)
  {
    j["Window"]["X"] = state.x;
    j["Window"]["Y"] = state.y;
  }
  j["Window"]["FullScreen"] = state.fullscreen;
  j["Window"]["Maximized"]  = state.maximized;
  j["Window"]["Vsync"]      = state.vsync;

  std::ofstream out_file(s_settings_path);
  if (out_file.is_open())
  {
    out_file << j.dump(4);
    out_file.close();
  }
  else
  {
    AERO_CORE_ERROR("Failed to save settings: {0}", s_settings_path);
  }

  Helper::makefile_read_only(s_settings_path);
}

void AppSettings::save_current_window_state() {
  WindowState state = s_window_state;
  Window&     window = Application::get().get_window();

  const vec2u size = window.get_size();
  if (size.x > 0 && size.y > 0)
  {
    state.width    = static_cast<int>(size.x);
    state.height   = static_cast<int>(size.y);
    state.has_size = true;
  }

  state.fullscreen = window.is_full_screen();
  state.vsync      = window.is_vsync();

#if (defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)) && !defined(PLATFORM_ANDROID)
  GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window.get_native_window());
  if (glfw_window)
  {
    state.maximized = glfwGetWindowAttrib(glfw_window, GLFW_MAXIMIZED) != 0;
    const vec2u position = window.get_position();
    state.x           = static_cast<int>(position.x);
    state.y           = static_cast<int>(position.y);
    state.has_position = true;
  }
#endif

  save_window_state(state);
}

void AppSettings::restore_window_state() {
  ensure_loaded();
  Window& window = Application::get().get_window();

  if (s_window_state.has_size)
    window.set_size({static_cast<AG_uint>(s_window_state.width),
                     static_cast<AG_uint>(s_window_state.height)});

  if (s_window_state.has_position && !s_window_state.fullscreen && !s_window_state.maximized)
    window.set_position({s_window_state.x, s_window_state.y});

  window.set_vsync(s_window_state.vsync);

#if (defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)) && !defined(PLATFORM_ANDROID)
  GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window.get_native_window());
  if (glfw_window && s_window_state.maximized)
    glfwSetWindowAttrib(glfw_window, GLFW_MAXIMIZED, GLFW_TRUE);
  else if (s_window_state.fullscreen && !window.is_full_screen())
    window.set_full_screen();
#else
  if (s_window_state.fullscreen && !window.is_full_screen())
    window.set_full_screen();
#endif
}

void AppSettings::reload_app() {
  reload_app(std::string());
}

void AppSettings::reload_app(const std::string& project_path) {
  std::string exePath = get_exe_path();

  if (exePath.empty()) {
    AERO_CORE_ERROR("Cannot reload app - executable path is empty");
    return;
  }

  std::wstring exe(exePath.begin(), exePath.end());
  if (project_path.empty()) {
    FileDialogs::run_exe(exe);
  } else {
    std::wstring arg(project_path.begin(), project_path.end());
    FileDialogs::run_exe(exe, arg);
  }

  exit(0);
}

std::string AppSettings::get_exe_path() {
#ifdef PLATFORM_WINDOWS
  char buffer[MAX_PATH];
  GetModuleFileNameA(nullptr, buffer, MAX_PATH);
  return std::string(buffer);

#elif defined(PLATFORM_ANDROID)
  // On Android, we don't have a direct executable path
  if (g_android_app && g_android_app->activity) {
    std::string path = g_android_app->activity->externalDataPath;
    if (path.empty()) {
      path = "/data/app/com.yourname.aeroengine";
    }
    return path;
  }
  return "";

#elif defined(PLATFORM_LINUX)
  char    buffer[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
  if (len != -1) {
    buffer[len] = '\0';
    return std::string(buffer);
  } else {
    AERO_CORE_ERROR("Failed to get executable path");
    return "";
  }
#endif
}

}  // namespace ag
