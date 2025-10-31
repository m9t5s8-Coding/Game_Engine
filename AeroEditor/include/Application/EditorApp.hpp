#pragma once

#include <Application/AppSettings.hpp>
#include <Application/EditorLayer.hpp>

namespace ag
{
  class EditorApp final : public Application
  {
  public:
    EditorApp();
    virtual ~EditorApp() override {}


    virtual void on_create() override;
    virtual void on_destroy() override;

  private:
    void save_json();
    void load_project_data();

  };
}