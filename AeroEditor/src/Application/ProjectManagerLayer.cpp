#include "Core/Log.hpp"

#include <imgui.h>

#include <Application/AppSettings.hpp>
#include <Application/ProjectManagerLayer.hpp>
#include <fstream>

namespace ag
{
ProjectManagerLayer::ProjectManagerLayer()
  : Layer("ProjectManagerLayer")
{
}

void ProjectManagerLayer::on_attach()
{
  load_projects();
}

void ProjectManagerLayer::on_detach()
{
}

void ProjectManagerLayer::on_update(ag::TimeStamp ts)
{
}

void ProjectManagerLayer::load_projects()
{
  m_projects.clear();
  m_projects_file_path = AppSettings::get_recent_projects_path();

  if (!std::filesystem::exists(m_projects_file_path))
  {
    AERO_CORE_INFO("No recent projects file found, starting with empty list");
    return;
  }

  Helper::makefile_read_only(m_projects_file_path, false);
  std::ifstream file(m_projects_file_path);

  if (!file.is_open())
  {
    AERO_CORE_ERROR("Failed to open recent projects file: {0}", m_projects_file_path);
    return;
  }
  try
  {
    json j;
    file >> j;
    if (j.contains("Projects"))
    {
      for (const auto& proj : j["Projects"])
      {
        ProjectEntry entry;
        Helper::load_json(proj, "Name", entry.name, std::string(""));
        Helper::load_json(proj, "File Path", entry.path, std::string(""));
        std::string timestamp_str;
        Helper::load_json(proj, "Last Modified", timestamp_str, std::string(""));

        std::tm            tm = {};
        std::istringstream ss(timestamp_str);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        entry.timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        entry.last_modified = format_date_time(entry.timestamp);

        m_projects.push_back(entry);
      }
    }
    file.close();
  }
  catch (const std::exception& e)
  {
    AERO_CORE_ERROR("Error Loading the Projects:{0}", e.what());
    m_projects.clear();
  }
}

void ProjectManagerLayer::save_projects()
{
  if (m_projects_file_path.empty())
  {
    AERO_CORE_ERROR("Cannot save projects: file path is empty");
    return;
  }

  try
  {
    std::filesystem::path file_path(m_projects_file_path);
    std::filesystem::path parent_dir = file_path.parent_path();

    if (!parent_dir.empty() && !std::filesystem::exists(parent_dir))
    {
      AERO_CORE_INFO("Creating directory: {0}", parent_dir.string());
      std::filesystem::create_directories(parent_dir);
    }
  }
  catch (const std::exception& e)
  {
    AERO_CORE_ERROR("Failed to create directory: {0}", e.what());
    return;
  }

  json j;
  j["Projects"] = json::array();

  for (const auto& proj : m_projects)
  {
    std::time_t        time = std::chrono::system_clock::to_time_t(proj.timestamp);
    std::tm            tm   = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");

    nlohmann::json proj_json;
    proj_json["Name"]          = proj.name;
    proj_json["File Path"]     = proj.path;
    proj_json["Last Modified"] = oss.str();

    AERO_CORE_INFO("Project Name:{0}, Project FilePath: {1}", proj.name, proj.path);

    j["Projects"].push_back(proj_json);
  }

  std::ofstream file(m_projects_file_path);

  if (!file.is_open())
  {
    AERO_CORE_ERROR("Failed to open file for writing: {0}", m_projects_file_path);
    AERO_CORE_ERROR("Error: {0}", strerror(errno));
    return;
  }

  try
  {
    file << j.dump(4);
    file.close();
    Helper::makefile_read_only(m_projects_file_path);
    AERO_CORE_INFO("Successfully saved {0} projects to: {1}",
                   m_projects.size(),
                   m_projects_file_path);
  }
  catch (const std::exception& e)
  {
    AERO_CORE_ERROR("Failed to write to file: {0}", e.what());
    file.close();
    Helper::makefile_read_only(m_projects_file_path);
  }
}

void ProjectManagerLayer::add_recent_project(const std::string name, std::string path)
{
  m_projects.erase(std::remove_if(m_projects.begin(),
                                  m_projects.end(),
                                  [&path](const ProjectEntry& p) { return p.path == path; }),
                   m_projects.end());

  ProjectEntry entry;
  entry.name          = name;
  entry.path          = path;
  entry.timestamp     = std::chrono::system_clock::now();
  entry.last_modified = format_date_time(entry.timestamp);

  m_projects.insert(m_projects.begin(), entry);

  if (m_projects.size() > 10)
    m_projects.resize(10);

  save_projects();
}

std::string ProjectManagerLayer::format_date_time(const std::chrono::system_clock::time_point& time)
{
  auto now      = std::chrono::system_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - time);

  auto seconds = duration.count();
  auto minutes = seconds / 60;
  auto hours   = minutes / 60;
  auto days    = hours / 24;
  auto weeks   = days / 7;

