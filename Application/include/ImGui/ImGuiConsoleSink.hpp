#pragma once

#ifdef AERO_EDITOR

  #include <imgui.h>
  #include <spdlog/sinks/base_sink.h>
  #include <spdlog/spdlog.h>

  #include <array>
  #include <mutex>
  #include <string>
  #include <vector>

namespace ag
{

class ImGuiConsoleSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
  struct Entry
  {
    std::string               message;
    std::string               logger_name;
    std::string               timestamp;
    spdlog::level::level_enum level = spdlog::level::info;
    int                       count = 1;
  };

  explicit ImGuiConsoleSink(size_t max_messages = 5000);
  ~ImGuiConsoleSink() override = default;

  void Draw(const char* title = "Console", bool* p_open = nullptr);
  void clear();
  size_t count_for_level(spdlog::level::level_enum level) const;

protected:
  void sink_it_(const spdlog::details::log_msg& msg) override;
  void flush_() override;

private:
  bool passes_filter(const Entry& entry) const;
  void copy_visible_to_clipboard();

  std::vector<Entry> m_messages;
  size_t             m_max_messages = 5000;

  char m_filter[256] = "";
  bool m_auto_scroll = true;
  bool m_scroll_to_bottom = false;
  bool m_collapse = true;
  bool m_show_timestamps = true;
  bool m_show_logger = true;
  int  m_selected = -1;

  std::array<bool, spdlog::level::n_levels> m_level_enabled{};
  std::array<size_t, spdlog::level::n_levels> m_level_counts{};
};

}  // namespace ag

#endif
