#pragma once

#include <Aero.hpp>


namespace ag
{
  class ProjectManagerLayer : public ag::Layer
  {
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


  };
}
