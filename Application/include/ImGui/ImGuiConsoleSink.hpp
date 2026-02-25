#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <mutex>
#include <string>
#include <vector>

struct ImVec4;
namespace ag
{

class ImGuiConsoleSink
{
public:
  struct Message
  {
    std::string               text;
    spdlog::level::level_enum level;
    std::string               timestamp;
    std::string               logger_name;
    size_t                    count = 1;
  };

  explicit ImGuiConsoleSink(size_t max_messages = 1000);

  void AddLog(const spdlog::details::log_msg& msg);
  void Draw(const char* title, bool* p_open = nullptr);

  void Clear();
  void set_formatter(std::unique_ptr<spdlog::formatter> formatter);

  size_t GetMessageCount() const;

private:
  std::vector<Message>               m_messages;
  size_t                             m_max_messages;
  bool                               m_auto_scroll         = true;
  bool                               m_show_timestamps     = true;
  bool                               m_show_logger_name    = true;
  bool                               m_collapse_duplicates = false;
  mutable std::mutex                 m_mutex;
  std::unique_ptr<spdlog::formatter> m_formatter;
  int                                m_selected_message = -1;

  // helper functions
  ImVec4 GetColorForLevel(spdlog::level::level_enum level) const;

  void ClearAbove(int index);
  void ClearBelow(int index);
  void ExportToFile();
};

}  // namespace ag
