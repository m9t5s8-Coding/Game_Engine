#pragma once

#include <spdlog/spdlog.h>
#include <mutex>
#include <vector>
#include <string>
#include <imgui.h>
#include <chrono>

namespace ag
{
  class ImGuiConsoleSink
  {
  public:
    struct Message
    {
      std::string text;
      spdlog::level::level_enum level;
      std::string timestamp;
    };

    ImGuiConsoleSink(size_t max_messages = 1000) : m_max_messages(max_messages) {}

    void AddLog(const spdlog::details::log_msg& msg)
    {
      spdlog::memory_buf_t formatted;
      if (m_formatter)
        m_formatter->format(msg, formatted);
      else
        formatted.append(msg.payload.data(), msg.payload.data() + msg.payload.size());

      // Format timestamp
      auto time_t = std::chrono::system_clock::to_time_t(msg.time);
      std::tm tm;

#ifdef _WIN32
      localtime_s(&tm, &time_t);
#else
      localtime_r(&time_t, &tm);
#endif

      char time_buffer[64];
      strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &tm);

      {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messages.push_back({ fmt::to_string(formatted), msg.level, std::string(time_buffer) });

        if (m_messages.size() > m_max_messages)
          m_messages.erase(m_messages.begin());
      }
    }

    void Draw(const char* title, bool* p_open = nullptr)
    {
      ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
      if (!ImGui::Begin(title, p_open))
      {
        ImGui::End();
        return;
      }

      if (ImGui::Button("Clear"))
        Clear();
      ImGui::SameLine();

      bool copy = ImGui::Button("Copy");
      ImGui::SameLine();
      m_filter.Draw("Filter", -100.0f);

      ImGui::Separator();

      ImGui::Checkbox("Auto-scroll", &m_auto_scroll);
      ImGui::SameLine();
      ImGui::Checkbox("Show timestamps", &m_show_timestamps);
      ImGui::SameLine();
      ImGui::Checkbox("Show level", &m_show_level);

      ImGui::Separator();

      const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
      ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

      if (copy)
        ImGui::LogToClipboard();

      {
        std::lock_guard<std::mutex> lock(m_mutex);

        for (const auto& message : m_messages)
        {
          if (!m_filter.PassFilter(message.text.c_str()))
            continue;

          ImVec4 color = GetColorForLevel(message.level);
          ImGui::PushStyleColor(ImGuiCol_Text, color);

          std::string display_text;
          if (m_show_timestamps && m_show_level)
          {
            // Convert string_view to string
            std::string level_str = spdlog::level::to_short_c_str(message.level);
            display_text = "[" + message.timestamp + "][" + level_str + "] " + message.text;
          }
          else if (m_show_timestamps)
          {
            display_text = "[" + message.timestamp + "] " + message.text;
          }
          else if (m_show_level)
          {
            std::string level_str = spdlog::level::to_short_c_str(message.level);
            display_text = "[" + level_str + "] " + message.text;
          }
          else
          {
            display_text = message.text;
          }

          ImGui::TextUnformatted(display_text.c_str());
          ImGui::PopStyleColor();
        }
      }

      if (copy)
        ImGui::LogFinish();

      if (m_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

      ImGui::EndChild();
      ImGui::End();
    }

    void Clear()
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_messages.clear();
    }

    void set_formatter(std::unique_ptr<spdlog::formatter> formatter)
    {
      m_formatter = std::move(formatter);
    }

  private:
    std::vector<Message> m_messages;
    size_t m_max_messages;
    bool m_auto_scroll = true;
    bool m_show_timestamps = true;
    bool m_show_level = true;
    ImGuiTextFilter m_filter;
    std::mutex m_mutex;
    std::unique_ptr<spdlog::formatter> m_formatter;

    ImVec4 GetColorForLevel(spdlog::level::level_enum level) const
    {
      switch (level)
      {
      case spdlog::level::trace:    return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
      case spdlog::level::debug:    return ImVec4(0.0f, 0.5f, 1.0f, 1.0f);
      case spdlog::level::info:     return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
      case spdlog::level::warn:     return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
      case spdlog::level::err:      return ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
      case spdlog::level::critical: return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
      default:                      return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
      }
    }
  };
}