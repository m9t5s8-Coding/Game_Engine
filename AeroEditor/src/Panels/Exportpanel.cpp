#include <imgui.h>

#include <Panels/Exportpanel.hpp>
#include <Project/FileDialogs.hpp>
#include <Project/Project.hpp>
#include <UI/PopUp.hpp>
#include <UI/UI.hpp>

namespace ag
{
ExportPanel::ExportPanel()
{
  auto project = Project::get_active_project();

  std::string project_directory = project->get_directory();
  std::strncpy(m_output_folder, project_directory.c_str(), sizeof(m_output_folder) - 1);
  m_output_folder[sizeof(m_output_folder) - 1] = '\0';
}

void ExportPanel::open()
{
  m_open = true;
  m_exporter.refresh_project_info();

  auto project = Project::get_active_project();
  if (project)
  {
    m_exporter.set_project_name(project->get_name());
    m_exporter.set_project_dir(project->get_directory());
  }
}

void ExportPanel::close()
{
  m_open = false;
  m_exporter.clear_status();
}

void ExportPanel::render()
{
  if (!m_open)
    return;

  PopUpModel model;
  model.id          = "##ExportGame";
  model.name        = "Export Game";
  model.window_size = {450, 600};

  model.draw_content = [this]()
  {
    this->draw_content();
  };

  model.on_close = [this]()
  {
    this->close();
    ImGui::CloseCurrentPopup();
  };

  Create_Open_Popup::draw_popup(model);
}

void ExportPanel::draw_content()
{
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
  ImGui::BeginChild("##ExportChild", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
  {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));

    ImGui::Text("Output Folder");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 50);

    ImGui::InputText("##OutputFolder", m_output_folder, sizeof(m_output_folder));
    ImGui::PopStyleVar();
    ImGui::SameLine();

    GUI_Button button;

    {
      button.label   = "...";
      button.size    = {40, 32};
      button.enabled = true;
      if (UI::draw_button(button))
      {
        std::string selected = ag::FileDialogs::select_folder("Select Output Folder");
        if (!selected.empty())
        {
          m_temp_output_path = selected;
          strncpy(m_output_folder, selected.c_str(), sizeof(m_output_folder));
          m_exporter.set_output_folder(selected);
        }
      }
    }
    ImGui::Dummy(ImVec2(0, 10));
    ImGui::Text("Pak Name");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##PakName", m_pak_name, sizeof(m_pak_name)))
    {
      std::string pak_name = m_pak_name;
      pak_name += ".pak";
      m_exporter.set_pak_name(pak_name);
    }

    ImGui::Dummy(ImVec2(0, 10));

    ImGui::Text("Exe Name");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##ExeName", m_exe_name, sizeof(m_exe_name)))
    {
      std::string exe_name = m_exe_name;
#ifdef PLATFORM_WINDOWS
      exe_name += ".exe";
#endif
      m_exporter.set_pak_name(exe_name);
    }
    ImGui::Dummy(ImVec2(0, 10));

    ImGui::Separator();

    bool busy = m_exporter.is_busy();

    float available_width = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX());

    button.label   = busy ? "Exporting..." : "Export Game";
    button.size    = {available_width, 35.0f};
    button.enabled = !busy && !m_exporter.get_project_name().empty();

    if (UI::draw_button(button))
    {
      m_exporter.set_output_folder(m_output_folder);
      std::string name = m_pak_name;
      name += ".pak";
      m_exporter.set_pak_name(name);

      name = m_exe_name;
#ifdef PLATFORM_WINDOWS
      name += ".exe";
#endif
      m_exporter.set_exe_name(name);
      m_exporter.start_export();
    }

    // Progress bar
    if (busy || m_exporter.get_status() == GameExporter::Status::Done)
    {
      ImGui::Spacing();

      float  progress = m_exporter.get_progress();
      ImVec2 bar_size = ImVec2(ImGui::GetContentRegionAvail().x, 30.0);

      ImGui::SetCursorPosX(ImGui::GetCursorPosX());

      // Progress bar background
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.17f, 0.21f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.26f, 0.59f, 0.98f, 1.0f));

      ImGui::ProgressBar(progress, bar_size, "");

      char overlay[32];
      snprintf(overlay, sizeof(overlay), "%.0f%%", progress * 100.0f);

      ImVec2 text_size = ImGui::CalcTextSize(overlay);
      ImVec2 pos       = ImGui::GetItemRectMin();
      pos.x += (bar_size.x - text_size.x) * 0.5f;
      pos.y += (bar_size.y - text_size.y) * 0.5f;

      ImGui::GetWindowDrawList()->AddText(pos, IM_COL32(255, 255, 255, 255), overlay);

      ImGui::PopStyleColor(2);
    }

    // Status messages
    auto status = m_exporter.get_status();
    if (status == GameExporter::Status::Done)
    {
      ImGui::Spacing();
      ImGui::SetCursorPosX(ImGui::GetCursorPosX());
      ImGui::TextColored(ImVec4(0.30f, 0.85f, 0.40f, 1.0f), "  Export complete!");
    }
    else if (status == GameExporter::Status::Failed)
    {
      ImGui::Spacing();
      ImGui::SetCursorPosX(ImGui::GetCursorPosX());
      ImGui::TextColored(ImVec4(0.90f, 0.30f, 0.30f, 1.0f),
                         "  ERROR: %s",
                         m_exporter.get_error_message().c_str());
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.50f, 1.0f), "LOG");
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.09f, 0.11f, 1.0f));
    ImGui::BeginChild("##log", ImVec2(0, 0), true);
    {
      const auto& log = m_exporter.get_log();
      for (size_t i = 0; i < log.size(); i++)
      {
        const auto& line = log[i];

        if (line.find("ERROR") != std::string::npos)
          ImGui::TextColored(ImVec4(0.90f, 0.30f, 0.30f, 1.0f), "%s", line.c_str());
        else if (line.find("WARNING") != std::string::npos)
          ImGui::TextColored(ImVec4(0.90f, 0.75f, 0.20f, 1.0f), "%s", line.c_str());
        else if (line.find("===") != std::string::npos)
          ImGui::TextColored(ImVec4(0.26f, 0.59f, 0.98f, 1.0f), "%s", line.c_str());
        else if (line.find("  +") != std::string::npos || line.find("  ->") != std::string::npos)
          ImGui::TextColored(ImVec4(0.60f, 0.60f, 0.60f, 1.0f), "%s", line.c_str());
        else
          ImGui::Text("%s", line.c_str());
      }

      if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
  }
  ImGui::EndChild();
  ImGui::PopStyleVar();
}
}  // namespace ag