  if (seconds < 60)
    return "Just now";
  else if (minutes < 60)
    return std::to_string(minutes) + (minutes == 1 ? " minute ago" : " minutes ago");
  else if (hours < 24)
    return std::to_string(hours) + (hours == 1 ? " hour ago" : " hours ago");
  else if (days == 1)
    return "Yesterday";
  else if (days < 7)
    return std::to_string(days) + " days ago";
  else if (weeks < 4)
    return std::to_string(weeks) + (weeks == 1 ? " week ago" : " weeks ago");
  else
    return "Over a month ago";
}

void ProjectManagerLayer::on_imgui_render()
{
  ag::vec2u window_size = ag::Application::get().get_window().get_size();
  ag::vec2f window_pos  = ag::Application::get().get_window().get_position();

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                  ImGuiWindowFlags_NoBringToFrontOnFocus |
                                  ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollbar |
                                  ImGuiWindowFlags_NoScrollWithMouse;

  ImGui::SetNextWindowPos({window_pos.x, window_pos.y});
  ImGui::SetNextWindowSize({(float)window_size.x, (float)window_size.y});

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));

  ImGui::Begin("MainWindow", nullptr, window_flags);
  ImGui::Spacing();
  {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::PushFont(io.Fonts->Fonts[1]);
    const char* text       = "Welcome to AERO Engine";
    float       text_width = ImGui::CalcTextSize(text).x;
    ImGui::SetCursorPosX((window_size.x - text_width) / 2.0f);

    ImGui::Text("%s", text);
    ImGui::PopFont();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  float total_width   = 80 + 5 + 80 + 5 + 420;
  float window_center = window_size.x / 2.0f;
  ImGui::SetCursorPosX(window_center - total_width / 2.0f);

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));

  if (ImGui::Button("Create", ImVec2(80, 35)))
  {
    create_new_project();
  }
  ImGui::SameLine(0, 5);
  if (ImGui::Button("Import", ImVec2(80, 35)))
  {
    open_existing_project();
  }

  ImGui::SameLine(0, 5);
  static char search_buffer[128] = "";

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
  ImGui::SetNextItemWidth(420);
  ImGui::InputTextWithHint("##SearchProjects",
                           "Search Projects...",
                           search_buffer,
                           IM_ARRAYSIZE(search_buffer));

  ImGui::PopStyleVar();

  ImGui::PopStyleVar(2);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  {
    ImGui::SetCursorPosX(window_center - total_width / 2.0f);
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::Text("Recent Projects");
    ImGui::PopFont();

    ImGui::SetCursorPosX(window_center - total_width / 2.0f);
    ImGui::Text("Select a project to open");
  }
  ImGui::Spacing();
  ImGui::Spacing();

  ImVec2 box_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
  ImGui::BeginChild("RecentProjectsBox",
                    box_size,
                    false,
                    ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);

  float item_width  = ImGui::GetContentRegionAvail().x - 5.0f;
  float item_height = 70.0f;

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15.0f, 15.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));

  for (int i = 0; i < m_projects.size(); i++)
  {
    ImGui::PushID(i);

    ImVec2 item_size(item_width, item_height);

    bool selected = false;
    if (ImGui::Selectable("##ProjectItem",
                          &selected,
                          ImGuiSelectableFlags_AllowDoubleClick,
                          item_size))
    {
      if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
      {
        AERO_CORE_INFO("Opening Project: {0}", m_projects[i].path);
        open_project(m_projects[i].path);
      }
    }

    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImU32 border_color     = ImGui::GetColorU32(ImVec4(0.35f, 0.35f, 0.40f, 1.0f));
    float border_thickness = 1.0f;

    draw_list->AddRect(min, max, border_color, 0.0f, 0, border_thickness);

    if (ImGui::BeginPopupContextItem())
    {
      if (ImGui::MenuItem("Open"))
      {
        open_project(m_projects[i].path);
      }
      if (ImGui::MenuItem("Remove from list"))
      {
      }
      if (ImGui::MenuItem("Show in Explorer"))
      {
        if (m_projects[i].path.empty())
          return;

        std::filesystem::path abs_path = std::filesystem::absolute(m_projects[i].path);

#ifdef PLATFORM_WINDOWS
        std::string command = "explorer /select,\"" + abs_path.string() + "\"";
        system(command.c_str());
#elif defined(PLATFORM_LINUX)
        std::string parent  = abs_path.parent_path().string();
        std::string command = "dolphin \"" + parent + "\" &";
        system(command.c_str());
#endif
      }

      ImGui::EndPopup();
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - item_width + 5.0f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

    ImGui::BeginGroup();

    ImGui::SetWindowFontScale(1.2f);
    ImGui::Text("%s", m_projects[i].name.c_str());
    ImGui::SetWindowFontScale(1.0f);

    ImGui::Text("%s", m_projects[i].path.c_str());

    std::string text = "Last opened: ";
    text += m_projects[i].last_modified;
    ImGui::SameLine();
    ImGui::SetWindowFontScale(0.9f);
    ImVec2 size  = ImGui::CalcTextSize(text.c_str());
    float  width = size.x;

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - width - 10.0f);
    ImGui::Text("Last opened: %s", m_projects[i].last_modified.c_str());
    ImGui::SetWindowFontScale(1.f);
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::PopID();
  }

  ImGui::PopStyleVar(3);

  if (m_projects.size() == 0)
  {
    float empty_text_width = ImGui::CalcTextSize("No recent projects").x;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - empty_text_width) * 0.5f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50.0f);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.6f, 1.0f));
    ImGui::Text("No recent projects");
    ImGui::PopStyleColor();

    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200.0f) * 0.5f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

    if (ImGui::Button("Create Your First Project", ImVec2(200.0f, 40.0f)))
    {
      create_new_project();
    }
  }

  ImGui::EndChild();

  ImGui::End();
  ImGui::PopStyleVar();
}

