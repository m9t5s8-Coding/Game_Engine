#pragma once

#include <string>

using json = nlohmann::json;

namespace ag {
class AppSettings {
public:
  enum class Mode {
    ProjectManager,
    Editor
  };

  static void load();

  static void ensure_loaded();

  static void get_mode() { ensure_loaded(); }
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

  static void reload_app();

private:
  static void        create_app_folder();
  static void        create_load_settings();
  static void        create_load_recent_projects();
  static void        save_mode();
  static std::string get_appdata_path();
  static std::string get_exe_path();

  static Mode        s_mode;
  static std::string s_settings_path;
  static std::string s_recent_projects_path;
  static bool        s_loaded;
};
}  // namespace ag
