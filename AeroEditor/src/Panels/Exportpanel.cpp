#include <imgui.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <Application/EditorLayer.hpp>
#include <Panels/Exportpanel.hpp>
#include <Project/FileDialogs.hpp>
#include <Project/Project.hpp>
#include <UI/PopUp.hpp>
#include <UI/UI.hpp>

namespace ag
{
namespace
{
template <size_t N>
void copy_to_buffer(char (&buffer)[N], const std::string& value)
{
  std::strncpy(buffer, value.c_str(), N - 1);
  buffer[N - 1] = '\0';
}

bool ends_with_lower(const std::string& value, const std::string& suffix)
{
  if (suffix.empty() || value.size() < suffix.size())
    return false;

  std::string tail = value.substr(value.size() - suffix.size());
  std::string lower_suffix = suffix;
  std::transform(tail.begin(), tail.end(), tail.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  std::transform(lower_suffix.begin(), lower_suffix.end(), lower_suffix.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return tail == lower_suffix;
}

std::string ensure_extension(std::string value, const std::string& extension)
{
  if (value.empty())
    value = (extension == ".pak") ? "game" : "Sandbox";
  if (extension.empty())
    return value;

  if (ends_with_lower(value, extension))
  {
    // Keep the canonical extension case; the runtime looks up files by exact
    // name on case-sensitive platforms.
    const size_t extension_start = value.size() - extension.size();
    if (value.compare(extension_start, extension.size(), extension) != 0)
      value.replace(extension_start, extension.size(), extension);
    return value;
  }
  return value + extension;
}

std::string strip_extension(std::string value, const std::string& extension)
{
  if (ends_with_lower(value, extension))
    value.erase(value.size() - extension.size());
  return value;
}

std::string pak_name_from_buffer(const char* buffer)
{
  return ensure_extension(buffer ? buffer : "", ".pak");
}

std::string exe_name_from_buffer(const char* buffer)
{
#ifdef PLATFORM_WINDOWS
  return ensure_extension(buffer ? buffer : "", ".exe");
#else
  return ensure_extension(buffer ? buffer : "", "");
#endif
}
}  // namespace

ExportPanel::ExportPanel()
{
  auto project = Project::get_active_project();
  std::string initial = project ? project->get_directory() : std::string(".");
  if (initial.empty())
    initial = ".";

  copy_to_buffer(m_output_folder, initial);
  m_exporter.set_output_folder(initial);

  copy_to_buffer(m_android_dir, AndroidBuildManager::detect_android_dir());
}

void ExportPanel::open()
{
  m_open               = true;
  m_active_tab         = 0;
  m_select_android_tab = false;
  m_exporter.refresh_project_info();

  auto project = Project::get_active_project();
  if (project)
  {
    if (m_exporter.get_output_folder().empty())
      m_exporter.set_output_folder(project->get_directory());
    if (m_exporter.get_project_name().empty())
      m_exporter.set_project_name(project->get_name());
    if (m_exporter.get_project_dir().empty())
      m_exporter.set_project_dir(project->get_directory());
  }

  copy_to_buffer(m_output_folder, m_exporter.get_output_folder());
  copy_to_buffer(m_pak_name, strip_extension(m_exporter.get_pak_name(), ".pak"));
  copy_to_buffer(m_exe_name, strip_extension(m_exporter.get_exe_name(), ".exe"));

  if (m_android_dir[0] == '\0')
    copy_to_buffer(m_android_dir, AndroidBuildManager::detect_android_dir());
}

void ExportPanel::open_android()
{
  open();
  m_active_tab         = 1;
  m_select_android_tab = true;
  m_android_builder.clear_status();
}

void ExportPanel::close()
{
  m_open = false;
  // Keep status/log state alive while an export is still running so reopening
  // the panel can show accurate progress. Clear completed state on next open.
  if (!m_exporter.is_busy())
    m_exporter.clear_status();
  m_android_builder.clear_status();
}

void ExportPanel::render()
{
  if (!m_open)
    return;

  PopUpModel model;
  model.id          = "##BuildAndExport";
  model.name        = "Build & Export";
  model.window_size = {620, 800};

  model.draw_content = [this]()
  {
    this->draw_content();
  };

  model.on_close = [this]()
  {
    this->close();
    ImGui::CloseCurrentPopup();
  };

  PopUp::draw(model);
}

void ExportPanel::draw_content()
{
  m_android_builder.update();

  if (!ImGui::BeginTabBar("##BuildAndExportTabs"))
    return;

  if (ImGui::BeginTabItem("Desktop"))
  {
    m_active_tab = 0;
    draw_desktop_content();
    ImGui::EndTabItem();
  }

  const ImGuiTabItemFlags android_flags =
      m_select_android_tab ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None;
  if (ImGui::BeginTabItem("Android", nullptr, android_flags))
  {
    m_active_tab         = 1;
    m_select_android_tab = false;
    draw_android_content();
    ImGui::EndTabItem();
  }

  ImGui::EndTabBar();
}

void ExportPanel::draw_desktop_content()
{
  auto project = Project::get_active_project();
  const std::string project_name = project ? project->get_name() : std::string();
  const bool has_project = !project_name.empty();
  const bool busy = m_exporter.is_busy();
  const auto status = m_exporter.get_status();
  const float progress = m_exporter.get_progress();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
  ImGui::BeginChild("##ExportChild", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
  {
    ImGui::TextUnformatted("Project");
    if (has_project)
      ImGui::Text("  %s", project_name.c_str());
    else
      ImGui::TextColored(ImVec4(0.90f, 0.30f, 0.30f, 1.0f),
                         "  No project loaded. Open or create a project first.");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::BeginDisabled(busy || !has_project);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));

      ImGui::TextUnformatted("Output Folder");
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 52.0f);
      ImGui::InputText("##OutputFolder", m_output_folder, sizeof(m_output_folder));

      ImGui::SameLine();
      GUI_Button browse;
      browse.label   = "...";
      browse.size    = {40, 32};
      browse.enabled = !busy && has_project;
      if (UI::draw_button(browse))
      {
        std::string selected = FileDialogs::select_folder("Select Output Folder");
        if (!selected.empty())
        {
          copy_to_buffer(m_output_folder, selected);
          m_exporter.set_output_folder(selected);
        }
      }

      ImGui::PopStyleVar();

      ImGui::Dummy(ImVec2(0, 8));
      ImGui::TextUnformatted("Game Archive Name");
      ImGui::SetNextItemWidth(-1);
      if (ImGui::InputText("##PakName", m_pak_name, sizeof(m_pak_name)))
        m_exporter.set_pak_name(pak_name_from_buffer(m_pak_name));
      ImGui::TextDisabled("  Saved as %s (runtime expects game.pak)",
                          pak_name_from_buffer(m_pak_name).c_str());

      ImGui::Dummy(ImVec2(0, 8));
      ImGui::TextUnformatted("Runtime Executable Name");
      ImGui::SetNextItemWidth(-1);
      if (ImGui::InputText("##ExeName", m_exe_name, sizeof(m_exe_name)))
        m_exporter.set_exe_name(exe_name_from_buffer(m_exe_name));
      ImGui::TextDisabled("  Looks for %s", exe_name_from_buffer(m_exe_name).c_str());
    }
    ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.26f, 0.59f, 0.98f, 1.0f), "What gets exported");
    const std::string exe_name = exe_name_from_buffer(m_exe_name);
    ImGui::BulletText("%s: project scenes, assets, scripts and settings.json.",
                      pak_name_from_buffer(m_pak_name).c_str());
    ImGui::BulletText("%s: built runtime copied from the Sandbox target.", exe_name.c_str());
    ImGui::BulletText("Engine data paks next to the runtime (for example assets.pak).");
    ImGui::TextColored(ImVec4(0.55f, 0.55f, 0.55f, 1.0f),
                       "Build the Sandbox/runtime target before exporting, then ship the whole folder.");

    ImGui::Spacing();

    const float available_width = ImGui::GetContentRegionAvail().x;
    GUI_Button export_button;
    export_button.label   = busy ? "Exporting..." : "Export Game";
    export_button.size    = {available_width, 36.0f};
    export_button.enabled = !busy && has_project;
    if (UI::draw_button(export_button))
    {
      std::string output = m_output_folder;
      if (output.empty() && project)
        output = project->get_directory();

      m_exporter.set_output_folder(output);
      m_exporter.set_pak_name(pak_name_from_buffer(m_pak_name));
      m_exporter.set_exe_name(exe_name_from_buffer(m_exe_name));
      copy_to_buffer(m_output_folder, output);
      m_exporter.start_export();
    }

    if (busy || status == GameExporter::Status::Done || status == GameExporter::Status::Failed ||
        progress > 0.0f)
    {
      ImGui::Spacing();

      ImVec2 bar_size = ImVec2(ImGui::GetContentRegionAvail().x, 26.0f);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.17f, 0.21f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
                            status == GameExporter::Status::Failed
                                ? ImVec4(0.90f, 0.30f, 0.30f, 1.0f)
                                : ImVec4(0.26f, 0.59f, 0.98f, 1.0f));
      ImGui::ProgressBar(progress, bar_size, "");
      ImGui::PopStyleColor(2);

      char overlay[32];
      std::snprintf(overlay, sizeof(overlay), "%.0f%%", progress * 100.0f);
      ImVec2 text_size = ImGui::CalcTextSize(overlay);
      ImVec2 pos       = ImGui::GetItemRectMin();
      pos.x += (bar_size.x - text_size.x) * 0.5f;
      pos.y += (bar_size.y - text_size.y) * 0.5f;
      ImGui::GetWindowDrawList()->AddText(pos, IM_COL32(255, 255, 255, 255), overlay);
    }

    const std::string status_message = m_exporter.get_status_message();
    if (status == GameExporter::Status::Done)
    {
      ImGui::Spacing();
      ImGui::TextColored(ImVec4(0.30f, 0.85f, 0.40f, 1.0f), "Export complete!");
      if (!status_message.empty())
        ImGui::TextWrapped("%s", status_message.c_str());
    }
    else if (status == GameExporter::Status::Failed)
    {
      ImGui::Spacing();
      const std::string error_message = m_exporter.get_error_message();
      ImGui::TextColored(ImVec4(0.90f, 0.30f, 0.30f, 1.0f), "%s", error_message.c_str());
      if (!status_message.empty())
        ImGui::TextWrapped("%s", status_message.c_str());
    }
    else if (busy)
    {
      ImGui::Spacing();
      ImGui::Text("%s", status_message.empty() ? "Working..." : status_message.c_str());
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("How to ship the exported game",
                                ImGuiTreeNodeFlags_DefaultOpen))
    {
      ImGui::TextWrapped("1. Build the Sandbox/runtime target for your platform first.");
      ImGui::TextWrapped("2. Export into a clean folder (or clear the folder before shipping).");
      ImGui::TextWrapped("3. Keep %s, %s and every engine data pak together in that folder.",
                         exe_name.c_str(), pak_name_from_buffer(m_pak_name).c_str());
      ImGui::TextWrapped("4. On Linux, make the runtime executable with 'chmod +x %s'.",
                         exe_name.c_str());
      ImGui::TextWrapped("5. Include any platform runtime libraries the executable needs, then zip the folder.");
      ImGui::TextColored(ImVec4(0.55f, 0.55f, 0.55f, 1.0f),
                         "Run the executable from the exported folder; paths are relative to it.");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.50f, 1.0f), "Export Log");
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.09f, 0.11f, 1.0f));
    ImGui::BeginChild("##ExportLog", ImVec2(0, 0), true);
    {
      auto log = m_exporter.get_log();
      for (const auto& line : log)
      {
        if (line.find("ERROR") != std::string::npos)
          ImGui::TextColored(ImVec4(0.90f, 0.30f, 0.30f, 1.0f), "%s", line.c_str());
        else if (line.find("WARNING") != std::string::npos)
          ImGui::TextColored(ImVec4(0.90f, 0.75f, 0.20f, 1.0f), "%s", line.c_str());
        else if (line.find("===") != std::string::npos)
          ImGui::TextColored(ImVec4(0.26f, 0.59f, 0.98f, 1.0f), "%s", line.c_str());
        else if (line.find("  +") != std::string::npos || line.find("  ->") != std::string::npos)
          ImGui::TextColored(ImVec4(0.60f, 0.60f, 0.60f, 1.0f), "%s", line.c_str());
        else
          ImGui::TextUnformatted(line.c_str());
      }

      if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 2.0f)
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
  }
  ImGui::EndChild();
  ImGui::PopStyleVar();
}

