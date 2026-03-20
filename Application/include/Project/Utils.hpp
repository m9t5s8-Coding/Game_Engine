#pragma once

namespace ag
{
class Platform
{
  static void        reload_app();
  static std::string get_appdata_path();
  static void        create_app_folder();
  static std::string get_exe_path();
};
}  // namespace ag