void ProjectManagerLayer::on_event(ag::Event& e)
{
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>(AERO_BIND_EVENT_FN(ProjectManagerLayer::on_window_close));
}

bool ProjectManagerLayer::on_window_close(WindowCloseEvent& e)
{
  Application::get().m_running = false;

  return true;
}

void ProjectManagerLayer::create_new_project()
{
  std::string newPath = ag::FileDialogs::select_folder("Select Folder for a new project");
  if (newPath.empty())
    return;

  auto project = ag::Project::new_project(newPath);
  json j;
  Helper::makefile_read_only(AppSettings::get_settings_path(), false);
  std::ifstream in_file(AppSettings::get_settings_path());
  if (in_file.is_open())
    in_file >> j;

  in_file.close();

  Helper::save_json(j["Project"], "Name", project->get_name(), std::string(""));
  Helper::save_json(j["Project"], "Directory", project->get_directory(), std::string(""));
  Helper::save_json(j["Project"],
                    "File Path",
                    project->get_project_file_directory(),
                    std::string(""));
  Helper::save_json(j,
                    "Mode",
                    (int)AppSettings::Mode::Editor,
                    (int)AppSettings::Mode::ProjectManager);

  AERO_CORE_INFO("Project File:{0}", project->get_project_file_directory());
  std::ofstream out_file(AppSettings::get_settings_path());
  out_file << j.dump(4);
  out_file.close();
  Helper::makefile_read_only(AppSettings::get_settings_path());
  add_recent_project(project->get_name(), project->get_directory());
  AppSettings::reload_app();
}
void ProjectManagerLayer::open_existing_project()
{
  std::string path = ag::FileDialogs::select_folder("Select Your Project Folder");
  if (!path.empty())
  {
    json j;
    auto project = ag::Project::load_project(path);
    if (!project->m_project_loaded)
    {
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

    Helper::makefile_read_only(AppSettings::get_settings_path(), false);
    std::ifstream in_file(AppSettings::get_settings_path());
    if (in_file.is_open())
      in_file >> j;

    in_file.close();

    Helper::save_json(j["Project"], "Name", project->get_name(), std::string(""));
    Helper::save_json(j["Project"], "Directory", project->get_directory(), std::string(""));
    Helper::save_json(j["Project"],
                      "File Path",
                      project->get_project_file_directory(),
                      std::string(""));
    Helper::save_json(j,
                      "Mode",
                      (int)AppSettings::Mode::Editor,
                      (int)AppSettings::Mode::ProjectManager);

    std::ofstream out_file(AppSettings::get_settings_path());
    out_file << j.dump(4);
    out_file.close();

    Helper::makefile_read_only(AppSettings::get_settings_path());
    add_recent_project(project->get_name(), project->get_directory());
    AppSettings::reload_app();
  }
}

void ProjectManagerLayer::open_project(const std::string path)
{
  AERO_CORE_INFO("Requested Load Path:{0}", path);
  std::string name = std::filesystem::path(path).filename().string();
  add_recent_project(name, path);
  json j;
  AERO_CORE_INFO("Actual Loaded Prpject Path:{0}", path);
  auto project = ag::Project::load_project(path);
  if (!project->m_project_loaded)
  {
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

  Helper::makefile_read_only(AppSettings::get_settings_path(), false);
  std::ifstream in_file(AppSettings::get_settings_path());
  if (in_file.is_open())
    in_file >> j;

  in_file.close();

  Helper::save_json(j["Project"], "Name", project->get_name(), std::string(""));
  Helper::save_json(j["Project"], "Directory", project->get_directory(), std::string(""));
  Helper::save_json(j["Project"],
                    "File Path",
                    project->get_project_file_directory(),
                    std::string(""));
  Helper::save_json(j,
                    "Mode",
                    (int)AppSettings::Mode::Editor,
                    (int)AppSettings::Mode::ProjectManager);

  AERO_CORE_INFO("Opening Project:{0}, Project FilePath:{1}",
                 project->get_name(),
                 project->get_project_file_directory());

  std::ofstream out_file(AppSettings::get_settings_path());
  out_file << j.dump(4);
  out_file.close();

  Helper::makefile_read_only(AppSettings::get_settings_path());
  AppSettings::reload_app();
}
}  // namespace ag
