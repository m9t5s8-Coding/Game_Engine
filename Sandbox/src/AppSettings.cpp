#include <AppSettings.hpp>
#include <Windows.h> 

AppSettings::Mode AppSettings::s_mode = AppSettings::Mode::ProjectManager;
std::string AppSettings::s_settings_path = "";
std::string AppSettings::s_recent_projects_path = "";

void AppSettings::load()
{
  create_app_folder();
}

std::string AppSettings::get_appdata_path()
{
  const char* appdata = std::getenv("APPDATA");

  if (appdata)
    return std::string(appdata);
  else
    return ".";
}
void AppSettings::create_app_folder()
{
  std::string app_folder = get_appdata_path() + "\\AEROEngine";
  s_settings_path = app_folder + "\\settings.json";
  s_recent_projects_path = app_folder + "\\recent_projects.json";

  // create app folder
  if (!std::filesystem::exists(app_folder))
    std::filesystem::create_directories(app_folder);

  // create app setting json file
  if (!std::filesystem::exists(s_settings_path))
  {
    std::ofstream file(s_settings_path);
    if (file.is_open())
    {
      json j;
      j["Mode"] = "ProjectManager";
      file << j.dump(4);
      file.close();
      AERO_CORE_INFO("Settings File created at: {0}", s_settings_path);
    }
  }
  else
  {
    json j;
    std::ifstream file(s_settings_path);

    if (!file.is_open())
    {
      AERO_CORE_ERROR("Failed to Open File!");
      return;
    }

    file >> j;
    file.close();
    if (j.contains("Mode") && j["Mode"].is_string())
    {
      std::string mode_str = j["Mode"];
      if (mode_str == "Editor")
        s_mode = Mode::Editor;
      else
        s_mode = Mode::ProjectManager;
    }
    else
    {
      s_mode = Mode::ProjectManager;
    }
  }

  // create recent projects file
  if (!std::filesystem::exists(s_recent_projects_path))
  {
    std::ofstream file(s_recent_projects_path);
    if (file.is_open())
    {
      file.close();
      AERO_CORE_INFO("Recent Projects File created at: {0}", s_recent_projects_path);
    }
  }
  else
  {
  }

}


void AppSettings::reload_app()
{

  std::string exePath = get_exe_path();

  STARTUPINFOA si = { sizeof(si) };
  PROCESS_INFORMATION pi;

  // Start new process
  if (CreateProcessA(
    exePath.c_str(),
    nullptr,   // Command line args
    nullptr, nullptr, FALSE,
    0, nullptr, nullptr,
    &si, &pi))
  {
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  }

  exit(0);
}

std::string AppSettings::get_exe_path()
{
  char buffer[MAX_PATH];
  GetModuleFileNameA(nullptr, buffer, MAX_PATH);
  return std::string(buffer);
}
