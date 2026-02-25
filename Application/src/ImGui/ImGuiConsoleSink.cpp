#include <fmt/core.h>
#include <imgui.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <ImGui/ImGuiConsoleSink.hpp>

namespace ag
{

ImGuiConsoleSink::ImGuiConsoleSink(size_t max_messages)
  : m_max_messages(max_messages)
{
}

void ImGuiConsoleSink::AddLog(const spdlog::details::log_msg& msg)
{
  spdlog::memory_buf_t formatted;
  if (m_formatter)
    m_formatter->format(msg, formatted);
  else
    formatted.append(msg.payload.data(), msg.payload.data() + msg.payload.size());

  // Format timestamp
  auto    time_t = std::chrono::system_clock::to_time_t(msg.time);
  std::tm tm;

#ifdef _WIN32
  localtime_s(&tm, &time_t);
#else
  localtime_r(&time_t, &tm);
#endif

  char time_buffer[64];
  strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &tm);

  std::string log_text    = fmt::to_string(formatted);
  std::string logger_name = std::string(msg.logger_name.begin(), msg.logger_name.end());

  {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_collapse_duplicates && !m_messages.empty() && m_messages.back().text == log_text &&
        m_messages.back().level == msg.level && m_messages.back().logger_name == logger_name)
    {
      m_messages.back().count++;
      m_messages.back().timestamp = std::string(time_buffer);
    }
    else
    {
      m_messages.push_back({log_text, msg.level, std::string(time_buffer), logger_name, 1});
    }

    if (m_messages.size() > m_max_messages)
      m_messages.erase(m_messages.begin());
  }
}

void ImGuiConsoleSink::Draw(const char* title, bool* p_open)
{
  ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
  if (!ImGui::Begin(title, p_open))
  {
    ImGui::End();
    return;
  }

  // Top toolbar
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 4.0f));

  if (ImGui::Button("Clear"))
    Clear();

  ImGui::SameLine();
  bool copy = ImGui::Button("Copy All");

  ImGui::SameLine();
  if (ImGui::Button("Export"))
    ExportToFile();

  ImGui::SameLine();
  ImGui::Text("Messages: %zu", GetMessageCount());

  ImGui::PopStyleVar(2);

  // Log content area
  const float footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
  ImGui::BeginChild("ScrollingRegion",
                    ImVec2(0, -footer_height),
                    false,
                    ImGuiWindowFlags_HorizontalScrollbar |
                        ImGuiWindowFlags_AlwaysVerticalScrollbar);

  if (copy)
    ImGui::LogToClipboard();

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));

  {
    std::lock_guard<std::mutex> lock(m_mutex);

    ImGuiListClipper clipper;
    clipper.Begin((int)m_messages.size());

    while (clipper.Step())
    {
      for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
      {
        const auto& message = m_messages[i];

        std::string display_text;

        if (m_show_timestamps)
          display_text += "[" + message.timestamp + "] ";

        if (!message.logger_name.empty())
          display_text += "[" + message.logger_name + "] : ";

        display_text += message.text;

        if (message.count > 1)
          display_text += " (x" + std::to_string(message.count) + ")";

        ImGui::TextUnformatted(display_text.c_str());

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
        {
          m_selected_message = i;
          ImGui::OpenPopup("LogContextMenu");
        }
      }
    }
  }

  ImGui::PopStyleVar();

  // Context menu
  if (ImGui::BeginPopup("LogContextMenu"))
  {
    if (m_selected_message >= 0 && m_selected_message < (int)m_messages.size())
    {
      const auto& msg = m_messages[m_selected_message];

      if (ImGui::MenuItem("Copy Message"))
        ImGui::SetClipboardText(msg.text.c_str());

      if (ImGui::MenuItem("Copy with Timestamp"))
      {
        std::string full_msg = "[" + msg.timestamp + "] " + msg.logger_name + ": " + msg.text;
        ImGui::SetClipboardText(full_msg.c_str());
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Clear All Above"))
        ClearAbove(m_selected_message);

      if (ImGui::MenuItem("Clear All Below"))
        ClearBelow(m_selected_message);
    }
    ImGui::EndPopup();
  }

  if (copy)
    ImGui::LogFinish();

  if (m_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    ImGui::SetScrollHereY(1.0f);

  ImGui::EndChild();

  ImGui::End();
}

void ImGuiConsoleSink::Clear()
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_messages.clear();
}

void ImGuiConsoleSink::set_formatter(std::unique_ptr<spdlog::formatter> formatter)
{
  m_formatter = std::move(formatter);
}

size_t ImGuiConsoleSink::GetMessageCount() const
{
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_messages.size();
}

ImVec4 ImGuiConsoleSink::GetColorForLevel(spdlog::level::level_enum level) const
{
  switch (level)
  {
    case spdlog::level::trace:
      return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    case spdlog::level::debug:
      return ImVec4(0.4f, 0.7f, 1.0f, 1.0f);
    case spdlog::level::info:
      return ImVec4(0.6f, 0.9f, 0.6f, 1.0f);
    case spdlog::level::warn:
      return ImVec4(1.0f, 0.8f, 0.3f, 1.0f);
    case spdlog::level::err:
      return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
    case spdlog::level::critical:
      return ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
    default:
      return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  }
}

void ImGuiConsoleSink::ClearAbove(int index)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  if (index > 0)
    m_messages.erase(m_messages.begin(), m_messages.begin() + index);
}

void ImGuiConsoleSink::ClearBelow(int index)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  if (index < (int)m_messages.size() - 1)
    m_messages.erase(m_messages.begin() + index + 1, m_messages.end());
}

void ImGuiConsoleSink::ExportToFile()
{
  std::lock_guard<std::mutex> lock(m_mutex);

  std::string all_logs;
  for (const auto& msg : m_messages)
  {
    all_logs += "[" + msg.timestamp + "] " + "[" + msg.logger_name + "] : " + msg.text + "\n";
  }
  ImGui::SetClipboardText(all_logs.c_str());
}

}  // namespace ag
