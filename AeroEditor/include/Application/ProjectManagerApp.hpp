#pragma once

#include <Application/AppSettings.hpp>
#include <Application/ProjectManagerLayer.hpp>

namespace ag
{
  class ProjectManagerApp final : public ag::Application
  {
  public:
    ProjectManagerApp();
    virtual ~ProjectManagerApp() override {}


    virtual void on_create() override;
    virtual void on_destroy() override;
  };
}