#pragma once

#include <Application/AppSettings.hpp>
#include <Application/EditorLayer.hpp>

namespace ag {
class EditorApp final : public Application {
public:
  explicit EditorApp(std::string project_path = std::string());
  virtual ~EditorApp() override {}

  virtual void on_create() override;
  virtual void on_destroy() override;

private:
  void save_json();
  void load_project_data();

  std::string m_project_path;
};
}  // namespace ag
