#ifdef AERO_EDITOR

  #include <imgui.h>

  #include <ImGui/ImGuiConsoleSink.hpp>

  #include <algorithm>
  #include <cctype>
  #include <chrono>
  #include <cstdio>
  #include <ctime>

namespace ag
{

namespace
{
constexpr ImVec4 k_level_colors[] = {
    {0.60f, 0.60f, 0.60f, 1.00f},  // trace
    {0.55f, 0.78f, 1.00f, 1.00f},  // debug
    {0.45f, 0.90f, 0.55f, 1.00f},  // info
    {1.00f, 0.78f, 0.30f, 1.00f},  // warn
    {1.00f, 0.42f, 0.42f, 1.00f},  // error
    {1.00f, 0.35f, 0.85f, 1.00f},  // critical
    {0.80f, 0.80f, 0.80f, 1.00f},  // off (unused)
};

constexpr const char* k_level_names[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "OFF",
};

constexpr const char* k_level_short[] = {
    "T", "D", "I", "W", "E", "F", "O",
};

std::string to_lower_copy(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return s;
}

bool contains_icase(const std::string& haystack, const std::string& needle)
{
  if (needle.empty())
    return true;
  const std::string h = to_lower_copy(haystack);
  const std::string n = to_lower_copy(needle);
  return h.find(n) != std::string::npos;
}

int level_index(spdlog::level::level_enum lvl)
{
  int idx = static_cast<int>(lvl);
  if (idx < 0)
    idx = 0;
  if (idx >= static_cast<int>(spdlog::level::n_levels))
    idx = static_cast<int>(spdlog::level::n_levels) - 1;
  return idx;
}

std::string format_timestamp(const spdlog::details::log_msg& msg)
{
  using namespace std::chrono;
  const auto time    = msg.time;
  const auto tt      = system_clock::to_time_t(time);
  const auto ms      = duration_cast<milliseconds>(time.time_since_epoch()) % 1000;

  std::tm tm{};
#if defined(_WIN32)
  localtime_s(&tm, &tt);
#else
  localtime_r(&tt, &tm);
#endif

  char buf[32];
  std::snprintf(buf,
                sizeof(buf),
                "%02d:%02d:%02d.%03d",
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec,
                static_cast<int>(ms.count()));
  return std::string(buf);
}
}  // namespace

ImGuiConsoleSink::ImGuiConsoleSink(size_t max_messages)
  : m_max_messages(max_messages)
{
  m_messages.reserve(max_messages);
  m_level_enabled.fill(true);
  m_level_counts.fill(0);
}

void ImGuiConsoleSink::sink_it_(const spdlog::details::log_msg& msg)
{
  const int idx = level_index(msg.level);
  if (idx >= 0 && idx < static_cast<int>(m_level_counts.size()))
    m_level_counts[idx]++;

  // Extract payload as the actual log message.
  const std::string payload(msg.payload.data(), msg.payload.size());
  const std::string logger(msg.logger_name.data(), msg.logger_name.size());
  const std::string ts = format_timestamp(msg);

  if (m_collapse && !m_messages.empty())
  {
    auto& last = m_messages.back();
    if (last.message == payload && last.level == msg.level && last.logger_name == logger)
    {
      last.count++;
      last.timestamp = ts;
      m_scroll_to_bottom = true;
      return;
    }
  }

  Entry e;
  e.message     = payload;
  e.level       = msg.level;
  e.logger_name = logger;
  e.timestamp   = ts;

  if (m_messages.size() >= m_max_messages)
  {
    const auto removed_level = level_index(m_messages.front().level);
    if (m_level_counts[removed_level] > 0)
      m_level_counts[removed_level]--;
    m_messages.erase(m_messages.begin());
    if (m_selected > 0)
      m_selected--;
    else if (m_selected == 0)
      m_selected = -1;
  }

  m_messages.push_back(std::move(e));
  m_scroll_to_bottom = true;
}

void ImGuiConsoleSink::flush_()
{
}

void ImGuiConsoleSink::clear()
{
  std::lock_guard<std::mutex> lock(base_sink<std::mutex>::mutex_);
  m_messages.clear();
  m_level_counts.fill(0);
  m_selected = -1;
}

size_t ImGuiConsoleSink::count_for_level(spdlog::level::level_enum level) const
{
  const int idx = level_index(level);
  if (idx < 0 || idx >= static_cast<int>(m_level_counts.size()))
    return 0;
  return m_level_counts[idx];
}

bool ImGuiConsoleSink::passes_filter(const Entry& entry) const
{
  const int idx = level_index(entry.level);
  if (idx >= 0 && idx < static_cast<int>(m_level_enabled.size()) && !m_level_enabled[idx])
    return false;

  if (m_filter[0] == '\0')
    return true;

  const std::string needle = m_filter;
  if (contains_icase(entry.message, needle))
    return true;
  if (contains_icase(entry.logger_name, needle))
    return true;
  if (contains_icase(k_level_names[idx], needle))
    return true;
  return false;
}

void ImGuiConsoleSink::copy_visible_to_clipboard()
{
  std::string out;
  out.reserve(m_messages.size() * 48);
  for (const auto& e : m_messages)
  {
    if (!passes_filter(e))
      continue;
    const int idx = level_index(e.level);
    out += '[';
    out += k_level_names[idx];
    out += "] ";
    if (!e.logger_name.empty())
    {
      out += '[';
      out += e.logger_name;
      out += "] ";
    }
    out += e.message;
    if (e.count > 1)
    {
      out += " (x";
      out += std::to_string(e.count);
      out += ')';
    }
    out += '\n';
  }
  ImGui::SetClipboardText(out.c_str());
}

void ImGuiConsoleSink::Draw(const char* title, bool* p_open)
{
  if (p_open && !*p_open)
    return;

  ImGui::SetNextWindowSize(ImVec2(900.0f, 320.0f), ImGuiCond_FirstUseEver);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));

  const bool open = ImGui::Begin(title, p_open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
  ImGui::PopStyleVar();

  if (!open)
  {
    ImGui::End();
    return;
  }

  // Toolbar
  {
    if (ImGui::Button("Clear"))
      clear();
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Clear all console messages");

    ImGui::SameLine();
    if (ImGui::Button("Copy"))
      copy_visible_to_clipboard();
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Copy visible log lines to clipboard");

    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &m_auto_scroll);
    ImGui::SameLine();
    ImGui::Checkbox("Collapse", &m_collapse);
    ImGui::SameLine();
    ImGui::Checkbox("Time", &m_show_timestamps);
    ImGui::SameLine();
    ImGui::Checkbox("Logger", &m_show_logger);

    ImGui::SameLine();
    ImGui::SetNextItemWidth(220.0f);
    ImGui::InputTextWithHint("##console_filter", "Filter logs...", m_filter, sizeof(m_filter));

    ImGui::SameLine();
    if (ImGui::SmallButton("X##clear_filter"))
      m_filter[0] = '\0';

    ImGui::SameLine();
    ImGui::TextDisabled("| %zu entries", m_messages.size());
  }

  // Level toggles row
  {
    const spdlog::level::level_enum levels[] = {
        spdlog::level::trace,
        spdlog::level::debug,
        spdlog::level::info,
        spdlog::level::warn,
        spdlog::level::err,
        spdlog::level::critical,
    };

    for (size_t i = 0; i < IM_ARRAYSIZE(levels); ++i)
    {
      if (i > 0)
        ImGui::SameLine();

      const int idx = level_index(levels[i]);
      const ImVec4 col = k_level_colors[idx];
      char label[64];
      std::snprintf(label, sizeof(label), "%s (%zu)##lvl_%d", k_level_names[idx], m_level_counts[idx], idx);

      ImGui::PushStyleColor(ImGuiCol_Text, col);
      ImGui::PushStyleColor(ImGuiCol_CheckMark, col);
      ImGui::Checkbox(label, &m_level_enabled[idx]);
      ImGui::PopStyleColor(2);
    }
  }

  ImGui::Separator();
  ImGui::Spacing();

  // Message list
  ImGui::BeginChild("##console_log_child", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_HorizontalScrollbar);

  std::lock_guard<std::mutex> lock(base_sink<std::mutex>::mutex_);

  int visible_index = 0;
  for (int i = 0; i < static_cast<int>(m_messages.size()); ++i)
  {
    const auto& e = m_messages[i];
    if (!passes_filter(e))
      continue;

    const int idx = level_index(e.level);
    const bool selected = (m_selected == i);

    ImGui::PushID(i);

    // Row background for severity
    if (e.level >= spdlog::level::err)
    {
      ImVec2 row_min = ImGui::GetCursorScreenPos();
      ImVec2 row_max = ImVec2(row_min.x + ImGui::GetContentRegionAvail().x, row_min.y + ImGui::GetTextLineHeightWithSpacing());
      ImU32 bg = (e.level >= spdlog::level::critical)
                     ? IM_COL32(90, 20, 60, 45)
                     : IM_COL32(90, 30, 30, 40);
      ImGui::GetWindowDrawList()->AddRectFilled(row_min, row_max, bg);
    }
    else if (e.level == spdlog::level::warn)
    {
      ImVec2 row_min = ImGui::GetCursorScreenPos();
      ImVec2 row_max = ImVec2(row_min.x + ImGui::GetContentRegionAvail().x, row_min.y + ImGui::GetTextLineHeightWithSpacing());
      ImGui::GetWindowDrawList()->AddRectFilled(row_min, row_max, IM_COL32(90, 70, 20, 30));
    }

    if (ImGui::Selectable("##log_row", selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick))
    {
      m_selected = i;
      if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
      {
        std::string text = "[" + std::string(k_level_names[idx]) + "] " + e.message;
        ImGui::SetClipboardText(text.c_str());
      }
    }
    ImGui::SameLine();

    // Timestamp
    if (m_show_timestamps)
    {
      ImGui::TextDisabled("%s", e.timestamp.c_str());
      ImGui::SameLine();
    }

    // Level badge
    ImGui::TextColored(k_level_colors[idx], "[%s]", k_level_names[idx]);
    ImGui::SameLine();

    // Logger
    if (m_show_logger && !e.logger_name.empty())
    {
      ImGui::TextColored(ImVec4(0.70f, 0.72f, 0.80f, 1.0f), "%s", e.logger_name.c_str());
      ImGui::SameLine();
      ImGui::TextDisabled("|");
      ImGui::SameLine();
    }

    // Message
    ImVec4 msg_col = k_level_colors[idx];
    if (e.level == spdlog::level::info)
      msg_col = ImVec4(0.92f, 0.94f, 0.96f, 1.0f);
    else if (e.level == spdlog::level::debug)
      msg_col = ImVec4(0.78f, 0.86f, 0.95f, 1.0f);
    else if (e.level == spdlog::level::trace)
      msg_col = ImVec4(0.72f, 0.72f, 0.72f, 1.0f);

    ImGui::TextColored(msg_col, "%s", e.message.c_str());

    if (e.count > 1)
    {
      ImGui::SameLine();
      ImGui::TextColored(ImVec4(0.55f, 0.60f, 0.70f, 1.0f), "(x%d)", e.count);
    }

    if (ImGui::IsItemHovered())
    {
      ImGui::BeginTooltip();
      ImGui::TextColored(k_level_colors[idx], "%s", k_level_names[idx]);
      ImGui::Separator();
      ImGui::TextUnformatted(e.message.c_str());
      if (e.count > 1)
        ImGui::TextDisabled("Repeated %d times", e.count);
      ImGui::EndTooltip();
    }

    ImGui::PopID();
    visible_index++;
  }

  if (visible_index == 0)
  {
    ImGui::TextDisabled("No log messages match the current filters.");
  }

  if (m_auto_scroll && m_scroll_to_bottom && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 20.0f)
  {
    ImGui::SetScrollHereY(1.0f);
  }
  if (m_scroll_to_bottom)
  {
    if (m_auto_scroll)
      ImGui::SetScrollHereY(1.0f);
    m_scroll_to_bottom = false;
  }

  ImGui::EndChild();
  ImGui::End();
}

}  // namespace ag

#endif
