#ifdef AERO_EDITOR

  #include <imgui.h>

  #include <ImGui/ImGuiConsoleSink.hpp>

namespace ag
{

static const ImVec4 LEVEL_COLORS[] = {
    {0.5f, 0.5f, 0.5f, 1.0f}, // trace
    {0.6f, 0.6f, 0.6f, 1.0f}, // debug
    {0.2f, 0.8f, 0.2f, 1.0f}, // info
    {0.9f, 0.6f, 0.0f, 1.0f}, // warn
    {0.9f, 0.2f, 0.2f, 1.0f}, // error
    {0.8f, 0.2f, 0.8f, 1.0f}, // critical
};

static const char* LEVEL_NAMES[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

ImGuiConsoleSink::ImGuiConsoleSink(size_t max_messages)
  : m_max_messages(max_messages)
{
  m_messages.reserve(max_messages);
}

void ImGuiConsoleSink::sink_it_(const spdlog::details::log_msg& msg)
{
  spdlog::memory_buf_t buf;
  base_sink<std::mutex>::formatter_->format(msg, buf);

  Entry e;
  e.text        = fmt::to_string(buf);
  e.level       = msg.level;
  e.logger_name = std::string(msg.logger_name.data(), msg.logger_name.size());

  if (!m_messages.empty())
  {
    auto& last = m_messages.back();
    if (last.text == e.text && last.level == e.level)
    {
      last.count++;
      m_scroll_to_bottom = true;
      return;
    }
  }

  if (m_messages.size() >= m_max_messages)
    m_messages.erase(m_messages.begin());

  m_messages.push_back(std::move(e));
  m_scroll_to_bottom = true;
}

void ImGuiConsoleSink::flush_()
{
}

// ─── Public API ───────────────────────────────────────────────────────────────

void ImGuiConsoleSink::clear()
{
  std::lock_guard<std::mutex> lock(base_sink<std::mutex>::mutex_);
  m_messages.clear();
  m_selected = -1;
}

// ─── Draw ─────────────────────────────────────────────────────────────────────

void ImGuiConsoleSink::Draw(const char* title, bool* p_open)
{
  if (p_open && !*p_open)
    return;

  ImGui::SetNextWindowSize({800, 400}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin(title, p_open))
  {
    ImGui::End();
    return;
  }

  // ── Toolbar ──
  if (ImGui::Button("Clear"))
    clear();

  ImGui::SameLine();
  ImGui::Checkbox("Auto-scroll", &m_auto_scroll);

  ImGui::SameLine();
  ImGui::SetNextItemWidth(200);
  ImGui::InputText("Filter", m_filter, sizeof(m_filter));

  ImGui::SameLine();
  ImGui::TextDisabled("(%zu msgs)", m_messages.size());

  ImGui::Separator();

  // ── Message list ──
  ImGui::BeginChild("##log", {0, 0}, false, ImGuiWindowFlags_HorizontalScrollbar);

  std::lock_guard<std::mutex> lock(base_sink<std::mutex>::mutex_);

  for (int i = 0; i < (int)m_messages.size(); ++i)
  {
    const auto& e = m_messages[i];

    // Apply filter
    if (m_filter[0] != '\0' && e.text.find(m_filter) == std::string::npos)
      continue;

    int  idx = std::min((int)e.level, (int)std::size(LEVEL_COLORS) - 1);
    bool sel = (m_selected == i);

    ImGui::PushID(i);
    if (ImGui::Selectable("##sel", sel, ImGuiSelectableFlags_AllowDoubleClick))
    {
      m_selected = i;
      if (ImGui::IsMouseDoubleClicked(0))
        ImGui::SetClipboardText(e.text.c_str());
    }
    ImGui::SameLine();

    // Level tag
    ImGui::TextColored(LEVEL_COLORS[idx], "[%s]", LEVEL_NAMES[idx]);
    ImGui::SameLine();

    // Logger name
    if (!e.logger_name.empty())
    {
      ImGui::TextColored({0.6f, 0.6f, 1.0f, 1.0f}, "[%s]", e.logger_name.c_str());
      ImGui::SameLine();
    }

    // Duplicate count
    if (e.count > 1)
    {
      ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.0f}, "(%dx)", e.count);
      ImGui::SameLine();
    }

    ImGui::TextColored(LEVEL_COLORS[idx], "%s", e.text.c_str());
    ImGui::PopID();
  }

  if (m_auto_scroll && m_scroll_to_bottom)
  {
    ImGui::SetScrollHereY(1.0f);
    m_scroll_to_bottom = false;
  }

  ImGui::EndChild();
  ImGui::End();
}

}  // namespace ag

#endif
