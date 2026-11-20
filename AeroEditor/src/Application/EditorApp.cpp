#include <Application/EditorApp.hpp>

#include <utility>

namespace ag {
EditorApp::EditorApp(std::string project_path) : m_project_path(std::move(project_path)) {}

void EditorApp::on_create() {
  NodeFactory::init();
  load_project_data();
  Renderer::init();

  ag::Log::init_with_imGui();
  push_layer(new EditorLayer());
}

void EditorApp::on_destroy() {
  Project::save_project();
  save_json();

  AppSettings::save_current_window_state();

  Renderer2D::shut_down();
  NodeFactory::shut_down();
}

void EditorApp::save_json() {
  json j;
  {
    Helper::makefile_read_only(AppSettings::get_settings_path(), false);
    std::ifstream in_file(AppSettings::get_settings_path());
    if (in_file.is_open())
      in_file >> j;

    in_file.close();

    Helper::save_json(j, "Mode", (int)AppSettings::Mode::ProjectManager, -1);

    std::ofstream out_file(AppSettings::get_settings_path());
    out_file << j.dump(4);
    out_file.close();
    Helper::makefile_read_only(AppSettings::get_settings_path());
  }
  {
    auto project = Project::get_active_project();

    auto project_file_path = project->get_project_file_directory();
    Helper::makefile_read_only(project_file_path, false);
    std::ifstream in_file(project_file_path);
    if (!in_file.is_open()) {
      AERO_CORE_INFO("Cannot Open File: {0}", project_file_path);
    }
    in_file >> j;
    in_file.close();

    auto active_scene = Scene::get_active_scene();

    Helper::save_json(j["Scene"], "Last Loaded", active_scene->get_name(), std::string(""));
    Helper::save_json(j["Scene"],
                      "Last Loaded Path",
                      active_scene->get_directory(),
                      std::string(""));

    std::ofstream out_file(project_file_path);
    if (!out_file.is_open()) {
      AERO_CORE_INFO("Cannot Open File: {0}", project_file_path);
    }
    out_file << j.dump(4);
    out_file.close();
    Helper::makefile_read_only(project_file_path);
  }
}

void EditorApp::load_project_data() {
  json        j;
  std::string project_path = m_project_path;

  // The command line is authoritative. The settings fallback is kept only for
  // direct EditorApp construction (for example, internal tooling).
  if (project_path.empty())
  {
    Helper::makefile_read_only(AppSettings::get_settings_path(), false);
    std::ifstream in_file(AppSettings::get_settings_path());

    if (!in_file.is_open()) {
      AERO_CORE_INFO("Cannot Open File!");
      return;
    }
    in_file >> j;
    in_file.close();
    Helper::makefile_read_only(AppSettings::get_settings_path());

    Helper::load_json(j["Project"], "Directory", project_path, std::string(""));
  }

  auto project = Project::load_project(project_path);
  if (!project || !project->m_project_loaded) {
    Helper::makefile_read_only(AppSettings::get_settings_path(), false);
    std::ifstream in_file(AppSettings::get_settings_path());
    if (in_file.is_open())
      in_file >> j;

    in_file.close();
    Helper::save_json(j,
                      "Mode",
                      (int)AppSettings::Mode::ProjectManager,
                      (int)AppSettings::Mode::ProjectManager);
    std::ofstream out_file(AppSettings::get_settings_path());
    out_file << j.dump(4);
    out_file.close();
    Helper::makefile_read_only(AppSettings::get_settings_path());
    AppSettings::reload_app();
    return;
  }

  m_project_path = project->get_project_file_directory();

  Helper::makefile_read_only(project->get_project_file_directory(), false);
  std::ifstream in_proj_file(project->get_project_file_directory());
  if (!in_proj_file.is_open()) {
    AERO_CORE_INFO("Cannot Open File");
    return;
  }
  in_proj_file >> j;
  in_proj_file.close();
  Helper::makefile_read_only(project->get_project_file_directory());

  // Window Data Loaded and Created
  {
    WindowProps props;
    Helper::load_json(j["Window"], "Title", props.Title, std::string("AeroEngine"));

    const WindowState window_state = AppSettings::get_window_state();
    if (window_state.has_size)
      props.Size = {static_cast<AG_uint>(window_state.width),
                    static_cast<AG_uint>(window_state.height)};

    init(props);
    AppSettings::restore_window_state();
  }

  {
    std::string last_scene_path;

    Helper::load_json(j["Scene"], "Last Loaded Path", last_scene_path, std::string(""));

    std::string path = project->get_directory() + project->get_scene_directory() + last_scene_path;

    auto scene = SaveScene::load_scene(path);
    Scene::set_active_scene(scene);
  }
}
}  // namespace ag
