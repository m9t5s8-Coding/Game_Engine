#pragma once

#ifdef AERO_EDITOR

  #include <imgui.h>
  #include <spdlog/sinks/base_sink.h>
  #include <spdlog/spdlog.h>

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
    std::string               text;
    spdlog::level::level_enum level;
    std::string               logger_name;
    int                       count = 1;
  };

  explicit ImGuiConsoleSink(size_t max_messages = 5000);
  ~ImGuiConsoleSink() override = default;

  void Draw(const char* title = "Console", bool* p_open = nullptr);
  void clear();

protected:
  void sink_it_(const spdlog::details::log_msg& msg) override;
  void flush_() override;

private:
  std::vector<Entry> m_messages;
  size_t             m_max_messages;

  char m_filter[256]      = "";
  bool m_auto_scroll      = true;
  bool m_scroll_to_bottom = false;
  int  m_selected         = -1;
};

}  // namespace ag

#endif