void ExportPanel::draw_android_content()
{
  auto project = Project::get_active_project();
  const bool has_project = project && !project->get_name().empty();
  const bool android_valid = AndroidBuildManager::has_android_project(m_android_dir);
  const auto status = m_android_builder.get_status();
  const float progress = m_android_builder.get_progress();
  const bool busy = m_android_builder.is_busy();

  const bool has_sdk = std::getenv("ANDROID_HOME") != nullptr ||
                       std::getenv("ANDROID_SDK_ROOT") != nullptr;
  const bool has_jdk = std::getenv("JAVA_HOME") != nullptr;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
  ImGui::BeginChild("##AndroidBuildChild", ImVec2(0, 0), false,
                    ImGuiWindowFlags_AlwaysUseWindowPadding);
  {
    ImGui::TextUnformatted("Project");
    if (has_project)
      ImGui::Text("  %s", project->get_name().c_str());
    else
      ImGui::TextColored(ImVec4(0.90f, 0.30f, 0.30f, 1.0f),
                         "  No project loaded. Open or create a project first.");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::BeginDisabled(busy || !has_project);
    {
      ImGui::TextUnformatted("Android Project Folder");
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 52.0f);
      ImGui::InputText("##AndroidDir", m_android_dir, sizeof(m_android_dir));
      ImGui::SameLine();
      GUI_Button browse;
      browse.label   = "...";
      browse.size    = {40, 32};
      browse.enabled = !busy && has_project;
      if (UI::draw_button(browse))
      {
        std::string selected = FileDialogs::select_folder("Select Android Project Folder");
        if (!selected.empty())
          copy_to_buffer(m_android_dir, selected);
      }

      if (android_valid)
        ImGui::TextColored(ImVec4(0.30f, 0.85f, 0.40f, 1.0f),
                           "  Gradle wrapper and app/build.gradle found.");
      else
        ImGui::TextColored(ImVec4(0.90f, 0.30f, 0.30f, 1.0f),
                           "  Missing gradlew / gradlew.bat or app/build.gradle.");

      ImGui::Dummy(ImVec2(0, 8));
      ImGui::TextUnformatted("Build Type");
      ImGui::RadioButton("Debug APK", &m_android_type, 0);
      ImGui::SameLine();
      ImGui::RadioButton("Release APK", &m_android_type, 1);

      ImGui::Dummy(ImVec2(0, 6));
      ImGui::Checkbox("Offline mode (use cached Gradle dependencies)", &m_android_offline);
      ImGui::Checkbox("Clean before building", &m_android_clean);
    }
    ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.26f, 0.59f, 0.98f, 1.0f), "Prerequisites");
    ImGui::TextColored(has_sdk ? ImVec4(0.30f, 0.85f, 0.40f, 1.0f)
                               : ImVec4(0.90f, 0.75f, 0.20f, 1.0f),
                       "  %s ANDROID_HOME or ANDROID_SDK_ROOT",
                       has_sdk ? "Found" : "Not set in the editor environment");
    ImGui::TextColored(has_jdk ? ImVec4(0.30f, 0.85f, 0.40f, 1.0f)
                               : ImVec4(0.90f, 0.75f, 0.20f, 1.0f),
                       "  %s JAVA_HOME",
                       has_jdk ? "Found" : "Not set in the editor environment");
    ImGui::TextDisabled("  Android SDK Platform 34 and NDK 25.2.9519653 are required.");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("What the Android build does", ImGuiTreeNodeFlags_DefaultOpen))
    {
      ImGui::BulletText("Packages the active project into game.pak (scenes, assets, scripts, settings).");
      ImGui::BulletText("Stages that pak in the Gradle build folder; source Android assets are not changed.");
      ImGui::BulletText("Runs the Gradle wrapper to assemble the selected APK variant.");
      if (m_android_type == 1)
        ImGui::TextColored(ImVec4(0.90f, 0.75f, 0.20f, 1.0f),
                           "  Release builds are currently unsigned (app-release-unsigned.apk).");
    }

    ImGui::Spacing();

    const float available_width = ImGui::GetContentRegionAvail().x;
    GUI_Button build_button;
    const bool release = m_android_type == 1;
    build_button.label = busy ? "Building Android..."
                              : (release ? "Build Release APK" : "Build Debug APK");
    build_button.size    = {available_width, 36.0f};
    build_button.enabled = !busy && has_project && android_valid;
    if (UI::draw_button(build_button))
    {
      AndroidBuildManager::Config config;
      config.android_dir = m_android_dir;
      config.build_type  = release ? AndroidBuildManager::BuildType::Release
                                   : AndroidBuildManager::BuildType::Debug;
      config.offline = m_android_offline;
      config.clean   = m_android_clean;

      // Package saved data. If scenes are dirty, reuse the editor's standard
      // Save / Continue prompt instead of silently building a stale game.pak.
      EditorLayer::get().request_scene_action("build an Android APK",
                                              [this, config]() { m_android_builder.start(config); });
    }

    if (busy || status == AndroidBuildManager::Status::Done ||
        status == AndroidBuildManager::Status::Failed || progress > 0.0f)
    {
      ImGui::Spacing();

      ImVec2 bar_size = ImVec2(ImGui::GetContentRegionAvail().x, 26.0f);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.17f, 0.21f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
                            status == AndroidBuildManager::Status::Failed
                                ? ImVec4(0.90f, 0.30f, 0.30f, 1.0f)
                                : ImVec4(0.26f, 0.59f, 0.98f, 1.0f));
      ImGui::ProgressBar(progress, bar_size, "");
      ImGui::PopStyleColor(2);

      char overlay[32];
      std::snprintf(overlay, sizeof(overlay), "%.0f%%", progress * 100.0f);
      ImVec2 text_size = ImGui::CalcTextSize(overlay);
      ImVec2 pos       = ImGui::GetItemRectMin();
      pos.x += (bar_size.x - text_size.x) * 0.5f;
      pos.y += (bar_size.y - text_size.y) * 0.5f;
      ImGui::GetWindowDrawList()->AddText(pos, IM_COL32(255, 255, 255, 255), overlay);
    }

    const std::string status_message = m_android_builder.get_status_message();
    if (status == AndroidBuildManager::Status::Done)
    {
      ImGui::Spacing();
      ImGui::TextColored(ImVec4(0.30f, 0.85f, 0.40f, 1.0f), "Android build complete!");
      if (!status_message.empty())
        ImGui::TextWrapped("%s", status_message.c_str());

      const std::string apk = m_android_builder.get_apk_path();
      ImGui::TextWrapped("APK: %s", apk.c_str());
      if (!apk.empty() && ImGui::Button("Copy APK path", ImVec2(130, 26)))
        ImGui::SetClipboardText(apk.c_str());
    }
    else if (status == AndroidBuildManager::Status::Failed)
    {
      ImGui::Spacing();
      ImGui::TextColored(ImVec4(0.90f, 0.30f, 0.30f, 1.0f), "%s",
                         m_android_builder.get_error_message().c_str());
      if (!status_message.empty())
        ImGui::TextWrapped("%s", status_message.c_str());
    }
    else if (busy)
    {
      ImGui::Spacing();
      ImGui::Text("%s", status_message.empty() ? "Working..." : status_message.c_str());
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.50f, 1.0f), "Android Build Log");
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.09f, 0.11f, 1.0f));
    ImGui::BeginChild("##AndroidBuildLog", ImVec2(0, 0), true);
    {
      for (const auto& line : m_android_builder.get_log())
      {
        if (line.find("ERROR") != std::string::npos || line.find("FAILED") != std::string::npos)
          ImGui::TextColored(ImVec4(0.90f, 0.30f, 0.30f, 1.0f), "%s", line.c_str());
        else if (line.find("WARNING") != std::string::npos)
          ImGui::TextColored(ImVec4(0.90f, 0.75f, 0.20f, 1.0f), "%s", line.c_str());
        else if (line.find("===") != std::string::npos ||
                 line.find("BUILD SUCCESSFUL") != std::string::npos)
          ImGui::TextColored(ImVec4(0.26f, 0.59f, 0.98f, 1.0f), "%s", line.c_str());
        else if (line.find("  +") != std::string::npos || line.find("  ->") != std::string::npos ||
                 line.find("  .pak") != std::string::npos)
          ImGui::TextColored(ImVec4(0.60f, 0.60f, 0.60f, 1.0f), "%s", line.c_str());
        else
          ImGui::TextUnformatted(line.c_str());
      }

      if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 2.0f)
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
  }
  ImGui::EndChild();
  ImGui::PopStyleVar();
}
}  // namespace ag
