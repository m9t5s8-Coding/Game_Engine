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

    void set_engine_theme();

    void on_imgui_render() override;

    void block_events(bool block) { m_block_events = block; }
  private:
    float m_time = 0.0f;
    bool m_block_events = true;


    void set_cyberpunk_theme();
    void set_warm_amber_theme();
    void set_ocean_blue_theme();
    void set_blood_red_theme();
    void set_monochrome_slate_theme();
  };
}
