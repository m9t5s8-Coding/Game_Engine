#include <Apch.hpp>
#include <Core/Application.hpp>
#include <Helper.hpp>
#include <Project/Assetmanager.hpp>
#include <Project/GlobalScripts.hpp>
#include <Project/Project.hpp>
#include <Project/SaveScene.hpp>

namespace fs = std::filesystem;

namespace ag
{

void Project::init()
{
  m_global_script_manager = AG_cscope<GlobalScriptsManager>();
}

AG_ref<Project> Project::new_project(const std::string& path)
{
  std::string project_path = path;
  Helper::normalize_path(project_path);
  auto project = AG_cref<Project>();

  project->set_directory(project_path);
  project->set_name(fs::path(path).filename().string());

  project->m_assets_directory  = "/Assets";
  project->m_scenes_directory  = "/Scenes";
  project->m_scripts_directory = "/Scripts";
  project->m_project_file_path = project_path + "/" + project->m_name + ".aeroproj";

  fs::create_directories(project_path + project->m_assets_directory);
  fs::create_directories(project_path + project->m_scenes_directory);
  fs::create_directories(project_path + project->m_scripts_directory);

  set_active_project(project);
  project->save();
  return project;
}

AG_ref<Project> Project::load_project(const std::string& path)
{
  auto project = AG_cref<Project>();

  json j;
  if (AssetManager::is_packed(AssetManager::Domain::Project))
  {
    std::string project_file_path = path;

    j = AssetManager::read_json(project_file_path, AssetManager::Domain::Project);
    if (j.is_null())
    {
      AERO_CORE_ERROR("Project::load_project: Failed to read {0} from .pak", project_file_path);
      project->m_project_loaded = false;
      return project;
    }

    Helper::load_json(j["Project"], "Name", project->m_name, std::string(""));
    Helper::load_json(j["Project"], "Assets", project->m_assets_directory, std::string(""));
    Helper::load_json(j["Project"], "Scenes", project->m_scenes_directory, std::string(""));
    Helper::load_json(j["Project"], "Scripts", project->m_scripts_directory, std::string(""));

    project->m_directory         = project->m_name;
    project->m_project_file_path = project_file_path;
    project->m_project_loaded    = true;
  }
  else
  {
    std::string file_path = path;
    std::string p         = path;
    Helper::normalize_path(file_path);
    Helper::normalize_path(p);

    if (std::filesystem::is_directory(file_path))
    {
      for (const auto& entry : std::filesystem::directory_iterator(file_path))
      {
        if (entry.path().extension() == ".aeroproj")
        {
          file_path = entry.path().string();
          Helper::normalize_path(file_path);
          break;
        }
      }
      if (file_path == p)
      {
        AERO_CORE_INFO("Project not Found!");
        project->m_project_loaded = false;
        return project;
      }
    }

    Helper::makefile_read_only(file_path, false);
    std::ifstream in(file_path);
    if (!in.is_open())
    {
      project->m_project_loaded = false;
      return project;
    }

    in >> j;
    in.close();
    Helper::makefile_read_only(file_path);

    project->m_directory         = std::filesystem::path(file_path).parent_path().string();
    project->m_project_file_path = file_path;
    project->m_project_loaded    = true;

    Helper::load_json(j["Project"], "Name", project->m_name, std::string(""));
    Helper::load_json(j["Project"], "Assets", project->m_assets_directory, std::string(""));
    Helper::load_json(j["Project"], "Scenes", project->m_scenes_directory, std::string(""));
    Helper::load_json(j["Project"], "Scripts", project->m_scripts_directory, std::string(""));
  }

  Helper::load_json(j["Network"], "Enabled", project->m_server_config.enabled, false);
  if (project->m_server_config.enabled)
  {
    Helper::load_json(j["Network"], "Port", project->m_server_config.port, (uint16_t)7777);
    Helper::load_json(j["Network"], "MaxClients", project->m_server_config.max_clients, 5);
    Helper::load_json(j["Network"], "TickRate", project->m_server_config.tick_rate, 20);
    Helper::load_json(j["Network"],
                      "ConnectionTimeout",
                      project->m_server_config.connection_timeout,
                      10);
    Helper::load_json(j["Network"], "Reconnect", project->m_server_config.auto_reconnect, true);
    Helper::load_json(j["Network"],
                      "ReconnectTries",
                      project->m_server_config.max_reconnection_tries,
                      3);
    Helper::load_json(j["Network"],
                      "ServerIP",
                      project->m_server_config.server_IP,
                      std::string("127.0.0.1"));

    if (j["Network"].contains("scripts"))
      project->m_server_config.scripts = j["Network"]["scripts"].get<std::vector<std::string>>();
  }
  if (j["Global"].contains("scripts"))
    project->m_global_scripts.global_scripts =
        j["Global"]["scripts"].get<std::vector<std::string>>();

  AERO_CORE_INFO("Project Loaded Successfully: {0}", project->m_name);

  project->m_from_pak = AssetManager::is_packed(AssetManager::Domain::Project);

  Project::set_active_project(project);

  return project;
}

AG_ref<Project> Project::save_project()
{
  auto project = get_active_project();
  json j;

  Helper::makefile_read_only(project->get_project_file_directory(), false);
  std::ifstream in(project->get_project_file_directory());
  in >> j;
  in.close();
  Helper::makefile_read_only(project->get_project_file_directory());

  auto& window = Application::get().get_window();
  j["Window"]  = {
      {     "Title",           window.get_title()},
      {      "Size",     window.get_size().save()},
      {"FullScreen",      window.is_full_screen()},
      {    "Center",                        false},
      {  "Position", window.get_position().save()},
      {     "Vsync",            window.is_vsync()}
  };

  Helper::save_json(j["Project"], "Directory", project->get_directory(), std::string(""));
  Helper::save_json(j["Project"],
                    "File Path",
                    project->get_project_file_directory(),
                    std::string(""));

  Helper::save_json(j["Network"], "Enabled", project->m_server_config.enabled, false);
  if (project->m_server_config.enabled)
  {
    Helper::save_json(j["Network"], "Port", project->m_server_config.port, (uint16_t)7777);
    Helper::save_json(j["Network"], "MaxClients", project->m_server_config.max_clients, 5);
    Helper::save_json(j["Network"], "TickRate", project->m_server_config.tick_rate, 20);
    Helper::save_json(j["Network"],
                      "ConnectionTimeout",
                      project->m_server_config.connection_timeout,
                      10);
    Helper::save_json(j["Network"], "Reconnect", project->m_server_config.auto_reconnect, true);
    Helper::save_json(j["Network"],
                      "ReconnectTries",
                      project->m_server_config.max_reconnection_tries,
                      3);
    Helper::save_json(j["Network"],
                      "ServerIP",
                      project->m_server_config.server_IP,
                      std::string("127.0.0.1"));
    j["Network"]["scripts"] = project->m_server_config.scripts;
  }

  j["Global"]["scripts"] = project->m_global_scripts.global_scripts;

  Helper::makefile_read_only(project->get_project_file_directory(), false);
  std::ofstream out(project->get_project_file_directory());
  out << j.dump(4);
  out.close();
  Helper::makefile_read_only(project->get_project_file_directory());
  return project;
}

void Project::save_active()
{
  if (s_active_project)
    s_active_project->save();
}

void Project::save()
{
  nlohmann::json j;
  auto           p = Project::get_active_project();
  Helper::save_json(j["Project"], "Name", p->m_name, std::string(""));
  Helper::save_json(j["Project"], "Directory", p->m_directory, std::string(""));
  Helper::save_json(j["Project"], "File Path", p->m_project_file_path, std::string(""));
  Helper::save_json(j["Project"], "Assets", p->m_assets_directory, std::string(""));
  Helper::save_json(j["Project"], "Scenes", p->m_scenes_directory, std::string(""));
  Helper::save_json(j["Project"], "Scripts", p->m_scripts_directory, std::string(""));

  Helper::save_json(j["Window"], "Title", p->m_name, std::string(""));
  Helper::save_json(j["Window"], "Size", vec2u(1280, 720), vec2u(1280, 720));
  Helper::save_json(j["Window"], "FullScreen", false, false);
  Helper::save_json(j["Window"], "Center", true, true);
  Helper::save_json(j["Window"], "Position", vec2i(0, 0), vec2i(0, 0));
  Helper::save_json(j["Window"], "Vsync", true, true);

  auto scene = AG_cref<Scene>();
  Scene::set_active_scene(scene);
  scene->set_name("main");
  scene->set_directory("/" + scene->get_name() + ".aeroscene");
  Helper::save_json(j["Scene"], "Last Loaded", scene->get_name(), std::string(""));
  Helper::save_json(j["Scene"], "Last Loaded Path", scene->get_directory(), std::string(""));
  Helper::save_json(j["Scene"], "Default", scene->get_name(), std::string(""));
  Helper::save_json(j["Scene"], "Default Path", scene->get_directory(), std::string(""));

  std::string scene_path = p->m_directory + p->m_scenes_directory + "/" + scene->get_directory();
  SaveScene::save_scene(scene, scene_path);

  Helper::makefile_read_only(p->m_project_file_path, false);
  std::ofstream out(p->m_project_file_path);
  out << j.dump(4);
  out.close();
  Helper::makefile_read_only(p->m_project_file_path);
}
}  // namespace ag
