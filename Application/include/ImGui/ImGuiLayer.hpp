#pragma once

#include <Layers/Layer.hpp>


namespace aero
{
  class ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer();
    ~ImGuiLayer();

    void on_attach() override;
    void on_detach() override;

    void on_update();
    void on_event(Event& event);

    void begin();
    void end();

    void on_imgui_render() override;
  private:
    float m_time = 0.0f;
    bool m_block_events = true;
  };
}
