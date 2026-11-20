#pragma once

#include <string>

namespace ag {
struct WindowState
{
  bool has_size     = false;
  bool has_position = false;
  int  width        = 1280;
  int  height       = 720;
  int  x            = 0;
  int  y            = 0;
  bool fullscreen   = false;
  bool maximized    = false;
  bool vsync        = true;
};

class AppSettings {
public:
  enum class Mode {
    ProjectManager,
    Editor
  };

  static void load();

  static void ensure_loaded();

  static Mode get_mode() {
    ensure_loaded();
    return s_mode;
  }
  static void set_mode(Mode mode) {
    s_mode = mode;
    save_mode();
  }

  static std::string get_settings_path() {
    ensure_loaded();
    return s_settings_path;
  }
  static std::string get_recent_projects_path() {
    ensure_loaded();
    return s_recent_projects_path;
  }

  static WindowState get_window_state();
  static void        save_window_state(const WindowState& state);
  static void        save_current_window_state();
  static void        restore_window_state();

  static void reload_app();
  // Restart the editor and pass a project folder or .aeroproj file as argv[1].
  static void reload_app(const std::string& project_path);

private:
  static void        create_app_folder();
  static void        create_load_settings();
  static void        create_load_recent_projects();
  static void        save_mode();
  static std::string get_appdata_path();
  static std::string get_exe_path();

  static Mode        s_mode;
  static WindowState s_window_state;
  static std::string s_settings_path;
  static std::string s_recent_projects_path;
  static bool        s_loaded;
};
}  // namespace ag
