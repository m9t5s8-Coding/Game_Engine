#pragma once

#include <Aero.hpp>
#include <chrono>
#include <vector>

namespace ag {
struct ProjectEntry {
  std::string                           name;
  std::string                           path;
  std::string                           last_modified;
  std::chrono::system_clock::time_point timestamp;
};

class ProjectManagerLayer : public ag::Layer {
public:
  ProjectManagerLayer();
  virtual ~ProjectManagerLayer() = default;

  virtual void on_attach() override;
  virtual void on_detach() override;

  virtual void on_update(ag::TimeStamp ts) override;
  virtual void on_imgui_render() override;
  virtual void on_event(ag::Event& e) override;

private:
  void create_new_project();
  void open_existing_project();
  void open_project(const std::string path);

  void        load_projects();
  void        save_projects();
  void        add_recent_project(const std::string name, std::string path);
  std::string format_date_time(const std::chrono::system_clock::time_point& time);

  bool on_window_close(WindowCloseEvent& e);

  std::vector<ProjectEntry> m_projects;
  std::string               m_projects_file_path;
};
}  // namespace ag
