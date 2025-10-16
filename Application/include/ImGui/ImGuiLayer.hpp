#pragma once

#include <Layers/Layer.hpp>


namespace ag
{
  class ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer();
    ~ImGuiLayer();

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_event(Event& event) override;

    void begin();
    void end();

    void on_imgui_render() override;
  private:
    float m_time = 0.0f;
    bool m_block_events = true;
  };
}
