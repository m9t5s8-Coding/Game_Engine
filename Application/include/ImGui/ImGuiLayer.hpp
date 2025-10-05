#pragma once

#include <Layers/Layer.hpp>

namespace aero
{
  class ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer();
    ~ImGuiLayer();

    void on_attack();
    void on_detach();

    void on_update();
    void on_event(Event& event);
  private:
  };
}
