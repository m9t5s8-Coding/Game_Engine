#pragma once

#include <Aero.hpp>


namespace ag
{
  class Sandbox2D : public ag::Layer
  {
  public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;

    virtual void on_update(TimeStamp ts) override;


  private:

    AG_ref<ViewController> m_view_controller;
    AG_ref<Scene> m_scene;

  };
}
